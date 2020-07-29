import serial
import serial.tools.list_ports
import io
import message
import traceback
import time

class Comm:
    def __init__(self):
        self._ser=None
        self.port=None
        self.id=None
        self.connects=0
        self.failures=0
        self.decode=0
    def connect(self):
        self.connects=self.connects+1
        ports = list(serial.tools.list_ports.comports())
        for port in ports:
            port_no=port.device
            description=port.description
            vid=port.vid
            pid=port.pid
            if(vid==0x2341): continue #skip nano USN
            if (
                    'USB-Serial' in description or
                    'USB-to-Serial' in description or
                    'USB Serial' in description
                ):
                ser=None
                try:
                    ser=serial.Serial(port=port_no, baudrate=57600, timeout=0.05)
                except Exception as e:
                    self.failures=self.failures+1
                    continue
                for i in range(1000):
                    m=message.Message()
                    line=None
                    line=self._readPacket(ser,False)
                    if line is None: continue
                    try:
                        m.decode(line)
                    except:
                        self.decode=self.decode+1
                        continue
                    if(m.status==m.ERR_OK):
                        self._ser=ser
                        self.port=port_no
                        self.id=m.intData
                        return
                    else: break
                ser.close()

    def readPacket(self):
        if(self._ser is None): self.connect()
        if(self._ser is None): return None
        line=self._readPacket(self._ser,True)
        if(line is None): self._ser=None
        return line

    def _readPacket(self,ser,reconnect=True):
        l=''
        while(True):
            try:
                c = ser.read().decode('UTF-8')
            except Exception as e:
                return None
            if(c!='-'):
                l=l+c
            else:
                return l

    def write(self,data):
        if self._ser is None: self.connect()
        else:
            try:
                self._ser.write(data)
            except:
                self._ser=None
