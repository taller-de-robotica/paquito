# Instalar
# pip3 install smbus2
# https://pypi.org/project/smbus2/
# Teoría: https://programarfacil.com/blog/arduino-blog/comunicacion-i2c-con-arduino/

#  Raspberry Pi I2C Master
#  Raspberry Pi Master for Arduino Slave
#  i2c_master_pi.py
#  Connects to Arduino via I2C
#  https://dronebotworkshop.com/i2c-arduino-raspberry-pi/

#  DroneBot Workshop 2019
#  https://dronebotworkshop.com

#  Indispensable activar I2C en setup

from smbus2 import SMBus

addr = 0x8     # dirección del bus en hexadecimal

with SMBus(1) as bus:     # indica /dev/ic2-1
    # El límite de SMBus son 32 bytes
    data = [8, 1, 2, 3, 4, 5, 6, 7, 8]
    bus.write_i2c_block_data(addr, 0, data)
