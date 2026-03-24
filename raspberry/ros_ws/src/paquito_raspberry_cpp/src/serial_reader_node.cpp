#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/float32_multi_array.hpp>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>

class ArduinoSerialReader : public rclcpp::Node {
public:
    ArduinoSerialReader() : Node("arduino_serial_reader"), serial_port_(-1) {
        
        // Inicializar el publicador
        // Publicaremos en el tópico "wheel_velocities2" con un tamaño de cola de 10
        publisher_ = this->create_publisher<std_msgs::msg::Float32MultiArray>("wheel_velocities2", 10);

        // Configuración del puerto serial
        // En Raspberry Pi 4, los pines físicos RX/TX principales suelen estar en /dev/serial0
        // Tener habilitado el hardware serial en raspi-config
        std::string port_name = "/dev/serial0"; 
        
        serial_port_ = open(port_name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

        if (serial_port_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Error al abrir el puerto serial: %s. Revisa permisos (grupo dialout).", port_name.c_str());
            return;
        }

        struct termios tty;
        if (tcgetattr(serial_port_, &tty) != 0) {
            RCLCPP_ERROR(this->get_logger(), "Error obteniendo atributos del puerto serial.");
            return;
        }

        // Configurar baudrate a 115200 (Coincide con el Arduino)
        cfsetispeed(&tty, B115200);
        cfsetospeed(&tty, B115200);

        // Configuración estándar: 8N1 (8 bits de datos, sin paridad, 1 bit de parada)
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag |= CREAD | CLOCAL; // Habilitar lectura, ignorar líneas de control de módem
        
        // Modo crudo (raw mode) para leer los bytes tal cual llegan
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Apagar control de flujo por software
        tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Deshabilitar manejo especial de bytes
        tty.c_oflag &= ~OPOST; // Salida cruda
        
        if (tcsetattr(serial_port_, TCSANOW, &tty) != 0) {
            RCLCPP_ERROR(this->get_logger(), "Error aplicando configuración al puerto serial.");
            return;
        }

        // Crear el timer de lectura
        // Dado que el Arduino envía datos cada 500ms, leer a 100Hz (10ms) es suficientemente rápido
        // para vaciar el buffer sin causar cuellos de botella.
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&ArduinoSerialReader::read_serial_data, this)
        );

        RCLCPP_INFO(this->get_logger(), "Nodo iniciado. Escuchando en %s a 115200 baudios. Publicando en '/wheel_velocities2'", port_name.c_str());
    }

    ~ArduinoSerialReader() {
        if (serial_port_ >= 0) {
            close(serial_port_);
        }
    }

private:
    int serial_port_;
    std::string buffer_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float32MultiArray>::SharedPtr publisher_;

    void read_serial_data() {
        if (serial_port_ < 0) return;

        char read_buf[256];
        // Leer datos de forma no bloqueante
        int bytes_read = read(serial_port_, &read_buf, sizeof(read_buf));

        if (bytes_read > 0) {
            // Añadir lo leído al buffer acumulativo
            buffer_.append(read_buf, bytes_read);
            
            size_t pos = 0;
            // Buscar el carácter de salto de línea '\n' que indica el fin del mensaje de Arduino
            while ((pos = buffer_.find('\n')) != std::string::npos) {
                // Extraer la línea (sin el salto de línea)
                std::string line = buffer_.substr(0, pos);
                
                // Limpiar posibles retornos de carro '\r' (común en Serial.println de Arduino)
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }

                // Borrar la línea procesada del buffer
                buffer_.erase(0, pos + 1);
                
                // Procesar la línea si no está vacía
                if (!line.empty()) {
                    process_and_publish(line);
                }
            }
        }
    }

    void process_and_publish(const std::string& line) {
        // La línea esperada es "valFL,valFR,valBL,valBR" (ej: "13,14,14,13" o "13.5,14.2,-14.0,13.1")
        
        std::vector<float> velocities;
        std::stringstream ss(line);
        std::string token;

        // Separar la cadena por comas
        while (std::getline(ss, token, ',')) {
            try {
                // Convertir cada fragmento a número flotante y añadirlo al vector
                velocities.push_back(std::stof(token));
            } catch (const std::invalid_argument& e) {
                RCLCPP_WARN(this->get_logger(), "Dato basura recibido: no se pudo convertir a float '%s'", token.c_str());
                return; // Si hay basura, ignoramos toda la línea para evitar publicar datos incompletos
            } catch (const std::out_of_range& e) {
                RCLCPP_WARN(this->get_logger(), "Dato fuera de rango: '%s'", token.c_str());
                return;
            }
        }

        // Verificar que recibimos exactamente 4 valores
        if (velocities.size() == 4) {
            // Crear el mensaje y asignar los datos
            auto msg = std_msgs::msg::Float32MultiArray();
            msg.data = velocities;

            // Publicar el mensaje
            publisher_->publish(msg);

            // Debug (Puedes comentar esto más adelante para no llenar la terminal)
            RCLCPP_INFO(this->get_logger(), "Publicado: [FL: %.2f, FR: %.2f, BL: %.2f, BR: %.2f]", 
                        velocities[0], velocities[1], velocities[2], velocities[3]);
        } else {
             RCLCPP_WARN(this->get_logger(), "Trama incompleta/incorrecta. Se esperaban 4 valores, se recibieron %zu. Línea original: '%s'", velocities.size(), line.c_str());
        }
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ArduinoSerialReader>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}