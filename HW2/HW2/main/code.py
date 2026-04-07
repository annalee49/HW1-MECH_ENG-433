from machine import Pin, PWM, ADC, Timer
import time

PWMPIN = 16

# Set up PWM on pin 16
pwm_pin = Pin(PWMPIN)
pwm = PWM(pwm_pin)
pwm.freq(10000)  # 10 kHz

# PWM parameters
min_level = 1000
max_level = 8000
level = min_level
step = 10

# The RP2040 PWM counter max value is 65535 by default; 
# MicroPython's PWM duty is 16-bit (0-65535), so we scale accordingly.
# You may need to adjust this for your exact timing needs.

# Set up ADC on pin 26 (ADC0)
adc = ADC(26)

# Timer callback function to read ADC and print voltage
def timer_callback(timer):
    result = adc.read_u16()  # 16-bit ADC reading (0-65535)
    voltage = (result / 65535) * 3.3
    print("{:.3f} V".format(voltage))

# Set up repeating timer every 100 ms
timer = Timer()
timer.init(freq=10, mode=Timer.PERIODIC, callback=timer_callback)

try:
    while True:
        # Update PWM duty cycle
        # Scale level (range 1000-8000) to 16-bit duty (0-65535)
        duty = int((level / 19999) * 65535)
        pwm.duty_u16(duty)

        level += step
        if level >= max_level or level <= min_level:
            step = -step

        time.sleep(0.2)

except KeyboardInterrupt:
    # Cleanup on exit
    pwm.deinit()
    timer.deinit()
    print("Program stopped")
