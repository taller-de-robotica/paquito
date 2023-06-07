import cv2

# Cambiar para poner la dirección de la Raspberry
dirección_ip = "http://192.168.16.107:8000/stream.mjpg"
cap = cv2.VideoCapture(dirección_ip)

video_fps = cap.get(cv2.CAP_PROP_FPS),
total_frames = cap.get(cv2.CAP_PROP_FRAME_COUNT)
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)

print(f"Cuadros por segundo: {video_fps }")
print(f"Cuadros en total:    {total_frames}")
print(f"Alto:                {height}")
print(f"Ancho:               {width}")
print()
print("Presione q sobre la imagen para cerrar.")

while True:
    ret, frame = cap.read()
    if not ret: break # break if no next frame
    
    cv2.imshow('Cam', frame) # show frame
    
    if cv2.waitKey(1) & 0xFF == ord('q'): # on press of q break
        break
        
# release and destroy windows
cap.release()
cv2.destroyAllWindows()

