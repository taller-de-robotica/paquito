// ConfigPines.h
#ifndef ConfigPines_h
#define ConfigPines_h

#include "Config.h"

// --- PINES COMUNES ---
const int I2C_SLAVE_ADDRESS = 0x8;
const int SPEAKER_PIN = 8;
const int CAMERA_SERVO = 2;

// LED PIN varía un poco entre códigos
#ifdef USAR_ROBOT_PACO
  const int LED_PIN = 9;
#else
  const int LED_PIN = 13;
#endif

#ifdef USAR_ROBOT_PACO
    // ==========================================
    //            PINES PARA PACO
    // ==========================================
    const int r_en_FR = 25; const int l_en_FR = 24; const int r_pwm_FR  = 11; const int l_pwm_FR  = 10; 
    const int r_en_FL = 23; const int l_en_FL = 22; const int r_pwm_FL  = 13; const int l_pwm_FL  = 12;
    const int r_en_BR = 29; const int l_en_BR = 28; const int r_pwm_BR  = 5;  const int l_pwm_BR  = 4;  
    const int r_en_BL = 27; const int l_en_BL = 26; const int r_pwm_BL  = 7;  const int l_pwm_BL  = 6;
    
    // Encoders PACO
    const int encoder_S1_FR = 30; const int encoder_S2_FR = 32;
    const int encoder_S1_FL = 31; const int encoder_S2_FL = 33;
    const int encoder_S1_BR = 34; const int encoder_S2_BR = 36;
    const int encoder_S1_BL = 35; const int encoder_S2_BL = 37;

#else
    // ==========================================
    //            PINES PARA PAQUITO
    // ==========================================
    const int speedPinR = 9;   const int RightMotorDirPin1 = 26;  const int RightMotorDirPin2 = 27;
    const int LeftMotorDirPin1 = 28; const int LeftMotorDirPin2 = 29; const int speedPinL = 10;
    
    const int speedPinRB = 11; const int RightMotorDirPin1B = 22; const int RightMotorDirPin2B = 23;
    const int LeftMotorDirPin1B = 24; const int LeftMotorDirPin2B = 25; const int speedPinLB = 12;

    // Encoders PAQUITO (Mismos números lógicos, pero instanciados para él)
    const int RightMotorS1PinRA = 30; const int RightMotorS2PinRA = 32;
    const int LeftMotorS1PinLA = 31;  const int LeftMotorS2PinLA = 33;
    const int RightMotorS1PinRB = 34; const int RightMotorS2PinRB = 36;
    const int LeftMotorS1PinLB = 35;  const int LeftMotorS2PinLB = 37;
#endif

#endif