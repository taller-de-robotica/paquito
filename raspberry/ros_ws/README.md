## Configuración

Para comunicar a la Raspberry con Arduino por medio del protocolo I2C, el usuario de la Raspberry debe estar en el grupo con los permisos correspondientes:

```
sudo raspi-config
# Navegar a interfacing options y activar i2c y video
sudo usermod -a -G i2c <usuario>
sudo usermod -a -G video <usuario>
sudo reboot
```

Para detectar dispositivos I2C conectados:

```
i2cdetect -y 1
```

## Para ejecutar

Al iniciar la sesión, cargar el _underlay_ y el _overlay_ y asignar el ```ROS_DOMAIN_ID``` según su número de robot:

```
source ~/ros2_jazzy/install/setup.bash
source ~/ros_ws/install/local_setup.bash
export ROS_DOMAIN_ID=1
```

Si se tiene ```ufw``` activado, permitir el tráfico desde la Raspberry:

```
sudo ufw allow from [IP]
```

Después se puede borrar listando las reglas numeradas y borrando el número correspondiente

```
sudo ufw status numbered
sudo ufw delete [number]
```

Para ejecutar todos los nodos en la Raspberry:

```
ros2 launch paquito_raspberry_cpp car_launch.py
```

### Ejecución unidad por unidad

Para activar el nodo que escucha los comandos de velocidad:

```
ros2 run paquito_raspberry_cpp car_subscriber
```

Para activar el nodo que envía comandos I2C:
```
ros2 run paquito_raspberry_cpp command_executor
```