
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
        return convertDlcL20dD4(self._startThold * 256)

    @startThold.setter
    def startThold(self,value):
        thold = int(convertDlcL20dD4Reverse(value) / 256)

        if ( thold > 0xFFFF):
            thold = 0xFFFF
        elif (thold < 0 ):
            thold = 0

        self._startThold = thold
        self._setConfig()

    @property
    def stopThold(self):
        return convertDlcL20dD4(self._stopThold * 256)

    @stopThold.setter
    def stopThold(self,value):
        thold = int(convertDlcL20dD4Reverse(value) / 256)

        if ( thold > 0xFFFF):
            thold = 0xFFFF
        elif (thold < 0 ):
            thold = 0

        self._stopThold = thold
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
        msg = f"CONFIG {self._period} {self._onTime} {self._startThold} {self._state} {self._stopThold}\n"
        self._ser.write(msg.encode('UTF-8'))
        print(msg)

    def _handleSerial(self):
        print("Starting thread")

        while self._runEn:
            try:
                raw = self._ser.readline()
                line = raw.decode('UTF-8')

                data = line.rstrip().split(' ')
                ts = time.time()

                if data[0] == 'DEBUG':
                    print(f"Got debug: {line}")

                elif data[0] == 'CONFIG':
                    #print(f"Got config: {line}")
                    doNotify = False

                    period     = int(data[1],0)
                    onTime     = int(data[2],0)
                    startThold = int(data[3],0)
                    state      = int(data[4],0)
                    stopThold  = int(data[5],0)

                    if (self._period != period) or \
                       (self._onTime != onTime) or \
                       (self._startThold != startThold) or \
                       (self._stopThold != stopThold) or \
                       (self._state != state):
                       doNotify = True

                    self._period = period
                    self._onTime = onTime
                    self._startThold = startThold
                    self._state = state
                    self._stopThold = stopThold

                    if doNotify and self._confCallBack is not None:
                        self._confCallBack()

                elif data[0] == 'STATUS' and len(data) == 5:
                    #print(f"Got status: {line}")
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

        print("Stopping thread")

