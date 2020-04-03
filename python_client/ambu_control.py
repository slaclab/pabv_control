
import serial
import time
import threading


class AmbuControl(object):

    def __init__(self, dev):

        self._ser = serial.Serial(dev ,9600, timeout=1.0)

        self._data = {'time': [],
                      'chan0': [],
                      'chan1': [],
                      'chan2': [],
                      'chan3': [],
                      'chan4': []}

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
                data = line.rstrip().split(' ')
                ts = time.time()

                if data[0] == 'PERIOD':
                    print(f"Got period feedback: {line}")

                if data[0] == 'ANALOG':
                    #print(f"Got line {line}")

                    count = int(data[1])
                    volts = [float(data[i+2]) * (5.0 / 1023.0) for i in range(4)]
                    press = data[6];

                    if self._file is not None:
                        self._file.write(f'{ts} {count} {volts[0]} {volts[1]} {volts[2]} {volts[3]} {press} \n')

                    if self._last is None:
                        self._last = count
                    elif self._last != count:
                        self._last = count

                        self._callBack(self._data,count)

                        self._data = {'time': [],
                                      'chan0': [],
                                      'chan1': [],
                                      'chan2': [],
                                      'chan3': [],
                                      'chan4': []}

                    self._data['time'].append(ts)
                    self._data['chan0'].append(volts[0])
                    self._data['chan1'].append(volts[1])
                    self._data['chan2'].append(volts[2])
                    self._data['chan3'].append(volts[3])
                    self._data['chan4'].append(press)
            except:
                pass

        print("Stopping thread")

