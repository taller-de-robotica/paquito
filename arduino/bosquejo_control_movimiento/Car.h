/*
  Car.h - Library for controling the wheels of an omnidirectional car.
  Created by Verónica E. Arriola, September 4, 2023.
*/
#ifndef Car_h
#define Car_h

#include "Arduino.h"

class Wheel
{
public:
  Wheel(unsigned int sfp, unsigned int sbp, unsigned int fp, unsigned int bp);

  void begin();

  void moveForward(unsigned int speed);
  void moveBackward(unsigned int speed);
  void setSignedSpeed(int16_t speed);

  void stop();

  // Auxiliar para depurar pines.
  void printStatus(String name);
private:
  const unsigned int SPEED_FORWARD_PIN;
  const unsigned int SPEED_BACKWARD_PIN;
  const unsigned int FORWARD_PIN;
  const unsigned int BACKWARD_PIN;
};

enum WheelId
{
  FL = 0,
  FR,
  BL,
  BR
};

class Encoder
{
public:
  Encoder(unsigned int s1, unsigned int s2);

  void begin();
  void read(int readings[2]);
  int getCount();
  void reset_count();
  int update();
  double getVelocidad();
private:
  const unsigned int S1_PIN;
  const unsigned int S2_PIN;
  unsigned long _lastTime;
  int _counter;
  int _s1State;
  int _s2State;
  int _s1LastState;
  double ultimaVelocidadRadS=0;

  //Los siguientes son nuevos y son para calcular la velocidad
  unsigned long tiempoAnterior = 0;
  unsigned long pulsosAnteriores = 0;
  //const float PPR = 1320.0; valor antiguo y de prueba
  const float PPR = 683.0; //Valor adquirido empíricamente
  const int intervaloMuestreo = 100; // 100ms es un buen equilibrio (10Hz)
};

class Car
{
public:
  static const int NUM_WHEELS = 4;

  Car(const Wheel wheels[NUM_WHEELS], Encoder encoders[NUM_WHEELS]);

  Car(const Wheel wheels[NUM_WHEELS]);

  void begin();

  // Mueve cada llanta una vez hacia adelante y hacia atrás.
  void testWheels();
  void moveForward(WheelId id, int speed);

  void stop();
  void stop(WheelId id);

  void moveForward(unsigned int speed);
  void moveBackward(unsigned int speed);
  void moveLeft(unsigned int speed);
  void moveRight(unsigned int speed);
  void moveNW(unsigned int speed);
  void moveNE(unsigned int speed);
  void moveSE(unsigned int speed);
  void moveSW(unsigned int speed);
  void rotateClockwise(unsigned int speed);
  void rotateCounterClockwise(unsigned int speed);
  void setSignedSpeeds(int16_t signedSpeeds[NUM_WHEELS]);
  void updateEncoders();

  float getVelocidad(WheelId id);
  
  double count(WheelId id);
private:
  const Wheel *_wheels;
  Encoder *_encoders;
};

#endif
