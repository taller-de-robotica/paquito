from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # 1. Nodo Driver del Mando (Paquete oficial de ROS2)
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node_driver',
            parameters=[{
                'dev': '/dev/input/js0',  # Puerto estándar del mando
                'deadzone': 0.1,          # Ignora movimientos mínimos del stick
                'autorepeat_rate': 20.0,  # Frecuencia de publicación en Hz
            }]
        ),

        # 2. Nodo de Python
        Node(
            package='control_paquito', # Reemplaza con el nombre de tu paquete
            executable='ps_control_node', # Nombre del script definido en setup.py
            name='ps_control_node',
            output='screen',
            emulate_tty=True # Importante para ver los logs en la terminal
        ),

        # 3. Nodo para ver imágenes
        Node(
            package='camera_viewer',
            executable='viewer_node',
            name='viewer_node',
            output='screen',
        )
    ])

