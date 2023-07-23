#  ____     ___     ___    _   _   ___   _____    ___
# |&&&&\   /&&&\   /&&&\  |&| |&| |&&&| |&&&&&|  /&&&\
# |&|_|&| |&|_|&| |&| |&| |&| |&|  |&|    |&|   |&| |&|
# |&&&&/  |&&&&&| |&| |&| |&| |&|  |&|    |&|   |&| |&|
# |&|     |&| |&| |&|&|&| |&|_|&|  |&|    |&|   |&|_|&| _
# |&|     |&| |&|  \&&\&\  \&&&/  |&&&|   |&|    \&&&/ (&)
#
# @author blackzafiro
# Para OSOYOO Mecanum v1.3, controlador L298N motor DC
# Control con RaspberryPi 4B
# 
# Referencias:
# 
# https://osoyoo.com/2020/03/01/use-raspberry-pi-to-control-mecanum-omni-wheel-robot-car/
# http://osoyoo.com/driver/mecanum/mecanum-pi.py
# https://osoyoo.com/2020/03/01/python-programming-tutorial-model-pi-l298n-motor-driver-for-raspberry-pi/
# https://osoyoo.com/driver/mecanum/modelpi.py
# https://www.electronicwings.com/raspberry-pi/raspberry-pi-pwm-generation-using-python-and-c
# https://github.com/raspberrypi/linux/blob/04c8e47067d4873c584395e5cb260b4f170a99ea/arch/arm/boot/dts/overlays/README#L944

import RPi.GPIO as GPIO #controla los pines GPIO
from rpi_hardware_pwm import HardwarePWM #permite usar pulsos por hardware
import time             #para fijar el tiempo durante el cual se ejecuta cada movimiento

# Dos pines controlan cada motor usando el L298N
# IN1=True  IN2=False el motor avanza
# IN1=False IN2=True  el motor retrocede
# en cualquier otro caso el motor se detiene
# Es análogo para IN3 e IN4
IN1_DERECHA_ATRÁS = 16     #GPIO 23
IN2_DERECHA_ATRÁS = 18     #GPIO 24

IN3_IZQUIERDA_ATRÁS = 13   #GPIO 27
IN4_IZQUIERDA_ATRÁS = 15   #GPIO 22

IN1_DERECHA_FRENTE = 40    #GPIO 21
IN2_DERECHA_FRENTE = 38    #GPIO 20

IN3_IZQUIERDA_FRENTE = 36  #GPIO 16
IN4_IZQUIERDA_FRENTE = 37  #GPIO 26

# ENA/ENB permiten controlar la velocidad de los motores izquierdo y derecho
# mediante PWM, pero la Raspberry 3 ó 4 sólo tiene dos controles y aquí hay cuatro,
# dos por cada controlador.
#ENA_DERECHA_ATRÁS = 33      #GPIO 13       PWM1
#ENB_IZQUIERDA_ATRÁS = 12    #GPIO 18 Audio PWM0

#ENA_DERECHA_FRENTE = 35     #GPIO 19       PWM1
#ENB_IZQUIERDA_FRENTE = 32   #GPIO 12       PWM0
ENB_CH_IZQUIERDA = 0 #12    #GPIO 18 Audio PWM0
ENA_CH_DERECHA = 1   #35    #GPIO 19       PWM1

USA_PWM = True
#vel_der_tras = None
#vel_izq_tras = None
#vel_der_frente = None
#vel_izq_frente = None
vel_der = None
vel_izq = None
velocidades = None

