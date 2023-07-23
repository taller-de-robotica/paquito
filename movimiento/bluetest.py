'''
Se conecta al HC-02 del OSOYOO, detectando su dirección.

sudo apt update
sudo apt install libbluetooth-dev libboost-python-dev libboost-thread-dev libglib2.0-dev python-dev-is-python3
pip3 install "setuptools<58.0"
pip3 install --upgrade pybluez
'''
import bluetooth
import socket       # El socket de pybluez ya no funciona
import sys

nearby_devices = bluetooth.discover_devices(lookup_names=True, lookup_class=True)
print("Found {} devices.".format(len(nearby_devices)))
print(nearby_devices)

bt_name = 'HC-02'
bt_addr = None
for addr, name, dev_class in nearby_devices:
    print("Addres: {} - Name: {} - Class: {}".format(addr, name, dev_class))
    if name == bt_name:
        bt_addr = addr
        
if not bt_addr:
    print("{} was not found", bt_name)
    sys.exit(1)
    
# Create the client socket
#port = first_match["port"]
#name = first_match["name"]
#host = first_match["host"]
print("Looking for services...")
services = bluetooth.find_service(address=bt_addr)
print(services)

if len(services) == 0:
    print("No se encontró el servicio")
    sys.exit(1)
if len(services) > 1:
    print("¡Tocayos a la vista!")
    sys.exit(2);
    
bt_port = services[0]['port']

print("Trying to connect to {} on PSM 0x{}...".format(bt_addr, bt_port))

# Se usa un socket normal porque el de PyBluez ya no funciona.
sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
sock.connect((bt_addr, bt_port))
print("Connected. Type something...")
while True:
    data = input()
    if not data:
        break
    data_bytes = bytes(data, 'utf-8')
    sock.send(data_bytes)
    print("Sent:", data)
    
    # Requiere que el carrito responda escribiendo en el Serial correspondiente.
    data = sock.recv(1024)
    print("Data received:", str(data))
sock.close()


