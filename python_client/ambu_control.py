
import serial
import time
import threading
import math
import sys
import traceback
import numpy

class AmbuControl(object):

    def __init__(self, dev):

        self._ser = serial.Serial(dev, 57600, timeout=1.0)

        self._runEn = True
        self._dataCallBack = self._debugCallBack
        self._confCallBack = None
        self._file = None
        self._last = None
        self._period = 0
        self._onTime = 0
        self._startThold = 0
        self._stopThold = 0
        self._volThold = 0
        self._state = 0
        self._stime = time.time()
        self._smillis = -1
        self._refresh = time.time()

        #self._data = {'time': [], 'count':[], 'press': [], 'flow':[], 'vol':[], 'maxP': [], 'inhP': [], 'maxV': []}
        self._data = npfifo(8,6000)

        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()

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

    def setConfCallBack(self, callBack):
        self._confCallBack = callBack

    @property
    def cycleRate(self):
        try:
            rate=(1.0 / ((self._period / 1000.0) / 60.0))
        except:
            rate=0.0
        return rate

    @cycleRate.setter
    def cycleRate(self,value):
        self._period = int((1.0 / float(value)) * 1000.0 * 60.0)
        self._setConfig()

    @property
    def onTime(self):
        return self._onTime / 1000.0

    @onTime.setter
    def onTime(self,value):
        self._onTime = int(value * 1000)
        self._setConfig()

    @property
    def startThold(self):
        return self._startThold

    @startThold.setter
    def startThold(self,value):
        self._startThold = value
        self._setConfig()

    @property
    def stopThold(self):
        return self._stopThold

    @stopThold.setter
    def stopThold(self,value):
        self._stopThold = value
        self._setConfig()

    @property
    def volThold(self):
        return self._volThold

    @volThold.setter
    def volThold(self,value):
        self._volThold = value
        self._setConfig()

    @property
    def state(self):
        return self._state

    @state.setter
    def state(self,value):
        self._state = value
        self._setConfig()

    def stop(self):
        self._runEn = False
        self._thread.join()

    def _setConfig(self):
        msg = f"CONFIG {self._period} {self._onTime} {self._startThold:.2f} {self._state} {self._stopThold:.2f} {self._volThold:.2f}\n"
        self._ser.write(msg.encode('UTF-8'))
        #print("Send Config: " + msg.rstrip())

    def _handleSerial(self):
        while self._runEn:
            try:
                raw = self._ser.readline()
                line = raw.decode('UTF-8')

                data = line.rstrip().split(' ')
                ts = time.time()

                if data[0] == 'DEBUG':
                    #print(f"Got debug: {line.rstrip()}")
                    pass

                elif data[0] == 'CONFIG':
                    #print(f"Got config: {line.rstrip()}")
                    doNotify = False

                    period     = int(data[1],0)
                    onTime     = int(data[2],0)
                    startThold = float(data[3])
                    state      = int(data[4],0)
                    stopThold  = float(data[5])
                    volThold   = float(data[6])

                    if (self._period != period) or \
                       (self._onTime != onTime) or \
                       (self._startThold != startThold) or \
                       (self._stopThold != stopThold) or \
                       (self._state != state) or \
                       (self._volThold != volThold):
                       doNotify = True

                    self._period = period
                    self._onTime = onTime
                    self._startThold = startThold
                    self._state = state
                    self._stopThold = stopThold
                    self._volThold = volThold

                    if doNotify and self._confCallBack is not None:
                        self._confCallBack()

                elif data[0] == 'STATUS' and len(data) == 6:
                    #print(f"Got status: {line.rstrip()}")
                    millis = int(data[1],0) 
                    count  = int(data[2],0)
                    press  = float(data[3])
                    flow   = float(data[4])
                    vol    = float(data[5])
                    #diffT  = ts - self._stime
                    if self._smillis == -1: 
                        self._smillis=millis
                        continue
                    else:
                        diffT=(millis-self._smillis)/1000.
                        if(diffT<=0): continue
                    self._data.append([diffT, count, press, flow, vol, self.startThold, self.stopThold, self.volThold])

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
                            self._dataCallBack(self._data, count, rate)
                        except Exception as e:
                            traceback.print_exc()
                            print("Got error {}".format(e))

            except Exception as e:
                traceback.print_exc()
                print(f"Got error {e}")


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
        
    
