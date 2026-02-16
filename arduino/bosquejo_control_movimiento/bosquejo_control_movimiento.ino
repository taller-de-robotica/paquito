/* 
 * P A Q U I T O
 * ___________________________________________________________________
 *
 * Control del robot desde una Raspberry utilizando I2C.
 * PAPIME PE104425 Visualización en 3D de Redes Neuronales.
 * @author blackzafiro
 * @author maxo237
 * @author AlevAriasV
 * @author Hazel
 *
 */
#include <Wire.h>
const int I2C_SLAVE_ADDRESS = 0x8; // Hexadecimal entre 8 y 127

// Voz
const int SPEAKER_PIN = 8;
const int LED_PIN = 13;

// Base Cámara
const int CAMERA_SERVO = 2;

// Llantas
const int speedPinR = 9;            // Front Wheel PWM pin connect Model-Y M_B ENA 
const int RightMotorDirPin1 = 26;   // Front Right Motor direction pin 1 to Model-Y M_B IN1 (K1)
const int RightMotorDirPin2 = 27;   // Front Right Motor direction pin 2 to Model-Y M_B IN2 (K1)
const int RightMotorS1PinRA = 30;   // Front Right Motor encoder Signal 1 Orange
const int RightMotorS2PinRA = 32;   // Front Right Motor encoder Signal 2 Green

const int LeftMotorDirPin1 = 28;   // Front Left Motor direction pin 1 to Model-Y M_B IN3  (K3)
const int LeftMotorDirPin2 = 29;   // Front Left Motor direction pin 2 to Model-Y M_B IN4  (K3)
const int LeftMotorS1PinLA = 31;    // Front Left Motor encoder Signal 1 Orange
const int LeftMotorS2PinLA = 33;    // Front Left Motor encoder Signal 2 Green
const int speedPinL = 10;           // Front Wheel PWM pin connect Model-Y M_B ENB

const int speedPinRB = 11;          // Rear Wheel PWM pin connect Left Model-Y M_A ENA 
const int RightMotorDirPin1B = 22;  // Rear Right Motor direction pin 1 to Model-Y M_A IN1 (K1)
const int RightMotorDirPin2B = 23;  // Rear Right Motor direction pin 2 to Model-Y M_A IN2 (K1) 
const int RightMotorS1PinRB = 34;   // Rear Right Motor encoder Signal 1 Orange
const int RightMotorS2PinRB = 36;   // Rear Right Motor encoder Signal 2 Green

const int LeftMotorDirPin1B = 24;  // Rear Left Motor direction pin 1 to Model-Y M_A IN3  (K3)
const int LeftMotorDirPin2B = 25;  // Rear Left Motor direction pin 2 to Model-Y M_A IN4  (K3)
const int LeftMotorS1PinLB = 35;    // Rear Left Motor encoder Signal 1 Orange
const int LeftMotorS2PinLB = 37;    // Rear Left Motor encoder Signal 2 Green
const int speedPinLB = 12;          // Rear Wheel PWM pin connect Model-Y M_A ENB


#include "Speak.h"
#include "Car.h"

const Wheel WHEELS[] = {
  Wheel(speedPinL,  LeftMotorDirPin1,   LeftMotorDirPin2),
  Wheel(speedPinR,  RightMotorDirPin1,  RightMotorDirPin2),
  Wheel(speedPinLB, LeftMotorDirPin1B,  LeftMotorDirPin2B),
  Wheel(speedPinRB, RightMotorDirPin1B, RightMotorDirPin2B)
};

const Encoder ENCODERS[] = {
  Encoder(LeftMotorS1PinLA,  LeftMotorS2PinLA),
  Encoder(RightMotorS1PinRA, RightMotorS2PinRA),
  Encoder(LeftMotorS1PinLB,  LeftMotorS2PinLB),
  Encoder(RightMotorS1PinRB, RightMotorS2PinRB)
};

Car paquito(WHEELS, ENCODERS);


