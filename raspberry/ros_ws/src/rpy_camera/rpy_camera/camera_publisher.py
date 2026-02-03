import rclpy
from rclpy.node import Node

import io
from picamera2 import Picamera2
import numpy as np
from libcamera import controls
from picamera2.encoders import JpegEncoder
from picamera2.outputs import FileOutput
from threading import Condition

#from sensor_msgs.msg import Image
from sensor_msgs.msg import CompressedImage


class StreamingOutput(io.BufferedIOBase):
    def __init__(self):
        self.frame = None
        self.condition = Condition()

    def write(self, buf):
        with self.condition:
            self.frame = buf
            self.condition.notify_all()


class CameraPublisher(Node):
    # Selected sensor format: 1536x864-SBGGR10_1X10 - Selected unicam format: 1536x864-pBAA
    # Sensor: /base/soc/i2c0mux/i2c@1/imx708@1a - Selected sensor format: 2304x1296-SBGGR10_1X10 - Selected unicam format: 2304x1296-pBAA
    HEIGHT = 1296 // 4 # 864
    WIDTH = 2304 // 4 #1536
    def __init__(self):
        super().__init__('camera_publisher_node')
        
        # Editor que publica la imagen
        #self.publisher_ = self.create_publisher(Image, '/camera/image', 10)
        self.publisher_ = self.create_publisher(CompressedImage, '/camera/image', 10)

        # Configuración de PiCamera2
        self.picam2 = picam2 = Picamera2()

        # Ajusta la resolución
        conf = picam2.create_video_configuration(
            main={
                'format': 'RGB888',
                #'size': (CameraPublisher.HEIGHT, CameraPublisher.WIDTH)
                'size': (CameraPublisher.WIDTH, CameraPublisher.HEIGHT)
            }
        )
        picam2.configure(conf)
        picam2.start()

        #output = StreamingOutput()
        #picam2.start_recording(JpegEncoder(), FileOutput(output))
        #picam2.set_controls({"AfMode": controls.AfModeEnum.Continuous, "AfSpeed": controls.AfSpeedEnum.Fast})

        # Cronómetro para capturar y publicar (30 FPS aprox)
        self.timer = self.create_timer(1.0 / 30.0, self.timer_callback)

        self.get_logger().info('Nodo editor de la cámara iniciado')

    def timer_callback(self):
        # Capturamos un cuadro
        #frame = self.picam2.capture_array()
        #print(type(frame))
        # Capturamos directamente en formato JPEG (muy rápido en el hardware de la Pi)
        # if frame is not None:
        #     # Elimina el relleno que agrega picamera2 para alinear 32 o 64 bytes
        #     frame_contig = np.ascontiguousarray(frame)

        #     msg = Image()
        #     msg.header.stamp = self.get_clock().now().to_msg()
        #     #msg.header.frame_id = "camera_link"
        #     msg.height = frame_contig.shape[0]  # CameraPublisher.HEIGHT
        #     msg.width = frame_contig.shape[1] # CameraPublisher.WIDTH
        #     msg.encoding = "rgb8"
        #     msg.is_bigendian = 0
        #     msg.step = frame_contig.shape[1] * 3  #CameraPublisher.WIDTH * 3    # Full row length in bytes = ancho * canales

        #     # Convertimos el array de NumPy a bytes puros para el mensaje
        #     msg.data = frame_contig.tobytes()

        #     self.get_logger().info('Publicando imagen')
        #     self.publisher_.publish(msg)


        # Creamos un buffer en memoria RAM
        stream = io.BytesIO()
        
        # Capturamos directamente en formato JPEG hacia el buffer
        # Esto usa el encoder de hardware de la Pi
        self.picam2.capture_file(stream, format='jpeg')
        
        if stream.tell() > 0:
            msg = CompressedImage()
            msg.header.stamp = self.get_clock().now().to_msg()
            msg.header.frame_id = "camera_frame"
            msg.format = "jpeg"
            
            # Obtenemos los bytes del buffer
            msg.data = stream.getvalue()
            
            self.publisher_.publish(msg)
            
        # Cerramos el buffer para liberar memoria
        stream.close()

    def destroy_node(self):
        self.picam2.stop()
        # picam2.stop_recording()
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)    
    node = CameraPublisher()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        node.get_logger().info("Deteniendo nodo cámara...")
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()


# from cv_bridge import CvBridge
# import numpy as np

# class CameraPublisher(Node):
#     def __init__(self):
#         super().__init__('camera_publisher_node')
        
#         # Publicador de la imagen
#         self.publisher_ = self.create_publisher(Image, 'video_frames', 10)
        
#         # Inicializamos CvBridge para convertir de OpenCV a ROS2
#         self.br = CvBridge()


#     def timer_callback(self):
#         # Capturamos un frame
#         frame = self.picam2.capture_array()

#         if frame is not None:
#             # Convertimos el array a un mensaje de imagen de ROS2
#             # Usamos 'bgr8' porque es el estándar en procesamiento de imagen
#             msg = self.br.cv2_to_imgmsg(frame, encoding="bgr8")
#             msg.header.stamp = self.get_clock().now().to_msg()
#             msg.header.frame_id = "camera_link"
            
#             # Publicamos
#             self.publisher_.publish(msg)

