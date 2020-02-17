import serial
from time import sleep
import sys


COM = 'COM3' #/dev/ttyACM0 (Linux)
BAUD_RATE = 9600

ser = serial.Serial(COM, BAUD_RATE, timeout = .1)

print('Waiting for device')
sleep(3)
print(ser.name)

while True:
	val = str(ser.readline().decode().strip('\r\n'))#Capture serial output as a decoded string
	valA = val.split("/")
	print(val, end="\r", flush=True)

    