// Frenar a paquito si no recibe nuevos comandos
const int16_t MAX_PWM = 255;
const int MAX_SPEED = 150;
const int MIN_SPEED = 50;

const float MAX_DECAY = 1.0;
const float SPEED_DECAY_RATE = 0.1;
float speed_decay = MAX_DECAY;

// Reduce la rapidez linealmente.
float decrease_speed_factor()
{
  float temp = speed_decay - SPEED_DECAY_RATE;
  speed_decay = max(temp, 0);
}

// Estado de movimiento
int speed = 70;
bool speak = false;


#include <Servo.h>
Servo cameraBaseServo;
int yaw_angle = 90;


// Inicializaciones
Voice voice(SPEAKER_PIN, LED_PIN);


// Comandos básicos

enum Command {
  STOP =             0b00000000,
  BRAKE =            0b11001100,
  ACCELERATE =       0b00110011,
  FORWARD =          0b00001111,
  NE =               0b00001010, //right turn
  RIGHT =            0b01101001, //SE already exists
  SEAST =            0b10100000, //right back
  BACKWARD =         0b11110000,
  SW =               0b01000001, //left back
  LEFT =             0b10010110,
  NW =               0b00000101, //left turn
  CLOCKWISE =        0b01011010, //clockwise
  COUNTCLOCKWISE =   0b10100101, //countclockwise
  SPEAK =            0b00010001,
  MOVE_CAMERA =      0b00100010, //spin camera servo (1 param)
  SET_WHEELS_SPEED = 0b11111111, //manually set speeds per wheel (4 params)
};

// Declara el prototipo manualmente aquí:
void toCharArray(Command c, const char *commandName, int len);

void toCharArray(Command c, const char *commandName, int len) {
  String commandString;
  switch(c) {
    case STOP:
      commandString = "stop";
      break;
    case BRAKE:
      commandString = "brake";
      break;
    case ACCELERATE:
      commandString = "accelerate";
      break;
    case FORWARD:
      commandString = "forward";
      break;
    case NE:
      commandString = "NE";
      break;
    case RIGHT:
      commandString = "right";
      break;
    case SEAST:
      commandString = "SE";
      break;
    case BACKWARD:
      commandString = "backward";
      break;
    case SW:
      commandString = "SW";
      break;
    case LEFT:
      commandString = "left";
      break;
    case NW:
      commandString = "NW";
      break;
    case CLOCKWISE:
      commandString = "clockwise";
      break;
    case COUNTCLOCKWISE:
      commandString = "countclockwise";
      break;
    case SPEAK:
      commandString = "speak";
      break;
    case MOVE_CAMERA:
      commandString = "move_camera";
      break;
    case SET_WHEELS_SPEED:
      commandString = "set_wheels_speed";
      break; 
    default:
      commandString = "unknown";
  }
  commandString.toCharArray(commandName, len);
}


void execute(Command c, unsigned char args[]);

