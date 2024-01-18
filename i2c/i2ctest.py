#  Raspberry Pi I2C Master
#  Raspberry Pi Master for Arduino Slave
#  i2c_master_pi.py
#  Connects to Arduino via I2C
#  https://dronebotworkshop.com/i2c-arduino-raspberry-pi/

#  DroneBot Workshop 2019
#  https://dronebotworkshop.com

#  Indispensable activar I2C en setup

from smbus import SMBus

addr = 0x8 # bus address
bus = SMBus(1) # indicates /dev/ic2-1

numb = 1

print ("Enter a number or 0 to turn OFF")
while numb == 1:

	str_number = input(">>>>   ")
	number = int(str_number)
	print("sending ", str_number, " = ", bin(number))

	bus.write_byte(addr, number)

	if number == 0:
		numb = 0
