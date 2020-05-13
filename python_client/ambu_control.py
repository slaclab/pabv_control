
import serial
import time
import threading
import math
import sys
import traceback

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
        self._state = 0

        self._initData()

        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()

    def _initData(self):
        self._data = {'time': [], 'press': [], 'flow':[], 'vol':[]}

    def openLog(self, fName):
        self._file = open(fName,'a')

    def closeLog(self):
        self._file.close()
        self._file = None

    def _debugCallBack(self,data,count):
        l = len(self._data['time'])
        print(f"Got data. Len={l} count={count}")

    def setDataCallBack(self, callBack):
        self._dataCallBack = callBack

    def setConfCallBack(self, callBack):
        self._confCallBack = callBack

    @property
    def cycleRate(self):
        return (1.0 / ((self._period / 1000.0) / 60.0))

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
        print("Send Config: " + msg.rstrip())

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

                elif data[0] == 'STATUS' and len(data) == 5:
                    #print(f"Got status: {line.rstrip()}")
                    count = int(data[1],0)
                    press = float(data[2])
                    flow  = float(data[3])
                    vol   = float(data[4])

                    if self._file is not None:
                        self._file.write(f'{ts}, {count}, {press}, {flow}, {vol}\n')

                    if self._last is None:
                        self._last = count
                    elif self._last != count:
                        self._last = count

                        try:
                            self._dataCallBack(self._data, count)
                        except Exception as e:
                            traceback.print_exc()
                            print("Got error {}".format(e))

                        self._initData()

                    self._data['time'].append(ts)
                    self._data['press'].append(press)
                    self._data['flow'].append(flow)
                    self._data['vol'].append(vol)

            except Exception as e:
                traceback.print_exc()
                print(f"Got error {e}")

