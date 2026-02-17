#include "Arduino.h"
#include "Car.h"
//#include <cmath>


// Wheel

Wheel::Wheel(unsigned int sfp, unsigned int sbp, unsigned int fp, unsigned int bp) : SPEED_FORWARD_PIN(sfp), SPEED_BACKWARD_PIN(sbp), FORWARD_PIN(fp), BACKWARD_PIN(bp) {}

void Wheel::begin()
{
    pinMode(FORWARD_PIN, OUTPUT);
    pinMode(BACKWARD_PIN, OUTPUT);
    pinMode(SPEED_FORWARD_PIN, OUTPUT);
    pinMode(SPEED_BACKWARD_PIN, OUTPUT);

    stop();
}

void Wheel::moveForward(unsigned int speed)
{
    //printStatus("mF ");

    digitalWrite(FORWARD_PIN, HIGH);
    digitalWrite(BACKWARD_PIN, HIGH);

    analogWrite(SPEED_FORWARD_PIN, speed);
    analogWrite(SPEED_BACKWARD_PIN, 0);
}

void Wheel::moveBackward(unsigned int speed)
{
    //printStatus("mB");

    digitalWrite(FORWARD_PIN, HIGH);
    digitalWrite(BACKWARD_PIN, HIGH);

    analogWrite(SPEED_FORWARD_PIN, 0);
    analogWrite(SPEED_BACKWARD_PIN, speed);
}

void Wheel::setSignedSpeed(int16_t speed)
{
    if (speed > 0) {
        moveForward(constrain(speed, 0, 255));
    } else if (speed < 0) {
        moveBackward(constrain(-1 * speed, 0, 255));
    } else {
        stop();
    }
}

void Wheel::stop()
{
    //printStatus("S");
    // No sé si las siguientes líneas añaden seguridad o no
    // digitalWrite(FORWARD_PIN, HIGH);
    // digitalWrite(BACKWARD_PIN, HIGH);

    analogWrite(SPEED_FORWARD_PIN, 0);
    analogWrite(SPEED_BACKWARD_PIN, 0);
}

void Wheel::printStatus(String name)
{
    Serial.print(name);
    Serial.print(" ");
    Serial.print(FORWARD_PIN);
    Serial.print(" ");
    Serial.print(BACKWARD_PIN);
    Serial.print(" ");
    Serial.println(SPEED_FORWARD_PIN);
    Serial.print(" ");
    Serial.println(SPEED_BACKWARD_PIN);
    Serial.print(" ");
}

// Encoder
Encoder::Encoder(unsigned int s1, unsigned int s2) : S1_PIN(s1), S2_PIN(s2), _counter(0) {}

void Encoder::begin()
{
    pinMode (S1_PIN, INPUT);
    pinMode (S2_PIN, INPUT);

    reset_count();
}

int Encoder::getCount()
{
    return _counter;
}

void Encoder::reset_count()
{
    _counter = 0;
    _lastTime = millis();
    _s1LastState = digitalRead(S1_PIN);  // Estado incial del pin S1
}

void Encoder::read(int readings[2])
{
    readings[0] = digitalRead(S1_PIN);
    readings[1] = digitalRead(S2_PIN);
    return readings;
}

/*
    Función que actualiza el valor del contador de pulsos del encoder en cuestión.
*/
int Encoder::update()
{
    _s1State = digitalRead(S1_PIN);

    unsigned long time = millis();
    int delta = time - _lastTime;
    _lastTime = time;

    // El sensor S1 cambió de posición (giró la rueda)
    if(_s1State != _s1LastState){

        _s2State = digitalRead(S2_PIN);
        if (_s2State != _s1State) _counter++;   // La rueda giró hacia adelante
        else _counter--;                        // La ruega giró hacia atrás
        
        _s1LastState = _s1State;  // Actualizamos el estado del sensor S1
    }
    //return double(_counter)/double(time);
    return _counter;
}


double Encoder::getVelocidad(){
    
    unsigned long tiempoActual = millis();

    // 2. Verificamos si pasó el tiempo de muestreo (Non-blocking delay)
    if (tiempoActual - tiempoAnterior >= intervaloMuestreo) {
    
   
    long pulsosActuales = getCount();
  
    // B. Calculamos la diferencia de pulsos y tiempo
    long deltaPulsos = pulsosActuales - pulsosAnteriores;
    double deltaTiempoSegundos = (tiempoActual - tiempoAnterior) / 1000.0; //¿Por qué acá dividimos entre 1000?, porque eso convierte de milisegundos a segundos
    //double deltaTiempoSegundos = (tiempoActual - tiempoAnterior);

    // C. Aplicamos la fórmula para Radianes por Segundo
    // (DeltaPulsos / PPR) = Vueltas
    // Vueltas * 2 * PI = Radianes
    // Radianes / Tiempo = Rad/s
    double velocidadRadS = (deltaPulsos / PPR) * 2.0 * 3.141592653 / deltaTiempoSegundos;

    // E. Actualizar variables "anteriores" para el siguiente ciclo
    pulsosAnteriores = pulsosActuales;
    tiempoAnterior = tiempoActual;
    ultimaVelocidadRadS = velocidadRadS;
    return velocidadRadS;
  }

    return ultimaVelocidadRadS;
}


// Car
Car::Car(const Wheel wheels[NUM_WHEELS], const Encoder encoders[NUM_WHEELS])
    : _wheels(wheels), _encoders(encoders){}