def inicializa_pines():
    #initialize GPIO pins, tell OS which pins will be used to control Model-Pi L298N board
    GPIO.setmode(GPIO.BOARD)          #set pin numbering system

    GPIO.setup(IN1_DERECHA_ATRÁS, GPIO.OUT) 
    GPIO.setup(IN2_DERECHA_ATRÁS, GPIO.OUT)
    GPIO.setup(IN3_IZQUIERDA_ATRÁS, GPIO.OUT)
    GPIO.setup(IN4_IZQUIERDA_ATRÁS, GPIO.OUT)
    
    GPIO.setup(IN1_DERECHA_FRENTE, GPIO.OUT) 
    GPIO.setup(IN2_DERECHA_FRENTE, GPIO.OUT)
    GPIO.setup(IN3_IZQUIERDA_FRENTE, GPIO.OUT)
    GPIO.setup(IN4_IZQUIERDA_FRENTE, GPIO.OUT)

    if USA_PWM:
        #following code only works when using Model-Pi instead of Model X motor driver board which can give raspberry Pi USB 5V power
        #Initialize Rear model X board ENA and ENB pins, tell OS that ENA,ENB will output analog PWM signal with 1000 frequency
        # global vel_der_tras
        # global vel_izq_tras
        # global vel_der_frente
        # global vel_izq_frente
        global vel_der
        global vel_izq
        global velocidades

        # vel_der_tras = GPIO.PWM(ENA_DERECHA_ATRÁS,1000)  #create PWM instance with frequency
        # vel_izq_tras = GPIO.PWM(ENB_IZQUIERDA_ATRÁS,1000)
        # vel_der_frente = GPIO.PWM(ENA_DERECHA_FRENTE,1000)
        # vel_izq_frente = GPIO.PWM(ENB_IZQUIERDA_FRENTE,1000)
        vel_der = HardwarePWM(pwm_channel=ENA_CH_DERECHA, hz=7000)  #create PWM instance with frequency
        vel_izq = HardwarePWM(pwm_channel=ENB_CH_IZQUIERDA, hz=7000)
        print(vel_der)
        print(vel_izq)
        
        #velocidades = [vel_der_tras, vel_izq_tras, vel_der_frente, vel_izq_frente]
        velocidades = [vel_der, vel_izq]
        #start PWM of required Duty Cycle
        for vel in velocidades:
            vel.start(0)

    else:
        #inicializa pines
        GPIO.setup(ENA_DERECHA_ATRÁS, GPIO.OUT)
        GPIO.setup(ENB_IZQUIERDA_ATRÁS, GPIO.OUT)
        GPIO.setup(ENA_DERECHA_FRENTE, GPIO.OUT)
        GPIO.setup(ENB_IZQUIERDA_FRENTE, GPIO.OUT)

        #activate motors 100%
        GPIO.output(ENA_DERECHA_ATRÁS,True)      
        GPIO.output(ENB_IZQUIERDA_ATRÁS,True)
        GPIO.output(ENA_DERECHA_FRENTE,True)
        GPIO.output(ENB_IZQUIERDA_FRENTE,True)

def finaliza_pines():
    GPIO.cleanup()
    for vel in velocidades:
            vel.stop()


#
# Movimiento llanta por llanta
#

#make rear right motor moving forward
def rr_ahead(speed):
    GPIO.output(IN1_DERECHA_ATRÁS,True)
    GPIO.output(IN2_DERECHA_ATRÁS,False)

    if USA_PWM:
        #ChangeDutyCycle(speed) function can change the motor rotation speed
        #rightSpeed.ChangeDutyCycle(speed)    #provide duty cycle in the range 0-100
        #vel_der_tras.ChangeDutyCycle(speed)
        vel_der.change_duty_cycle(speed)

#make rear right motor moving backward
def rr_back(speed):
    GPIO.output(IN2_DERECHA_ATRÁS,True)
    GPIO.output(IN1_DERECHA_ATRÁS,False)

    if USA_PWM:
        #vel_der_tras.ChangeDutyCycle(speed)
        vel_der.change_duty_cycle(speed)


#make rear left motor moving forward    
def rl_ahead(speed):  
    GPIO.output(IN3_IZQUIERDA_ATRÁS,True)
    GPIO.output(IN4_IZQUIERDA_ATRÁS,False)

    if USA_PWM:
        #vel_izq_tras.ChangeDutyCycle(speed)
        vel_izq.change_duty_cycle(speed)

#make rear left motor moving backward    
def rl_back(speed):  
    GPIO.output(IN4_IZQUIERDA_ATRÁS,True)
    GPIO.output(IN3_IZQUIERDA_ATRÁS,False)
    
    if USA_PWM:
        #vel_izq_tras.ChangeDutyCycle(speed)
        vel_izq.change_duty_cycle(speed)
    
    
#make front right motor moving forward
def fr_ahead(speed):
    GPIO.output(IN1_DERECHA_FRENTE,True)
    GPIO.output(IN2_DERECHA_FRENTE,False)

    if USA_PWM:
        #vel_der_frente.ChangeDutyCycle(speed)
        vel_der.change_duty_cycle(speed)

#make front right motor moving backward
def fr_back(speed):
    GPIO.output(IN2_DERECHA_FRENTE,True)
    GPIO.output(IN1_DERECHA_FRENTE,False)

    if USA_PWM:
        #vel_der_frente.ChangeDutyCycle(speed)
        vel_der.change_duty_cycle(speed)


