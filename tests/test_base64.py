import serial
import base64
import serial.tools.list_ports
import traceback
import struct
import collections
import sys
sys.path.append("python_client")

import message

ser = serial.Serial("/dev/ttyACM0", 57600, timeout=.1)
while(True):
    line=''
    raw = ser.read_until("---")
    line = raw.decode('UTF-8')
    items=line.split('---')
    for item in items:        
        if(len(item)==0): continue   
        m=message.Message()
        m.decode(item)        
        print(m.intData)
        print(m.floatData)
        print(m.string)

        