Car::Car(const Wheel wheels[NUM_WHEELS])
    : _wheels(wheels), _encoders(NULL){}

void Car::begin()
{
    for(int i = 0; i < NUM_WHEELS; i++)
    {
      // Inicializa llanta
      _wheels[i].begin();

      if(_encoders != NULL) {
        // Inicializa odómetro
        _encoders[i].begin();
      }
    }
 }

/*
    Nuevo método que actualiza los contadores de los 4 encoders del carro 
    (suma o resta éstos contadores).
    Éste método se debe de invocar en el loop para que se actualicen constantemente
    dichos contadores.
*/
void Car::updateEncoders(){
    // Si el carro no tiene encoders, no se hace nada
    if(_encoders == NULL) return;

    //Si el carro sí tiene encoders, se invoca la función que actualiza el 
    //contador de pulsos de cada encoder
    _encoders[FL].update(); 
    _encoders[FR].update();
    _encoders[BL].update();
    _encoders[BR].update();
}

  // Mueve cada llanta una vez hacia adelante y hacia atrás.
void Car::testWheels()
{
    int delTime = 1000;
    int speed = 70;
    for(int i = 0; i < NUM_WHEELS; i++)
    {
        _wheels[i].moveForward(speed);
        delay(delTime);
        _wheels[i].stop();
        delay(delTime);
        _wheels[i].moveBackward(speed);
        delay(delTime);
        _wheels[i].stop();
        delay(delTime);
    }
}

void Car::moveForward(WheelId id, int speed)
{
    _wheels[id].moveForward(speed);
}

void Car::stop()
{
    for(int i = 0; i < NUM_WHEELS; i++)
    {
      _wheels[i].stop();
    }
}

void Car::stop(WheelId id)
{
    _wheels[id].stop();
}

void Car::moveForward(unsigned int speed)
{
    for(int i = 0; i < NUM_WHEELS; i++)
    {
        _wheels[i].moveForward(speed);
    }
}

void Car::moveBackward(unsigned int speed)
{
    for(int i = 0; i < NUM_WHEELS; i++)
    {
        _wheels[i].moveBackward(speed);
    }
}

void Car::moveLeft(unsigned int speed)
{
    _wheels[FR].moveForward(speed);
    _wheels[BL].moveForward(speed);
    _wheels[FL].moveBackward(speed);
    _wheels[BR].moveBackward(speed);
}

void Car::moveRight(unsigned int speed)
{
    _wheels[FL].moveForward(speed);
    _wheels[BR].moveForward(speed);
    _wheels[FR].moveBackward(speed);
    _wheels[BL].moveBackward(speed);
}

void Car::moveNW(unsigned int speed)
{
    _wheels[FR].moveForward(speed);
    _wheels[BR].moveForward(speed);
    _wheels[FL].stop();
    _wheels[BL].stop();
}

void Car::moveNE(unsigned int speed)
{
    _wheels[FL].moveForward(speed);
    _wheels[BL].moveForward(speed);
    _wheels[FR].stop();
    _wheels[BR].stop();
}

void Car::moveSE(unsigned int speed)
{
    _wheels[FL].moveBackward(speed);
    _wheels[BL].moveBackward(speed);
    _wheels[FR].stop();
    _wheels[BR].stop();
}

void Car::moveSW(unsigned int speed)
{
    _wheels[FR].moveBackward(speed);
    _wheels[BR].moveBackward(speed);
    _wheels[FL].stop();
    _wheels[BL].stop();
}

void Car::rotateClockwise(unsigned int speed)
{
    _wheels[FL].moveForward(speed);
    _wheels[BL].moveForward(speed);
    _wheels[FR].moveBackward(speed);
    _wheels[BR].moveBackward(speed);
}

void Car::rotateCounterClockwise(unsigned int speed)
{
    _wheels[FR].moveForward(speed);
    _wheels[BR].moveForward(speed);
    _wheels[FL].moveBackward(speed);
    _wheels[BL].moveBackward(speed);
}

void Car::setSignedSpeeds(int16_t signedSpeeds[NUM_WHEELS])
{
    for(int i = 0; i < NUM_WHEELS; i++)
    {
        int16_t speed = signedSpeeds[i];
        if (speed > 0) {
            _wheels[i].moveForward(constrain(speed, 0, 255));
        } else if (speed < 0) {
            _wheels[i].moveBackward(constrain(-1 * speed, 0, 255));
        } else {
            _wheels[i].stop();
        }
    }
    // Esta función sigue un orden diferente para el orden de las llantas
    /*
    _wheels[FL].setSignedSpeed(signedSpeeds[1]);
    _wheels[FR].setSignedSpeed(signedSpeeds[0]);
    _wheels[BR].setSignedSpeed(signedSpeeds[3]);
    _wheels[BL].setSignedSpeed(signedSpeeds[2]);
    */
    /*
    _wheels[FL].setSignedSpeed(signedSpeeds[0]);
    _wheels[BL].setSignedSpeed(signedSpeeds[1]);
    _wheels[FR].setSignedSpeed(signedSpeeds[2]);
    _wheels[BR].setSignedSpeed(signedSpeeds[3]);
    */
}

float Car::getVelocidad(WheelId id)
{
    return _encoders[id].getVelocidad();
}

double Car::count(WheelId id)
{
    return _encoders[id].getCount();
}
