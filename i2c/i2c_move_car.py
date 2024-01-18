# Instalar
# pip3 install smbus2
# https://pypi.org/project/smbus2/
# Indispensable activar I2C en setup

from smbus2 import SMBus
import time

slave_addr = 0x8     # dirección del bus en hexadecimal
bus = SMBus(1) # indica /dev/ic2-1
sleep_interval = 0.1

STOP = 0b00000001
FORWARD = 0b00001111
BACKWARD = 0b11110000

with SMBus(1) as bus:
    # El límite de SMBus son 32 bytes

    # Prueba
    data = [8, 1, 2, 3, 4, 5, 6, 7, 8]
    bus.write_i2c_block_data(slave_addr, 0, data)
    time.sleep(sleep_interval)

    # Avanza
    data = [FORWARD, 100]
    bus.write_i2c_block_data(slave_addr, 0, data)
    time.sleep(3)

    # Frena
    data = [STOP]
    bus.write_i2c_block_data(slave_addr, 0, data)
    time.sleep(sleep_interval)

    # Retrocede
    data = [BACKWARD, 100]
    bus.write_i2c_block_data(slave_addr, 0, data)
    time.sleep(3)

    # Frena
    data = [STOP]
    bus.write_i2c_block_data(slave_addr, 0, data)
    time.sleep(sleep_interval)
    