
import serial
import time
import threading
import math
import sys
import traceback
import numpy
import message
import serial.tools.list_ports


class AmbuControl(object):

    # State constants
    RunStates = { 0:'StateForceOff', 1:'StateForceOn', 2:'StateRunOff', 3:'StateRunOn' }

    # Config constants
    ConfigKey = { 'GetConfig'    : 0, 'SetRespRate' : 1, 'SetInhTime'   : 2, 'SetPipMax'     : 3,
                  'SetPipOffset' : 4, 'SetVolMax'   : 5, 'SetVolOffset' : 6, 'SetVolInThold' : 7,
                  'SetPeepMin'   : 8, 'SetRunState' : 9, 'ClearAlarm'   : 10 }

    # Status constants
    StatusKey = { 'AlarmPipMax'  : 0x01, 'AlarmVolMax' : 0x02, 'Alarm12V' : 0x04, 'Alarm9V' : 0x08, 'VolInh' : 0x10 }

    def __init__(self):

        self._ser = None #serial.Serial(port=dev, baudrate=57600, timeout=1.0)
        self._runEn = False
        self._dataCallBack  = self._debugCallBack
        self._stateCallBack = None
        self._file = None
        self._last = None
        self._gotConf = False

        self._respRate = 0
        self._inhTime = 0
        self._pipMax = 0
        self._pipOffset = 0
        self._volMax = 0
        self._volOffset = 0
        self._volInThold = 0
        self._peepMin = 0
        self._runState = 0

        self._status = 0

        self._stime = 0
        self._smillis = -1
        self._refresh = time.time()
        self._version='unknown'
        self._cpuid=[0]*4
        self.artime = 0

        self._data = npfifo(9,6000)

    def openLog(self, fName):
        self._file = open(fName,'a')

    def closeLog(self):
        self._file.close()
        self._file = None

    def _debugCallBack(self,data,count,*args):
        l = len(self._data.get_i())
        print(f"Got data. Len={l} count={count}")

    def setDataCallBack(self, callBack):
        self._dataCallBack = callBack

    def setStateCallBack(self, callBack):
        self._stateCallBack = callBack

    @property
    def version(self):
        return self._version

    @property
    def respRate(self):
        return self._respRate

    @respRate.setter
    def respRate(self,value):
        self._respRate = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._respRate),(self.ConfigKey['SetRespRate']));
        self._write(data)

    @property
    def inhTime(self):
        return self._inhTime

    @inhTime.setter
    def inhTime(self,value):
        self._inhTime = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._inhTime),(self.ConfigKey['SetInhTime']));
        self._write(data)

    @property
    def pipMax(self):
        return self._pipMax

    @pipMax.setter
    def pipMax(self,value):
        self._pipMax = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._pipMax),(self.ConfigKey['SetPipMax']));
        self._write(data)

    @property
    def pipOffset(self):
        return self._pipOffset7

    @pipOffset.setter
    def pipOffset(self,value):
        self._pipOffset = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._pipOffset),(self.ConfigKey['SetPipMaxOffset']))
        self._write(data)

    @property
    def volMax(self):
        return self._volMax

    @volMax.setter
    def volMax(self,value):
        self._volMax = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._pipOffset),(self.ConfigKey['SetPipMaxOffset']))
        self._write(data)


    @property
    def volOffset(self):
        return self._volOffset

    @volOffset.setter
    def volOffset(self,value):
        self._volOffset = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._volMax),(self.ConfigKey['SetVolMax']))
        self._write(data)

    @property
    def volInThold(self):
        return self._volInThold

    @volInThold.setter
    def volInThold(self,value):
        self._volInThold = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._volInThold),(self.ConfigKey['SetVolInThold']))
        self._write(data)

    @property
    def peepMin(self):
        return self._peepMin

    @peepMin.setter
    def peepMin(self,value):
        self._peepMin = value
        m=Message.message()
        data=m.writeData(m.PARAM_FLOAT,0,(self._peepMin),(self.ConfigKey['SetPeepMin']))
        self._write(data)

    @property
    def runState(self):
        return self._runState

    @runState.setter
    def runState(self,value):
        self._runState = value
        m=Message.message()
        data=m.writeData(m.PARAM_INTEGER,0,(),(self.ConfigKey['SetRunState'],self._runState))
        self._write(data)

    @property
    def alarmVolMax(self):
        return ((self._status & self.StatusKey['AlarmVolMax']) != 0)

    @property
    def alarmPipMax(self):
        return ((self._status & self.StatusKey['AlarmPipMax']) != 0)

    @property
    def alarm9V(self):
        return ((self._status & self.StatusKey['Alarm9V']) != 0)

    @property
    def alarm12V(self):
        return ((self._status & self.StatusKey['Alarm12V']) != 0)

    @property
    def volInhFlag(self):
        return ((self._status & self.StatusKey['VolInh']) != 0)

    def clearAlarm(self):
        m=Message.message()
        data=m.writeData(m.PARAM_SET,0,(),())
        self._write(data)
        self._status = 0

    def stop(self):
        self._runEn = False
        self._thread.join()

    def start(self):
        self._runEn = True
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()
        self.requestConfig()

    def requestConfig(self):
        self._write(f"CONFIG {self.ConfigKey['GetConfig']} 0\n".encode('UTF-8'))

    def _write(self,data):
        if self._ser is not None:
            try:
                self._write(data)
            except:
                self._ser=None

    def _readPacket(self):
        if(self._ser is None): return None
        l=''
        while(True):
            try:
                c = self._ser.read().decode('UTF-8')
                if(c!='-'):
                    l=l+c
                else:
                    c1= self._ser.read().decode('UTF-8')
                    c2= self._ser.read().decode('UTF-8')
                    if(c1 == '-' and c2 =='-'):
                        return l
                    else:
                        return None
            except:
                self._ser=None
                return None
    def _connect(self):
        ports = list(serial.tools.list_ports.comports())
        for port_no, description, address in ports:
            if 'USB-Serial' in description or 'USB-to-Serial' in description:
                ser=serial.Serial(port=port_no, baudrate=57600, timeout=1.0)
                for i in range(1000):
                    try:
                        m=message.Message()
                        self._ser=ser
                        line=self._readPacket()
                        self._ser=None
                        if line is None: continue
                        m.decode(line)
                        if(m.status!=m.ERR_OK): continue
                        if(m.id==m.CPU_ID and m.nInt==4):
                            new_cpuid=m.intData
                            if(self._cpuid!=new_cpuid):
                                pass #put some code her for new connection
                            else:
                                pass # resume old connection
                            self._ser=ser
                            break
                    except:
                        ser.close()
                        self._ser=None
                        break
            if self._ser: return






    def _handleSerial(self):
        counter=0
        while self._runEn:
            try:
                if self._ser is None:
                    self._connect()
                line=self._readPacket()
                if(line is None): continue
                ts = time.time()
                m=message.Message()
                try:
                    m.decode(line)
                except:
                    pass
                if(m.status!=m.ERR_OK): continue
                if(m.id == m.VERSION):
                    self._version=m.string
                if(m.id == m.CPU_ID and m.nInt==4):
                    self._cpuid=m.intData
                elif m.id == m.CONFIG  and m.nFloat==8 and m.nInt==1:
                    data=m.floatData
                    state=m.intData
                    #print(f"Got config: {line.rstrip()}")
                    doNotify = False
                    nconf = {}

                    nconf['_respRate']    = data[0]
                    nconf['_inhTime']     = data[1]
                    nconf['_pipMax']      = data[2]
                    nconf['_pipOffset']   = data[3]
                    nconf['_volMax']      = data[4]
                    nconf['_volOffset']   = data[5]
                    nconf['_volInThold']  = data[6]
                    nconf['_peepMin']     = data[7]
                    nconf['_runState']    = state

                    for k,v in nconf.items():
                        if v != getattr(self,k):
                            doNotify = True
                        setattr(self,k,v)

                    if ((not self._gotConf) or doNotify) and self._stateCallBack is not None:
                        self._gotConf = True
                        self._stateCallBack()

                elif self._gotConf and m.id == m.DATA  and m.nFloat==5 and m.nInt==2:
                    #print(f"Got status: {line.rstrip()}")
                    millis = m.millis
                    data=  m.floatData
                    idata  = m.intData
                    count  = idata[0]
                    status = idata[1]
                    volMax = data[0]
                    pipMax = data[1]
                    press  = data[2]
                    flow   = data[3]
                    vol    = data[4]

                    doStatus = (status != self._status)
                    self._status = status

                    if self._smillis == -1:
                        self._smillis=millis
                        self._stime = time.time()
                        continue
                    else:
                        diffT=(millis-self._smillis)/1000.
                        if(diffT<=0): continue

                    stime  = ts - self._stime
                    artime= millis/1000.
                    self._data.append([diffT, count, press, flow, vol, self.volInThold, self.pipMax, self.volMax, self.peepMin])

                    if self._file is not None:
                        self._file.write(f'{ts}, {count}, {press}, {flow}, {vol}\n')

                    if time.time() - self._refresh > 0.5:
                        self._refresh = time.time()

                        try:
                            num_points = self._data.get_n()
                            denom= (self._data.A[0,-1] - self._data.get_nextout_time())
                            if denom!=0:
                                rate = num_points / (self._data.A[0,-1] - self._data.get_nextout_time())
                            else:
                                rate=0
                        except Exception as e:
                            #traceback.print_exc()
                            #print(f"Got error {e}")
                            #print(num_points, self._data.A[0,-1], self._data.get_nextout_time())
                            rate=0.

                        try:
                            self._dataCallBack(self._data, count, rate, stime, artime, volMax, pipMax)
                            #print(f"Got status: {line.rstrip()}")
                        except Exception as e:
                            #traceback.print_exc()
                            #print("Got callback error {}".format(e))
                            pass

                    if doStatus and self._stateCallBack is not None:
                        self._stateCallBack()


            except Exception as e:
                #traceback.print_exc()
                #print(f"Got handleSerial error {e}")
                pass


class npfifo:
    def __init__(self, num_parm, num_points):
        self._n = num_parm
        self._x = num_points
        self.A = numpy.zeros((self._n, self._x))
        self._i = 0

    def append(self, X):
        if len(X) != self._n:
            #print("Wrong number of parameters to append, ignoring")
            return
        # Move the data in the buffer
        self.A[:,:-1] = self.A[:,1:]
        # add the data to the end of the buffer
        self.A[:,-1] = X
        # increment number of data-points entered
        self._i += 1

    def clear(self):
        self.A = 0.0
        self._i = 0

    def get_data(self):
        if self._i > 1:
            # Returns data array up to the minimum of length or entries
            return self.A[:,-min(self._i, self._x):]
        else:
            return None

    def get_i(self):
        return self._i

    def get_n(self):
        return min(self._i, self._x)

    def get_nextout_time(self):
        return self.A[0, -(self.get_n()-1)]
