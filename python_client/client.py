
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
                      'chan3': []}

        self._runEn = True
        self._callBack = self._debugCallBack
        self._thread = threading.Thread(target=self._handleSerial)
        self._thread.start()

    def _debugCallBack(self,data):
        l = len(self._data['time'])
        print(f"Got data. Len={l}")

    def setCallBack(self, callBack):
        self._callBack = callBack

    def setPeriod(self, period, on):
        self._ser.write(f"PERIOD {period} {on}\n".encode('UTF-8'))

    def stop(self):
        self._runEn = False
        self._thread.join()

    def _handleSerial(self):
        print("Starting thread")

        while self._runEn:

            line = self._ser.readline().decode('UTF-8')

            data = line.split(' ')

            if data[0] == 'PERIOD':
                print(f"Got period feedback: {line}")

            if data[0] == 'ANALOG':

                if int(data[1]) == 1:
                    self._callBack(self._data)

                    self._data = {'time': [],
                                  'chan0': [],
                                  'chan1': [],
                                  'chan2': [],
                                  'chan3': []}

                self._data['time'].append(time.time())
                self._data['chan0'].append(int(data[2]))
                self._data['chan1'].append(int(data[3]))
                self._data['chan2'].append(int(data[4]))
                self._data['chan3'].append(int(data[5]))

        print("Stopping thread")

#if __name__ == "__main__":

test = AmbuControl('/dev/ttyACM0')

