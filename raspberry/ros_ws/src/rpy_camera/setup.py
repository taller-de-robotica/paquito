from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'rpy_camera'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='vero',
    maintainer_email='v.arriola@ciencias.unam.mx',
    description='Publicador de cámara para Raspberry Pi',
    license='GPL-3.0-only',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'camera_publisher_node = rpy_camera.camera_publisher:main'
        ],
    },
)
