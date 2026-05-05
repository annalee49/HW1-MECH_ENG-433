import numpy as np 
import pandas as pd
import matplotlib as plt
import pgzero as pgz
import math
import pygame 
import serial
import time
import board
import pyscreen as screen
import pgzrun
import pygame

# =============================================
# OPEN WINDOW AND DRAW A CIRCLE
# =============================================

# size of the window that opens
WIDTH = 800
HEIGHT = 600
# this function is called 60 times per second
def draw():
   screen.clear()
   screen.draw.circle((400, 300), 30, 'white')

# =============================================
# DRAW EYES THAT FOLLOW MOUSE
# =============================================
import math
import pygame
WIDTH = 600
HEIGHT = 600
def update():
   pass
def draw():
   screen.fill((0, 0, 0))
   def draw_eye(eye_x, eye_y):
       mouse_x, mouse_y = pygame.mouse.get_pos()
       screen.draw.text('x: ' + str(mouse_x)+ '\n' + 'y: ' + str(mouse_y),(0, 0))
       distance_x = mouse_x - eye_x
       distance_y = mouse_y - eye_y
       distance = min(math.sqrt(distance_x**2 + distance_y**2), 30)
       angle = math.atan2(distance_y, distance_x)
       pupil_x = eye_x + (math.cos(angle) * distance)
       pupil_y = eye_y + (math.sin(angle) * distance)
       screen.draw.filled_circle((eye_x, eye_y), 50, color=(255, 255, 255))
       screen.draw.filled_circle((pupil_x, pupil_y), 15, color=(0, 0, 100))
   draw_eye(WIDTH/3, HEIGHT/2)
   draw_eye(WIDTH*2/3, HEIGHT/2)

# =============================================
# DRAW MOUSE POSITION AS TEXT
# =============================================
import pygame
import math
WIDTH = 600
HEIGHT = 600
def update():
   pass
def draw():
   screen.fill((0, 0, 0))
   def draw_mouse(x,y):
       screen.draw.text('x: ' + str(x)+ '\n' + 'y: ' + str(y),(0, 0))
       screen.draw.filled_circle((x, y), 10, color=(255, 0, 0))
   mouse_x, mouse_y = pygame.mouse.get_pos()
   draw_mouse(mouse_x, mouse_y)

# =============================================
# PIANO
# =============================================
WIDTH = 400
HEIGHT = 300
beep_a = tone.create('E3', 0.1)
beep_s = tone.create('D3', 0.1)
beep_d = tone.create('C3', 0.1)
# this function is called when a keyboard key is pressed down
def on_key_down(key):
   if key == keys.A:
       beep_a.play()
   if key == keys.S:
       beep_s.play()
   if key == keys.D:
       beep_d.play()
def draw():
   screen.fill((0, 0, 0)) # a black background, (red, green, blue)

# =============================================
# OPEN PORT AND DRAW NUMBER
# =============================================
import serial
ser = serial.Serial('/dev/tty.usbmodem1101') # the name of your port here
print('Opening port: ' + str(ser.name))
import pygame
WIDTH = 600
HEIGHT = 600
n1_int = 0
n2_int = 0
def update():
   n_bytes = ser.readline() # read all the letters available
   s = str(n_bytes) # turn them into a str
   result1 = s[s.find('(')+1:s.find(',')] # find everything beween ( and ,
   result2 = s[s.find(',')+1:s.find(')')] # find everything between , and )
   global n1_int
   n1_int = int(result1) # convert str to int
   global n2_int
   n2_int = int(result2)
def draw():
   screen.fill((0, 0, 0))
   screen.draw.text('a0: ' + str(n1_int)+ '\n' + 'a1: ' + str(n2_int),(0, 0))

# =============================================
# PICO
# =============================================
import time # for time.sleep()
import board # for pin names
import analogio # for adc

a0 = analogio.AnalogIn(board.A0) # read the voltage on A0
a1 = analogio.AnalogIn(board.A1) # read the voltage on A1

while 1:
    print("("+str(a0.value)+","+str(a1.value)+")")
    time.sleep(1/30) # print out 30 times per second
