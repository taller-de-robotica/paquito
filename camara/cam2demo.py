from picamera2 import Picamera2
from libcamera import controls

# https://www.tomshardware.com/how-to/raspberry-pi-camera-module-3-python-picamera-2

picam2 = Picamera2()
picam2.start(show_preview=True)
picam2.set_controls({"AfMode": controls.AfModeEnum.Continuous, "AfSpeed": controls.AfSpeedEnum.Fast})
c = input("Presione una techa para cerrar")
picam2.title_fields = ["ExposureTime", "AnalogueGain"]
picam2.stop_preview()
picam2.stop()
