from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

package_name = 'paquito_raspberry_cpp'

def generate_launch_description():
    package_dir = get_package_share_directory(package_name)
    return LaunchDescription([
        Node(
            package=package_name,
            executable='command_executor',
            name='command_executor',
            output='screen',
        ),
        Node(
            package=package_name,
            name='car_velocity_subscriber',
            executable='car_velocity_subscriber',
            output='screen',
        ),
        Node(
            package='rpy_camera', # Nombre que pusiste en setup.py
            executable='camera_publisher_node',    # El nombre del entry_point
            name='pi_camera_streamer',
            output='screen',
            parameters=[{
                'use_sim_time': False
            }],
            remappings=[
                ('/video_frames', '/robot/camera/image_raw')
            ]
        )
    ])

