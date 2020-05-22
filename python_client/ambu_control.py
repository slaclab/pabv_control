
import serial
import time
import threading
import math
import sys
import traceback
import numpy
import random



class AmbuControl(object):

    # State constants
    RunStates = { 0:'StateForceOff', 1:'StateForceOn', 2:'StateRunOff', 3:'StateRunOn' }

    # Config constants
    ConfigKey = { 'GetConfig'    : 0, 'SetRespRate' : 1, 'SetInhTime'   : 2, 'SetPipMax'     : 3,
                  'SetPipOffset' : 4, 'SetVolMax'   : 5, 'SetVolOffset' : 6, 'SetVolInThold' : 7,
                  'SetPeepMin'   : 8, 'SetRunState' : 9, 'ClearAlarm'   : 10 }

    # Alarm constants
    AlarmKey = { 'AlarmPipMax'  : 1, 'AlarmVolMax' : 2, 'Alarm12V' : 3, 'Alarm9V' : 8 }

    def __init__(self, dev):

        self._ser = serial.Serial(port=dev, baudrate=57600, timeout=1.0)
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

        self._alarm = 0

        self._stime = 0
        self._smillis = -1
        self._refresh = time.time()
        self._version='unknown'
        self.artime = 0

        self._data = npfifo(9,6000)

    def openLog(self, fName):
        self._file = open(fName,'a')

    def closeLog(self):
        self._file.close()
        self._file = None

    def _debugCallBack(self,data,count):
        # BM: I dont understand the point of this
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
        self._ser.write(f"CONFIG {self.ConfigKey['SetRespRate']} {self._respRate:.2f}\n".encode('UTF-8'))

    @property
    def inhTime(self):
        return self._inhTime

    @inhTime.setter
    def inhTime(self,value):
        self._inhTime = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetInhTime']} {self._inhTime:.2f}\n".encode('UTF-8'))

    @property
    def pipMax(self):
        return self._pipMax

    @pipMax.setter
    def pipMax(self,value):
        self._pipMax = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetPipMax']} {self._pipMax:.2f}\n".encode('UTF-8'))

    @property
    def pipOffset(self):
        return self._pipOffset

    @pipOffset.setter
    def pipOffset(self,value):
        self._pipOffset = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetPipOffset']} {self._pipOffset:.2f}\n".encode('UTF-8'))

    @property
    def volMax(self):
        return self._volMax

    @volMax.setter
    def volMax(self,value):
        self._volMax = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetVolMax']} {self._volMax:.2f}\n".encode('UTF-8'))

    @property
    def volOffset(self):
        return self._volOffset

    @volOffset.setter
    def volOffset(self,value):
        self._volOffset = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetVolOffset']} {self._volOffset:.2f}\n".encode('UTF-8'))

    @property
    def volInThold(self):
        return self._volInThold

    @volInThold.setter
    def volInThold(self,value):
        self._volInThold = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetVolInThold']} {self._volInThold:.2f}\n".encode('UTF-8'))

    @property
    def peepMin(self):
        return self._peepMin

    @peepMin.setter
    def peepMin(self,value):
        self._peepMin = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetPeepMin']} {self._peepMin:.2f}\n".encode('UTF-8'))

    @property
    def runState(self):
        return self._runState

    @runState.setter
    def runState(self,value):
        self._runState = value
        self._ser.write(f"CONFIG {self.ConfigKey['SetRunState']} {self._runState}\n".encode('UTF-8'))

    @property
    def alarmVolMax(self):
        return ((self._alarm & self.AlarmKey['AlarmVolMax']) != 0)

    @property
    def alarmPipMax(self):
        return ((self._alarm & self.AlarmKey['AlarmPipMax']) != 0)

    @property
    def alarm9V(self):
        return ((self._alarm & self.AlarmKey['Alarm9V']) != 0)

    @property
    def alarm12V(self):
        return ((self._alarm & self.AlarmKey['Alarm12V']) != 0)

    def clearAlarm(self):
        self._ser.write(f"CONFIG {self.ConfigKey['ClearAlarm']} 0\n".encode('UTF-8'))
        self._alarm = 0

    def stop(self):
        self._runEn = False
        self._thread.join()

    def start(self):
        self._runEn = True
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()
        #self.requestConfig()

    def requestConfig(self):
        self._ser.write(f"CONFIG {self.ConfigKey['GetConfig']} 0\n".encode('UTF-8'))

    def _handleSerial(self):
        counter=0
        while self._runEn:
            try:
                raw = self._ser.readline()
                line = raw.decode('UTF-8')
                data = line.rstrip().split(' ')
                ts = time.time()

                if data[0] == 'DEBUG':
                    #print(f"Got debug: {line.rstrip()}")
                    pass

                elif data[0] == 'VERSION' and len(data)==2:
                    #print(f"Got version: {line.rstrip()}")
                    self._version=data[1]

                elif data[0] == 'CONFIG' and len(data) == 10:
                    #print(f"Got config: {line.rstrip()}")
                    doNotify = False
                    nconf = {}

                    nconf['_respRate']    = float(data[1])
                    nconf['_inhTime']     = float(data[2])
                    nconf['_pipMax']      = float(data[3])
                    nconf['_pipOffset']   = float(data[4])
                    nconf['_volMax']      = float(data[5])
                    nconf['_volOffset']   = float(data[6])
                    nconf['_volInThold']  = float(data[7])
                    nconf['_peepMin']     = float(data[8])
                    nconf['_runState']    = int(data[9],0)

                    for k,v in nconf.items():
                        if v != getattr(self,k):
                            doNotify = True
                        setattr(self,k,v)

                    if ((not self._gotConf) or doNotify) and self._stateCallBack is not None:
                        self._gotConf = True
                        self._stateCallBack()

                elif self._gotConf and data[0] == 'STATUS' and len(data) == 7:
                    #print(f"Got status: {line.rstrip()}")
                    millis = int(data[1],0)
                    count  = int(data[2],0)
                    alarm  = int(data[3])
                    press  = float(data[4])
                    flow   = float(data[5])
                    vol    = float(data[6])

                    doAlarm = (alarm != self._alarm)
                    self._alarm = alarm

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
                            traceback.print_exc()
                            print(f"Got error {e}")
                            print(num_points, self._data.A[0,-1], self._data.get_nextout_time())
                            rate=0.

                        try:
                            self._dataCallBack(self._data, count, rate, stime, artime)
                            #print(f"Got status: {line.rstrip()}")
                        except Exception as e:
                            traceback.print_exc()
                            print("Got callback error {}".format(e))

                    if doAlarm and self._stateCallBack is not None:
                        self._stateCallBack()


            except Exception as e:
                traceback.print_exc()
                print(f"Got handleSerial error {e}")


class npfifo:
    def __init__(self, num_parm, num_points):
        self._n = num_parm
        self._x = num_points
        self.A = numpy.zeros((self._n, self._x))
        self._i = 0

    def append(self, X):
        if len(X) != self._n:
            print("Wrong number of parameters to append, ignoring")
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

