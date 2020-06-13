import serial
import serial.tools.list_ports
import io
import message
import traceback

class Comm:
    def __init__(self):
        self._ser=None
        self.port=None
        self.id=None
        self.connects=0
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
                try:
                    ser=serial.Serial(port=port_no, baudrate=57600, timeout=1)
                except:
                    return
                for i in range(1000):
                    m=message.Message()
                    self._ser=ser
                    line=self.readPacket()
                    self._ser=None
                    if line is None: continue
                    try:
                        m.decode(line)
                    except: continue
                    if(m.status!=m.ERR_OK): continue
                    if(m.id==m.CPU_ID and m.nInt==4):
                        self._ser=ser
                        self.port=port_no
                        self.id=m.intData
                        break
            if self._ser:                
                return

    def readPacket(self):
        if(self._ser is None): self.connect()
        if(self._ser is None): return
        l=''
        while(True):
            try:
                c = self._ser.read().decode('UTF-8')
            except:
                self._ser=None
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

    