void execute(Command c, unsigned char args[]) {
  switch(c) {
    case STOP:
#ifdef DEBUG_PAQUITO
      Serial.println("--> Detente");
#else
      paquito.stop();
#endif
      break;
    case BRAKE:
#ifdef DEBUG_PAQUITO
      Serial.println("--> brake");
#else
      if (speed > MIN_SPEED) {
        speed--;
      }
#endif
      break;
    case ACCELERATE:
#ifdef DEBUG_PAQUITO
      Serial.println("--> accelerate");
#else
      if (speed < MAX_SPEED) {
        speed++;
      }
#endif
      break;
    case FORWARD:
#ifdef DEBUG_PAQUITO
      Serial.println("--> Avanza " + args[0]);
#else
      paquito.moveForward(speed);
#endif
      break;
    case NE:
#ifdef DEBUG_PAQUITO
      Serial.println("--> NE");
#else
      paquito.moveNE(speed);
#endif
      break;
    case RIGHT:
#ifdef DEBUG_PAQUITO
      Serial.println("--> right");
#else
      paquito.moveRight(speed);
#endif
      break;
    case SEAST:
#ifdef DEBUG_PAQUITO
      Serial.println("--> SE");
#else
      paquito.moveSE(speed);
#endif
      break;
    case BACKWARD:
#ifdef DEBUG_PAQUITO
      Serial.println("--> Retrocede " + args[0]);
#else
      paquito.moveBackward(speed);
#endif
      break;
    case SW:
#ifdef DEBUG_PAQUITO
      Serial.println("--> SW");
#else
      paquito.moveSW(speed);
#endif
      break;
    case LEFT:
#ifdef DEBUG_PAQUITO
      Serial.println("--> left");
#else
      paquito.moveLeft(speed);
#endif
      break;
    case NW:
#ifdef DEBUG_PAQUITO
      Serial.println("--> NW");
#else
      paquito.moveNW(speed);
#endif
      break;
    case CLOCKWISE:
#ifdef DEBUG_PAQUITO
      Serial.println("--> clockwise");
#else
      paquito.rotateClockwise(speed);
#endif
      break;
    case COUNTCLOCKWISE:
#ifdef DEBUG_PAQUITO
      Serial.println("--> countclockwise");
#else
      paquito.rotateCounterClockwise(speed);
#endif
      break;
    case SPEAK:
      Serial.println("--> Dí algo " + args[0]);
      speak = true;
      break;
    case MOVE_CAMERA:
      Serial.println("--> Mover la cámara " + args[0]);
      //sonar_around = true;
      break;
    case SET_WHEELS_SPEED:
      Serial.println("--> Asigna velocidades a las llanatas " + args[0]);
      // do stuff here
      break;
    default:
      Serial.print("--> Comando desconocido");
      Serial.println(c);
  }
  delay(100); // 0.1mm
}

// Protocolo para recibir velocidades por I2C

// Estructura para recibir los datos (4 enteros cortos = 8 bytes)
union Packet {
  struct {
    int16_t fr;
    int16_t rr;
    int16_t fl;
    int16_t rl;
  } val;
  byte bytes[8];
};

Packet rxData;
volatile bool newData = false;
int16_t wheel_speeds[Car::NUM_WHEELS];

// Variable para temporizador de impresión en el PC (evita usar delays)
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);
  
  // Servo de cámara
  cameraBaseServo.attach(CAMERA_SERVO);
  cameraBaseServo.write(90);

  // Inicializar habla
  randomSeed(analogRead(0));
  voice.begin();

  // Inicializar carro
  paquito.begin();
  Serial.println("*.´`. paquito init .´`.*");

  // Inicializar comunicación I2C
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);

  voice.babble();
}

// Se asegura de que la velocidad de cada rueda no supere
// el máximo,
// si alguna rueda va más rápido escala linealmente
// todas las velocidades para dejarla en el máximo,
// tratando de conservar la dirección de la velocidad
// del carrito.
void limitSpeed(int16_t signedSpeeds[Car::NUM_WHEELS]){
  int16_t max_value = MAX_PWM;
  for (int i = 0; i < Car::NUM_WHEELS; i++) {
    int16_t val = signedSpeeds[i];  // abs no debe usar [] adentro
    if (abs(val) > max_value) {
      max_value = abs(val);
    }
  }
  if (max_value > MAX_PWM) {
    float tasa = ((float)MAX_PWM) / ((float)max_value);
    for (int i = 0; i < Car::NUM_WHEELS; i++) {
      signedSpeeds[i] *= tasa;
    }
  }
}