#make front left motor moving forward    
def fl_ahead(speed):  
    GPIO.output(IN3_IZQUIERDA_FRENTE,True)
    GPIO.output(IN4_IZQUIERDA_FRENTE,False)

    if USA_PWM:
        #vel_izq_frente.ChangeDutyCycle(speed)
        vel_izq.change_duty_cycle(speed)

#make Front left motor moving backward    
def fl_back(speed):  
    GPIO.output(IN4_IZQUIERDA_FRENTE,True)
    GPIO.output(IN3_IZQUIERDA_FRENTE,False)

    if USA_PWM:
        #vel_izq_frente.ChangeDutyCycle(speed)
        vel_izq.change_duty_cycle(speed)


#make both motors stop
def stop_car():
    GPIO.output(IN1_DERECHA_ATRÁS,False)
    GPIO.output(IN2_DERECHA_ATRÁS,False)
    GPIO.output(IN3_IZQUIERDA_ATRÁS,False)
    GPIO.output(IN4_IZQUIERDA_ATRÁS,False)
    GPIO.output(IN1_DERECHA_FRENTE,False)
    GPIO.output(IN2_DERECHA_FRENTE,False)
    GPIO.output(IN3_IZQUIERDA_FRENTE,False)
    GPIO.output(IN4_IZQUIERDA_FRENTE,False)
    for vel in velocidades:
        vel.change_duty_cycle(0)


#
# Movimientos del carrito
#

def go_ahead(speed):
    rl_ahead(speed)
    rr_ahead(speed)
    fl_ahead(speed)
    fr_ahead(speed)
    
def go_back(speed):
    rr_back(speed)
    rl_back(speed)
    fr_back(speed)
    fl_back(speed)

#making right turn   
def turn_right(speed):
    rl_ahead(speed)
    rr_back(speed)
    fl_ahead(speed)
    fr_back(speed)
      
#make left turn
def turn_left(speed):
    rr_ahead(speed)
    rl_back(speed)
    fr_ahead(speed)
    fl_back(speed)

# parallel left shift 
def shift_left(speed):
    fr_ahead(speed)
    rr_back(speed)
    rl_ahead(speed)
    fl_back(speed)

# parallel right shift 
def shift_right(speed):
    fr_back(speed)
    rr_ahead(speed)
    rl_back(speed)
    fl_ahead(speed)

def upper_right(speed):
    rr_ahead(speed)
    fl_ahead(speed)

def lower_left(speed):
    rr_back(speed)
    fl_back(speed)
    
def upper_left(speed):
    fr_ahead(speed)
    rl_ahead(speed)

def lower_right(speed):
    fr_back(speed)
    rl_back(speed)





def mueve_carrito():
    '''
    Rutina de movimientos que ejecutará el carrito.
    '''
    speed = 100

    go_ahead(speed)
    time.sleep(1)
    stop_car()

    go_back(speed)
    time.sleep(1)
    stop_car()

    turn_left(speed)
    time.sleep(1)
    stop_car()

    turn_right(speed)
    time.sleep(1)
    stop_car()

    shift_right(speed)
    time.sleep(1)
    stop_car()

    shift_left(speed)
    time.sleep(1)
    stop_car()

    upper_left(speed)
    time.sleep(1)
    stop_car()

    lower_right(speed)
    time.sleep(1)
    stop_car()

    upper_right(speed)
    time.sleep(1)
    stop_car()

    lower_left(speed)
    time.sleep(1)
    stop_car()


def mueve_carrito_vel():

    # for duty in range(0,101,1):
    #     pi_pwm.ChangeDutyCycle(duty) #provide duty cycle in the range 0-100
    #     time.sleep(0.01)
    # time.sleep(0.5)
    
    # for duty in range(100,-1,-1):
    #     pi_pwm.ChangeDutyCycle(duty)
    #     time.sleep(0.01)
    # time.sleep(0.5)

    speed = 100  # Con menos de 100 emite un zumbido

    # Basta con cambiar el duty cicle, pero esta será un prueba rápida
    for duty_speed in range(10,101,1):
        #go_ahead(duty_speed)
        go_ahead(speed)
        time.sleep(0.03)
    stop_car()

    for duty_speed in range(10,101,1):
        #go_back(duty_speed)
        go_back(speed)
        time.sleep(0.03)
    stop_car()


if __name__ == '__main__':
    try:
        inicializa_pines()
        mueve_carrito_vel()
    except KeyboardInterrupt:    # If CTRL+C is pressed, exit cleanly:
        print("Keyboard interrupt")
    except Exception as  x:
        print("some error")
        raise(x) 
    finally:
        finaliza_pines()

