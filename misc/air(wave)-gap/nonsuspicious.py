import RPi.GPIO as GPIO
from time import sleep
from os import system
from sys import argv

startup_time = 60
bit_length = 10
led_pin = 13
pwm_freq = 8000

get_bin = lambda x, n: format(x, 'b').zfill(n)
with open('flag.txt', 'r') as f:
    flag = bytearray(f.read().strip().encode())

bits = ''
for byte in flag:
    bits += get_bin(byte, 8)

# Setup GPIO pins
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(led_pin,GPIO.OUT)
pi_pwm = GPIO.PWM(led_pin, pwm_freq)
pi_pwm.start(0)

system('echo none | sudo tee /sys/class/leds/led0/trigger >/dev/null') # Take control of led0

# Starting the fan idling to not draw any suspicion
pi_pwm.ChangeDutyCycle(100)

print(f'[*] Waiting {startup_time}s before transmitting...')
sleep(startup_time)

print(f'[*] Now transmitting data...')
system('echo 1 | sudo tee /sys/class/leds/led0/brightness >/dev/null') # Enable led0 when transmitting

for i, bit in enumerate(bits, 1):
    # Fancy logging no one will ever see :(
    print('[{}] Transmitting bit {}/{}'.format("/-\|"[i%4], i, len(bits)), end='\r')
    power = 20 if bit == '0' else 100
    pi_pwm.ChangeDutyCycle(power)
    sleep(bit_length)

print('[+] Done transmitting data!')
system('echo 0 | sudo tee /sys/class/leds/led0/brightness >/dev/null') # Disable led0 when done