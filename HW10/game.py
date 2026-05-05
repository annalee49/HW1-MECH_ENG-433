import serial
import pgzrun
import random

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

LANES = 8
lane_y = [i * 60 for i in range(1, LANES)]

cars = []

# create cars per lane
for y in lane_y:
    for i in range(3):
        cars.append({
            "x": random.randint(0, WIDTH),
            "y": y,
            "speed": random.choice([-3, 3])
        })

def update():
    global player_x, player_y
    global prev_L, prev_J, prev_R

    line = ser.readline().decode().strip()

    for car in cars:
        car["x"] += car["speed"]

        # wrap around screen
        if car["x"] > WIDTH:
            car["x"] = -60
        if car["x"] < -60:
            car["x"] = WIDTH

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

    for car in cars:
        if (
            player_x < car["x"] + 40 and
            player_x + 40 > car["x"] and
            player_y == car["y"]
        ):
            print("💥 DEAD")
            player_x = WIDTH // 2
            player_y = HEIGHT - TILE


def draw():
    screen.fill((30, 30, 30))

    screen.draw.filled_rect(
        Rect((player_x, player_y), (TILE, TILE)),
        (0, 255, 0)
    )
    for car in cars:
        screen.draw.filled_rect(
            Rect((car["x"], car["y"]), (40, 40)),
            (255, 0, 0)
        )

    # optional grid
    for x in range(0, WIDTH, TILE):
        screen.draw.line((x, 0), (x, HEIGHT), (50, 50, 50))

    for y in range(0, HEIGHT, TILE):
        screen.draw.line((0, y), (WIDTH, y), (50, 50, 50))


pgzrun.go()