
import serial
import time
import threading
import math
import sys
import traceback

def convertArduinoHaf(val):

    flow = 50.0 * (((float(val) / 16384.0) - 0.1) / 0.8)

    return flow


def convertArduinoAdcToVolts(val):
    return float(val) * (5.0 / 1023.0)


def convertNpa700B02WD(val):
    press = float(val-8192) * ( 5.07492 / 8191.0 )

    return press


def convertNpa700B02WDFlow(val):
    #return convertNpa700B02WD(val) * 12.0

    press = float(val-8192) * ( 2.0 / 8191.0 )

    B = 51.7

    if press < 0:
        sign = -1
        press = abs(press)
    else:
        sign = 1

    return sign * B * math.sqrt(abs(press))


def convertSp110Sm02Flow(val):
    press = float(val-32768) * ( 2.0 / (0.9 * 32768.0) )

    B = 62.0

    if press < 0:
        sign = -1
        press = abs(press)
    else:
        sign = 1

    return sign * B * math.sqrt(abs(press))


def convertDlcL20dD4(val):
    press = -1.25 * ((float(val) - (0.5 * float(2**24))) / (0.5 * float(2**24))) * 20.0 * 2.54
    return press


def convertDlcL20dD4Reverse(press):
    adc = ((press / (-1.25 * 20.0 * 2.54)) * (0.5 * float(2**24))) + (0.5 * float(2**24))
    return int(adc)


def convertRaw(val):
    return float(val)


def convertZero(val):
    return 0.0


class AmbuControl(object):

    def __init__(self, dev, convert=[], adjust=[]):

        self._ser = serial.Serial(dev, 57600, timeout=1.0)

        self._convert = convert
        self._runEn = True
        self._callBack = self._debugCallBack
        self._file = None
        self._last = None
        self._adjust = adjust
        self._period = 0
        self._onTime = 0
        self._startThold = 0
        self._state = 0
        self._first = False

        if len(convert) != len(adjust):
            raise Exception("Convert and adjust mismatch")
            sys.exit(1)

        self._initData()

        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()

        while not self._first and self._runEn:
            print("Waiting for first message...")
            time.sleep(1.0)

        if self._first:
            print("Got first message!")

    def _initData(self):
        self._data = {'time': [], 'data': [], 'raw': []}

        for cf in self._convert:
            if cf is not None:
                self._data['data'].append([])
                self._data['raw'].append([])

    def openLog(self, fName):
        self._file = open(fName,'a')

    def closeLog(self):
        self._file.close()
        self._file = None

    def _debugCallBack(self,data,count):
        l = len(self._data['time'])
        print(f"Got data. Len={l} count={count}")

    def setCallBack(self, callBack):
        self._callBack = callBack

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
    def state(self):
        return self._state

    @state.setter
    def state(self,value):
        self._state = value
        self._setConfig()

    def clearCount(self):
        self._ser.write(f"CNTRST\n".encode('UTF-8'))

    def stop(self):
        self._runEn = False
        self._thread.join()

    def _setConfig(self):
        self._ser.write(f"CONFIG {self._period} {self._onTime} {self._startThold} {self._state}\n".encode('UTF-8'))

    def _handleSerial(self):
        print("Starting thread")

        while self._runEn:
            try:
                raw = self._ser.readline()
                line = raw.decode('UTF-8')

                if not self._first and len(line) > 2:
                    print(f"Got: {line}")

                data = line.rstrip().split(' ')
                ts = time.time()

                if data[0] == 'PERIOD':
                    print(f"Got period feedback: {line}")

                if data[0] == 'STATUS' and len(data) >= (len(self._convert)+6):
                    #print(f"Got status: {line}")
                    count = int(data[1],0)
                    self._period = int(data[2],0)
                    self._onTime = int(data[3],0)
                    self._startThold = int(data[4],0)
                    self._state = int(data[5],0)

                    convValues = []
                    rawValues = []
                    volume = 0

                    for i,cf in enumerate(self._convert):
                        if cf is not None:
                            val = int(data[i+6],0)
                            convValues.append(cf(val + self._adjust[i]))
                            rawValues.append(val)

                    if self._file is not None:
                        self._file.write(f'{ts}, {count}, ' + ', '.join(map(str,convValues)))
                        self._file.write('\n')

                    if self._last is None:
                        self._last = count
                    elif self._last != count:
                        self._last = count

                        try:
                            self._callBack(self._data,count)
                            avgs = [int(sum(lst) / len(lst)) for lst in self._data['raw']]

                            #print(f"Got status: {line}")
                            print('Averages: ' + ' '.join(map(str,avgs)))
                        except Exception as e:
                            traceback.print_exc()
                            print("Got error {}".format(e))

                        self._initData()

                    self._data['time'].append(ts)

                    for i,dat in enumerate(zip(convValues,rawValues)):
                        self._data['data'][i].append(dat[0])
                        self._data['raw'][i].append(dat[1])

                    self._first = True

            except Exception as e:
                traceback.print_exc()
                print(f"Got error {e}")

        print("Stopping thread")

