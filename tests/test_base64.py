import serial
import base64
import serial.tools.list_ports
import traceback
import struct
import collections


class Message():
    def __init__(self):
        self._MSGID_DATA=0xc1;
        self._MSGID_VERSION=0xc2;
        self._MSGID_CONFIG=0xc3;
        self._MSGID_DEBUG=0xc4;

    def decode(self,msg):
        print(len(msg),msg)
        dec=base64.b64decode(msg)
        header=struct.unpack("<BIB",dec[:6])
        l=header[-1]
        int_len=(l>>4);
        float_len=(l&0xf);
        data=struct.unpack("<"+str(float_len)+"f"+str(int_len)+  "IH",dec[6:])
        try:            
            checksum=self._fletcher16(dec[:-2])
            print(checksum)
        except:
            throw()
        return data
    def encode(f,i):
        pass
    def _fletcher16(self,data):
        sum1=0
        sum2=0
        for i in range(len(data)):
            sum1=((sum1+data[i]) & 0xffff) % 255
            sum2=((sum2+sum1) & 0xffff) % 255
        result=((sum2 << 8) | sum1) & 0xffff
        return result
        


ser = serial.Serial("/dev/ttyACM0", 57600, timeout=.1)
while(True):
    line=''
    raw = ser.read_until("---")
    line = raw.decode('UTF-8')
    line=line.rstrip('---')

    if(len(line)==0): continue
    try:
        m=Message().decode(line)        
        print(m)
    except:
        pass




