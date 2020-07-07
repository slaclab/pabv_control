import time
import threading
import math
import sys
import traceback
import message
import comm
import queue
import npfifo



class AmbuControl(object):

    # State constants
    RunStates = { 0:'StateForceOff', 1:'StateForceOn', 2:'StateRunOff', 3:'StateRunOn' }

    # Mode constants
    RunModes = { 0:'ModeVolume', 1:'ModePressure' }

    # Cycle States
    CycleStates = { 0:'StateOff', 1:'StateOn', 2:'StateHold' }

    # Config constants
    ConfigKey = { 'GetConfig'    : 0, 'SetRespRate' : 1, 'SetInhTime'   : 2,  'SetPipMax'     : 3,
                  'SetPipOffset' : 4, 'SetVolMax'   : 5, 'SetVolFactor' : 6,  'SetVolInThold' : 7,
                  'SetPeepMin'   : 8, 'SetRunState' : 9, 'MuteAlarm'    : 10, 'SetRunMode'    : 11}

    # Status constants
    StatusKey = { 'AlarmPipMax'  : 0x01, 'AlarmVolLow' : 0x02, 'Alarm12V'     : 0x04,
                  'Warn9V'       : 0x08, 'VolInh'      : 0x10, 'AlarmPresLow' : 0x20,
                  'WarnPeepMin'  : 0x40 }

    def __init__(self):

        self._ser = comm.Comm()
        self._runEn = False
        self._configCallBack = None
        self._file = None
        self._last = None

        self._respRate = 0
        self._inhTime = 0
        self._pipMax = 0
        self._pipOffset = 0
        self._volMax = 0
        self._volFactor = 0
        self._volInThold = 0
        self._peepMin = 0
        self._runState = 0

        self._status = 0
        self._prevmillis=0
        self._stime = 0
        self._smillis = -1
        self._refresh = time.time()
        self._version='unknown'
        self._cpuid=[0]*4
        self.artime = 0
        self._tOffset=0
        self._timestamp = time.time()
        self._cfgSerialNum = 0
        self._queue=None

        self._data = npfifo.npfifo(9,6000)

    def openLog(self, fName):
        self._file = open(fName,'a')

    def closeLog(self):
        self._file.close()
        self._file = None

    def setConfigCallBack(self, callBack):
        self._configCallBack = callBack

    @property
    def version(self):
        return self._version.decode("utf8")

    @property
    def respRate(self):
        return self._respRate

    @respRate.setter
    def respRate(self,value):
        self._respRate = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._respRate],[self.ConfigKey['SetRespRate']]);
        self._ser.write(data)

    @property
    def inhTime(self):
        return self._inhTime

    @inhTime.setter
    def inhTime(self,value):
        self._inhTime = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._inhTime],[self.ConfigKey['SetInhTime']]);
        self._ser.write(data)

    @property
    def pipMax(self):
        return self._pipMax

    @pipMax.setter
    def pipMax(self,value):
        self._pipMax = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._pipMax],[self.ConfigKey['SetPipMax']]);
        self._ser.write(data)

    @property
    def pipOffset(self):
        return self._pipOffset

    @pipOffset.setter
    def pipOffset(self,value):
        self._pipOffset = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._pipOffset],[self.ConfigKey['SetPipOffset']])
        self._ser.write(data)

    @property
    def volMax(self):
        return self._volMax

    @volMax.setter
    def volMax(self,value):
        self._volMax = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._volMax],[self.ConfigKey['SetVolMax']])
        self._ser.write(data)

    @property
    def volFactor(self):
        return self._volFactor

    @volFactor.setter
    def volFactor(self,value):
        self._volFactor = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._volFactor],[self.ConfigKey['SetVolFactor']])
        self._ser.write(data)

    @property
    def volInThold(self):
        return self._volInThold

    @volInThold.setter
    def volInThold(self,value):
        self._volInThold = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._volInThold],[self.ConfigKey['SetVolInThold']])
        self._ser.write(data)

    @property
    def peepMin(self):
        return self._peepMin

    @peepMin.setter
    def peepMin(self,value):
        self._peepMin = value
        m=message.Message()
        data=m.writeData(m.PARAM_FLOAT,0,[self._peepMin],[self.ConfigKey['SetPeepMin']])
        self._ser.write(data)

    @property
    def runState(self):
        return self._runState

    @runState.setter
    def runState(self,value):
        self._runState = value
        m=message.Message()
        data=m.writeData(m.PARAM_INTEGER,0,[],[self.ConfigKey['SetRunState'],self._runState])
        self._ser.write(data)

    @property
    def runMode(self):
        return self._runMode

    @runMode.setter
    def runMode(self,value):
        self._runMode = value
        m=message.Message()
        data=m.writeData(m.PARAM_INTEGER,0,[],[self.ConfigKey['SetRunMode'],self._runMode])
        self._ser.write(data)

    @property
    def alarmPipMax(self):
        return ((self._status & self.StatusKey['AlarmPipMax']) != 0)

    @property
    def alarmVolLow(self):
        return ((self._status & self.StatusKey['AlarmVolLow']) != 0)

    @property
    def alarm12V(self):
        return ((self._status & self.StatusKey['Alarm12V']) != 0)

    @property
    def warn9V(self):
        return ((self._status & self.StatusKey['Warn9V']) != 0)

    @property
    def volInhFlag(self):
        return ((self._status & self.StatusKey['VolInh']) != 0)

    @property
    def alarmPresLow(self):
        return ((self._status & self.StatusKey['AlarmPresLow']) != 0)

    @property
    def warnPeepMin(self):
        return ((self._status & self.StatusKey['WarnPeepMin']) != 0)

    @property
    def currState(self):
        return ((self._status >> 24) & 0xFF)
    @property
    def serialNum(self):
        return self._cfgSerialNum
    @property
    def cpuId(self):
        return self._cpuid
    @property
    def com(self):
        return self._ser.port
    def muteAlarm(self):
        m=message.Message()
        data=m.writeData(m.PARAM_SET,0,[],[self.ConfigKey['MuteAlarm']  ])
        self._ser.write(data)
        self._status = 0

    def requestConfig(self):
        m=message.Message()
        data=m.writeData(m.PARAM_SET,0,[],[self.ConfigKey['GetConfig']])
        self._ser.write(data)
    def setQueue(self,queue):
        self._queue=queue

    def stop(self):
        self._runEn = False
        self._thread.join()


    def start(self):
        self._runEn = True
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()
        self.requestConfig()


    def _handleSerial(self):
        counter=0
        while self._runEn:
            line=self._ser.readPacket()
            if(line is None): continue
            ts = time.time()
            m=message.Message()
            try:
                m.decode(line)
            except:
                continue
            if(m.status!=m.ERR_OK): continue
            if(m.id == m.VERSION):
                self._version=m.string
            if(m.id == m.CPU_ID and m.nInt==4):
                self._cpuid="%08x%08x%08x%08x" % m.intData
            elif m.id == m.CONFIG  and m.nFloat==8 and m.nInt==3:
                newSerial = m.intData[1]

                if newSerial != self._cfgSerialNum:
                    self._cfgSerialNum = newSerial
                    self._respRate    = m.floatData[0]
                    self._inhTime     = m.floatData[1]
                    self._pipMax      = m.floatData[2]
                    self._pipOffset   = m.floatData[3]
                    self._volMax      = m.floatData[4]
                    self._volFactor   = m.floatData[5]
                    self._volInThold  = m.floatData[6]
                    self._peepMin     = m.floatData[7]
                    self._runState    = m.intData[0]
                    self._runMode     = m.intData[2]

                    if (self._configCallBack is not None):
                        self._configCallBack()

            elif self._cfgSerialNum != 0 and m.id == m.DATA and m.nFloat==5 and m.nInt==2:
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

                self._status = status

                if self._smillis == -1:
                    self._smillis=millis
                    self._stime = time.time()
                    continue
                else:
                    # handle overflow and arduino reset case
                    if(millis<self._prevmillis):
                        self._smillis=millis
                        reboot_time=time.time()-self._timestamp
                        #we could count resets here
                        self._tOffset=self._diffT+reboot_time
                        self._timestamp=time.time()
                        self._prevmillis=millis
                        continue
                    diffT=(millis-self._smillis)/1000.+self._tOffset
                    if(diffT<=0): continue
                    self._diffT=diffT
                    self._timestamp=time.time()
                    self._prevmillis=millis
                stime  = ts - self._stime
                artime= millis/1000.
                self._data.append([diffT, count, press, flow, vol, self.volInThold, self.pipMax, self.volMax, self.peepMin])

                if self._file is not None:
                    self._file.write(f'{ts}, {status}, {count}, {press}, {flow}, {vol}\n')

                if time.time() - self._refresh > 0.08:
                    self._refresh = time.time()

                    num_points = self._data.get_n()
                    denom= (self._data.A[0,-1] - self._data.get_nextout_time())
                    if denom!=0:
                        rate = num_points / (self._data.A[0,-1] - self._data.get_nextout_time())
                    else:
                        rate=0
                    qe=[self._data, count, rate, stime, artime, volMax, pipMax]
                    try:
                        self._queue.put(qe,block=False)
                    except:
                        pass
