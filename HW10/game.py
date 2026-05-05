import serial
import pgzrun

ser = serial.Serial('/dev/tty.usbmodem1101', 115200)

WIDTH = 600
HEIGHT = 600
TILE = 40

player_x = WIDTH // 2
player_y = HEIGHT - TILE

# store previous states for edge detection
prev_L = 0
prev_J = 0
prev_R = 0


def update():
    global player_x, player_y
    global prev_L, prev_J, prev_R

    line = ser.readline().decode().strip()

    try:
        parts = line.split()

        L = int(parts[0].split(":")[1])
        J = int(parts[1].split(":")[1])
        R = int(parts[2].split(":")[1])

        # EDGE DETECTION (0 -> 1 press)
        if L == 1 and prev_L == 0:
            player_x -= TILE

        if R == 1 and prev_R == 0:
            player_x += TILE

        if J == 1 and prev_J == 0:
            player_y -= TILE

        prev_L, prev_J, prev_R = L, J, R

        # boundaries
        player_x = max(0, min(WIDTH - TILE, player_x))
        player_y = max(0, min(HEIGHT - TILE, player_y))

    except:
        pass  # ignore malformed serial lines


def draw():
    screen.fill((30, 30, 30))

    screen.draw.filled_rect(
        Rect((player_x, player_y), (TILE, TILE)),
        (0, 255, 0)
    )

    # optional grid
    for x in range(0, WIDTH, TILE):
        screen.draw.line((x, 0), (x, HEIGHT), (50, 50, 50))

    for y in range(0, HEIGHT, TILE):
        screen.draw.line((0, y), (WIDTH, y), (50, 50, 50))


pgzrun.go()