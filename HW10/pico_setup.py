import digitalio
import board
import time

button = digitalio.DigitalInOut(board.GP15)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.DOWN  # key line

while True:
    print("(" + str(int(button.value)) + ")")
    time.sleep(1/30)
