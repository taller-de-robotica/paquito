# Requiere
'''
pip3 install pygame
'''
# Conexión al bluetooth
import socket

#bt_addr = '00:0C:BF:03:28:59'
bt_addr = '00:0C:BF:0D:6E:66'
bt_port = 1

sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
sock.connect((bt_addr, bt_port))
print("Connected")


# Example file showing a circle moving on screen
import pygame

# pygame setup
pygame.init()
screen = pygame.display.set_mode((640, 360))
pygame.display.set_caption('Control Bluetooth')

clock = pygame.time.Clock()
running = True
dt = 0


CIRCLE_SPEED = 300
CIRCLE_DIAMETER = 40

center_pos = pygame.Vector2(screen.get_width() / 2, screen.get_height() / 2)
player_pos = pygame.Vector2(center_pos)

MAX_RAD = (screen.get_width() if screen.get_width() < screen.get_height() else screen.get_height())/2 - CIRCLE_DIAMETER
MIN_X = center_pos.x - MAX_RAD
MAX_X = center_pos.x + MAX_RAD
MIN_Y = center_pos.y - MAX_RAD
MAX_Y = center_pos.y + MAX_RAD

font = pygame.font.SysFont(None, 24)
text_img = None

standard_color = "purple"
stop_color = "red"
current_color = standard_color

# Estado de movimiento
# 789
# 456
# 123
prev_state = 5
current_state = prev_state


def send():
    """
    Manda el estado actual para indicar la dirección
    en que se debe mover el robot.
    """
    global current_state
    
    data = str(current_state)
    data_bytes = bytes(data, 'utf-8')
    sock.send(data_bytes)
    data_back = sock.recv(1024)
    
    global text_img
    text_img = font.render(f'Sent: {data}   Received: {data_back}', True, 'black')

def move(dx=0, dy=0):
    """
    Mueve al círculo en la dirección indicada.
    OJO: usa variables globales
    """
    send()
    
    global prev_state
    
    if current_state == prev_state:
        # Se sigue moviendo en la misma dirección
        if player_pos.x < MAX_X and player_pos.x > MIN_X:
            player_pos.x += CIRCLE_SPEED * dt * dx
        if player_pos.y < MAX_Y and player_pos.y > MIN_Y:
            player_pos.y += CIRCLE_SPEED * dt * dy
    else:
        # Cambio de dirección
        prev_state = current_state
        player_pos.x = center_pos.x + CIRCLE_SPEED * dt * dx
        player_pos.y = center_pos.y + CIRCLE_SPEED * dt * dy
    
    global current_color
    current_color = standard_color
    

while running:
    # poll for events
    # pygame.QUIT event means the user clicked X to close your window
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # fill the screen with a color to wipe away anything from last frame
    screen.fill("steelblue1")
    circ = pygame.draw.circle(screen, 'azure3', center_pos, CIRCLE_DIAMETER)
    if text_img:
        screen.blit(text_img, (10, 10))

    keys = pygame.key.get_pressed()
    
    if keys[pygame.K_q]:
    	running = False
    
    if keys[pygame.K_e]:
        current_state = 7
        move(dx=-1, dy=-1)
    if keys[pygame.K_r]:
        current_state = 8
        move(dy=-1)
    if keys[pygame.K_t]:
        current_state = 9
        move(dx=1, dy=-1)
    
    if keys[pygame.K_d]:
        current_state = 4
        move(dx=-1)
    if keys[pygame.K_f]:
        current_state = 5
        send()
        player_pos.x = center_pos.x
        player_pos.y = center_pos.y
        current_color = stop_color
    if keys[pygame.K_g]:
        current_state = 6
        move(dx=1)

    if keys[pygame.K_c]:
        current_state = 1
        move(dx=-1, dy=1)
    if keys[pygame.K_v]:
        current_state = 2
        move(dy=1)
    if keys[pygame.K_b]:
        current_state = 3
        move(dx=1, dy=1)
        
    circ = pygame.draw.circle(screen, current_color, player_pos, CIRCLE_DIAMETER)

    # flip() the display to put your work on screen
    pygame.display.flip()

    # limits FPS to 60
    # dt is delta time in seconds since last frame, used for framerate-
    # independent physics.
    dt = clock.tick(60) / 1000

sock.close()
pygame.quit()