void loop() {

  // 1. === POLLING DE ENCODERS (DEBE EJECUTARSE SIEMPRE) ===
  paquito.updateEncoders();

  if (newData) {
    // Depura para verificar qué llega
    // Serial.print("FL: "); Serial.print(rxData.val.fl);
    // Serial.print(" FR: "); Serial.println(rxData.val.fr);
    /*
    wheel_speeds[0] = rxData.val.fl;
    wheel_speeds[1] = rxData.val.rl;
    wheel_speeds[2] = rxData.val.fr;
    wheel_speeds[3] = rxData.val.rr;
    */
    wheel_speeds[FL] = rxData.val.fl;
    wheel_speeds[BL] = rxData.val.rl;
    wheel_speeds[FR] = rxData.val.fr;
    wheel_speeds[BR] = rxData.val.rr;
    limitSpeed(wheel_speeds);
    paquito.setSignedSpeeds(wheel_speeds);

    newData = false;
  }
  else {
    decrease_speed_factor();
  }

  // 3. === IMPRESIÓN DE ENCODERS EN EL MONITOR SERIAL DE PC ===
  // Imprimimos cada medio segundo (500 ms)
  if (millis() - lastPrintTime >= 500) {
    lastPrintTime = millis();
    
    Serial.println("====== ENCODERS ======");
    Serial.print("FL: "); Serial.println(paquito.count(FL));
    Serial.print("FR: "); Serial.println(paquito.count(FR));
    Serial.print("BL: "); Serial.println(paquito.count(BL));
    Serial.print("BR: "); Serial.println(paquito.count(BR));
    Serial.println("======================");


  // Enviar información del codificador
  Serial2.print("[ENC] ");
  for(int i = 0; i < Car::NUM_WHEELS; i++)
  {
    Serial2.print(paquito.count(i));
    Serial2.print(" ");
  }
  Serial2.println("[/ENC]");



  // 4. === IMPRESIÓN DE VELOCIDAD DE LOS MOTORES ===
  // Imprimimos cada  segundo (1000 ms)
  if (millis() - lastPrintTime >= 1000) {
    lastPrintTime = millis();
    
    Serial.println("====== VELOCIDADES ======");
    Serial.print("FL: "); Serial.println(paquito.getVelocidad(FL));
    Serial.print("FR: "); Serial.println(paquito.getVelocidad(FR));
    Serial.print("BL: "); Serial.println(paquito.getVelocidad(BL));
    Serial.print("BR: "); Serial.println(paquito.getVelocidad(BR));
    Serial.println("======================");



  if (speak) {
    voice.babble();
    speak = false;
  }
}

// Función que se ejecuta cuando el maestro va a enviar información.
void receiveEvent(int howMany) {
  if (howMany == 9) {
    // Velocidad llanta por llanta
    // Verificamos comando
    unsigned char commandByte = Wire.read();
    if (commandByte != SET_WHEELS_SPEED) {
      Serial.print("No es el código del comando para asignar velocidad.\n");
      return;
    }
    speed_decay = MAX_DECAY;

    // Leemos los 8 bytes directamente en nuestra unión
    for(int i = 0; i < 8; i++) {
      rxData.bytes[i] = Wire.read();
    }
    newData = true;
  } else {
    char buf[200];
    sprintf(buf, "\n...~~~ Received %d:\n", howMany);
    Serial.print(buf);

    /*
    char ini = Wire.read();
    sprintf(buf, "First byte: %d\n", ini);
    Serial.print(buf);
    */
    /*
    if (howMany < 2) {
      Serial.println("~~~ Menos de 2 bytes.  Descartando basura.\n");
      // Limpiar buffer si llega basura
      while(Wire.available()) Wire.read();
      return;
    }
    */
    unsigned char commandByte = Wire.read();
    

    const int commandBufLength = 15;
    char commandName[commandBufLength];
    toCharArray(commandByte, commandName, commandBufLength);
    sprintf(buf, "Command: %d %s\n", commandByte, commandName);

    Serial.print(buf);
    

    Serial.println("Arguments:");
    unsigned char args[howMany - 1];
    int ind = 0;
    int ava;
    while (ava = Wire.available()) {
      unsigned char c = Wire.read(); // receive byte as a character
      sprintf(buf, "Wire read %d: %d=%c\n", ava, c, c);
      Serial.print(buf);

      args[ind++] = c;
    }

    speed_decay = MAX_DECAY;
    execute(commandByte, args);
    Serial.println("~~~...\n");
    
    // Limpiar buffer si llega basura
    while(Wire.available()) Wire.read();
  }
}