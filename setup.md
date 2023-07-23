# ROS 2

Siguiendo:
[Compile Humble](https://docs.ros.org/en/humble/Installation/Alternatives/Ubuntu-Development-Setup.html), desde la terminal ejecutar:

```
sudo apt update && sudo apt upgrade
sudo apt update && sudo apt install locales
```

Editar ```/etc/locale.gen```  descomentar ```es_MX.UTF-8 UTF-8``` y continuar ejecutando:

```
sudo locale-gen es_MX es_MX.UTF-8
sudo update-locale LC_ALL=es_MX.UTF-8 LANG=es_MX.UTF-8
export LANG=es_MX.UTF-8

sudo apt install software-properties-common
sudo apt update && sudo apt install curl -y
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo bullseye) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

sudo apt update && sudo apt install -y \
  python3-flake8-docstrings \
  python3-pip \
  python3-pytest-cov \
  ros-dev-tools
```

Si se instala mediante ssh 

```
export PYTHON_KEYRING_BACKEND=keyring.backends.null.Keyring
```
esto porque sino se asigna esta variable, la raspberry solicita localmente una contraseña para acceder al key ring, pero esta solicitud no se transmite por ```ssh``` y la instalación se bloquea.

Como si fuera Ubuntu 20.04, instalar las dependencias siguientes:

```
python3 -m pip install -U \
   flake8-blind-except \
   flake8-builtins \
   flake8-class-newline \
   flake8-comprehensions \
   flake8-deprecated \
   flake8-import-order \
   flake8-quotes \
   "pytest>=5.3" \
   pytest-repeat \
   pytest-rerunfailures
```

Utilizando un editor de archivos, agregar ```/home/paquito/.local/bin``` a ```$PATH``` al final de ```.bashrc```, esto debe verse:
```
export PATH=/home/paquito/.local/bin:$PATH
```

Continuar descargando el código fuente como sigue:
```
cd
mkdir -p ROS/ros2_humble/src
cd ROS/ros2_humble
vcs import --input https://raw.githubusercontent.com/ros2/ros2/humble/ros2.repos src
sudo apt upgrade
```

Instalar dependencias indicadas por ```ROS 2```:

Lobotomizar, es decir, borrar la carpeta **ignition**, si no aparecen dependencias que no es posible satisfacer.  Después de esto se puede realizar el proceso para instalar dependencias:

```
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -y --skip-keys "fastcdr rti-connext-dds-6.0.1 urdfdom_headers" --os=ubuntu:jammy --rosdistro=humble

cd ~/ROS/ros2_humble/
```

A partir de aquí es necesario borrar las carpetas **ros2/rviz, ros2/rosbag2** que se descargan dentro de ```src```, pues no compilan.  Tampoco se puede hacer antes pues las dependencias solicitian estos paquetes y si no están no se descargan todas las bibliotecas que harán falta.

Para el siguiente paso, la herramienta de compilación ```colcon``` intentará utilizar todos los cores de la Raspberry, pero entonces el proceso se traba en los paquetes más pesados.  Por ejemplo, ```rclcpp``` no pasa del 42% o 74%.  Siguiendo lo explicado en [Executor](https://colcon.readthedocs.io/en/released/reference/executor-arguments.html) y [Number of threads](https://answers.ros.org/question/368249/colcon-build-number-of-threads/) habrá que compilar secuencialmente:

```
colcon build --symlink-install --executor sequential --parallel-workers 1
```

# Uso

Una vez ha terminado el proceso de compilación ya podemos utilizar ```ROS 2```, en cada terminar ejecutar:

```
source install/local_setup.sh
```

Probar en dos terminales con:
## Talker
```
. ~/ROS/ros2_humble/install/local_setup.sh
ros2 run demo_nodes_cpp talker
```

## Listener
```
. ~/ros2_humble/install/local_setup.sh
ros2 run demo_nodes_py listener
```
