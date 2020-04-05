
import serial
import time
import threading


def convertArduinoAdcToVolts(val):
    return float(val) * (5.0 / 1023.0)


def convertNpa700B02WD(val):
    if val >= 2**(14-1):
        val -= 2**14

    press = float(val) * ( 50.62101 / float((2**14)-1))

    return press


Conversion = [ convertArduinoAdcToVolts,
               convertArduinoAdcToVolts,
               convertArduinoAdcToVolts,
               convertArduinoAdcToVolts,
               convertNpa700B02WD ]


class AmbuControl(object):

    def __init__(self, dev):

        self._ser = serial.Serial(dev ,9600, timeout=1.0)

        self._data = {'time': [], 'data': [[], [], [], [], []]}

        self._runEn = True
        self._callBack = self._debugCallBack
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()
        self._file = None
        self._last = None

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

            if data[0] == 'ANALOG' and len(data) == 7:
                count = int(data[1])

                values = [Conversion[i](int(data[i+2])) for i in range(5)]

                if self._file is not None:
                    self._file.write(f'{ts}, {count}, ' + ', '.join(map(str,values)))

                if self._last is None:
                    self._last = count
                elif self._last != count:
                    self._last = count

                    try:
                        self._callBack(self._data,count)
                    except Exception as e:
                        print("Got error {}".format(e))

                    self._data = {'time': [], 'data': [[], [], [], [], []]}

                self._data['time'].append(ts)
                for i in range(5):
                    self._data['data'][i].append(values[i])

        print("Stopping thread")

