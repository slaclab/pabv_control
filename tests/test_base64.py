import serial
import base64
import serial.tools.list_ports
import traceback
import struct
import collections
import sys
sys.path.append("python_client")

import message

ser = serial.Serial("/dev/ttyUSB0", 57600, timeout=.1)
l=''
while(True):
    c = ser.read().decode('UTF-8')
    if(c!='-'): 
        l=l+c
    else:
        c1= ser.read().decode('UTF-8')
        c2= ser.read().decode('UTF-8')
        if(c1 == '-' and c2 =='-'):
           m=message.Message()
           m.decode(l)        
           print(m.string)
           print(m.floatData)
        l=''
           

        




