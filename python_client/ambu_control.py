
import serial
import time
import threading


def convertArduinoAdcToVolts(val):
    return float(val) * (5.0 / 1023.0)


def convertNpa700B02WD(val):
    press = float(val-8192) * ( 5.07492 / 8191.0 )

    return press


def convertNpa700B02WDFlow(val):
    return convertNpa700B02WD(val) * 12.0


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

    def __init__(self, dev, convert=[]):

        self._ser = serial.Serial(dev ,9600, timeout=1.0)

        self._convert = convert
        self._runEn = True
        self._callBack = self._debugCallBack
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()
        self._file = None
        self._last = None

        self._initData()

    def _initData(self):
        self._data = {'time': [], 'data': []}

        for cf in self._convert:
            if cf is not None:
                self._data['data'].append([])

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

    def setPeriod(self, period, on):
        self._ser.write(f"PERIOD {period} {on}\n".encode('UTF-8'))

    def clearCount(self):
        self._ser.write(f"CNTRST\n".encode('UTF-8'))

    def stop(self):
        self._runEn = False
        self._thread.join()

    def _handleSerial(self):
        print("Starting thread")

        while self._runEn:
            try:
                raw = self._ser.readline()
                line = raw.decode('UTF-8')
            except:
                line = ''

            data = line.rstrip().split(' ')
            ts = time.time()

            if data[0] == 'PERIOD':
                print(f"Got period feedback: {line}")

            if data[0] == 'ANALOG' and len(data) >= (len(self._convert)+2):
                count = int(data[1])
                values = []

                for i,cf in enumerate(self._convert):
                    if cf is not None:
                        values.append(cf(int(data[i+2],0)))

                if self._file is not None:
                    self._file.write(f'{ts}, {count}, ' + ', '.join(map(str,values)))
                    self._file.write('\n')

                if self._last is None:
                    self._last = count
                elif self._last != count:
                    self._last = count

                    try:
                        self._callBack(self._data,count)
                    except Exception as e:
                        print("Got error {}".format(e))

                    self._initData()

                self._data['time'].append(ts)

                for i,val in enumerate(values):
                    self._data['data'][i].append(val)

        print("Stopping thread")

