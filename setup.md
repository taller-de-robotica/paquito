# ROS 2

Siguiendo:
[Compile Humble](https://docs.ros.org/en/humble/Installation/Alternatives/Ubuntu-Development-Setup.html)

```
sudo apt update && sudo apt upgrade
sudo apt update && sudo apt install locales
```

Editar ```/etc/locale.gen```  descomentar ```es_MX.UTF-8 UTF-8```.

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
esto porque sino la raspberry solicita localmente una contraseña para acceder al key ring, pero esta solicitud no se transmite por ```ssh``` y la instalación se bloquea.

Como si fuera Ubuntu 20.04

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

Agregar /home/paquito/.local/bin a $PATH en .bashrc

```
cd
mkdir -p ROS/ros2_humble/src
cd ROS/ros2_humble
vcs import --input https://raw.githubusercontent.com/ros2/ros2/humble/ros2.repos src
sudo apt upgrade
```

# Es necesario lobotomizar ignition, ros2/rviz, ros2/rosbag2

sudo rosdep init
rosdep upgrade
rosdep install --from-paths src --ignore-src -y --skip-keys "fastcdr rti-connext-dds-6.0.1 urdfdom_headers" --os=ubuntu:jammy --rosdistro=humble

cd ~/ROS/ros2_humble/

# https://colcon.readthedocs.io/en/released/reference/executor-arguments.html
# https://answers.ros.org/question/368249/colcon-build-number-of-threads/

# Evita que se atore, rclcpp no pasa del 42% o 74%

colcon build --symlink-install --executor sequential --parallel-workers 1


# Para usar:

source install/local_setup.bash

# Probar con
## Talker

. ~/ROS/ros2_humble/install/local_setup.bash
ros2 run demo_nodes_cpp talker

## Listener
. ~/ros2_humble/install/local_setup.bash
ros2 run demo_nodes_py listener

