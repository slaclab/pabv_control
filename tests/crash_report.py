import time
print("PABV Version/Crash Report Tool --START--")
print(time.asctime())

import sys
print(sys.platform)
print(sys.version)

try:
    import numpy
    print("NUMPY: ", numpy.__version__)
except:
    print("NUMPY: Could not import")

try:
    from PyQt5.QtCore import QT_VERSION_STR
    print("PyQt: ", QT_VERSION_STR)
except:
    print("PyQt: could not import")

print("")
import os
try:
    conda_list = os.popen("conda list")
    conda_modules = conda_list.readlines()
    for i in range(3):
        print(conda_modules[i].rstrip())
    interisting_modules = ["pyqt","numpy","pyserial","python"]
    for i in interisting_modules:
        n = len(i)
        for j in range(3,len(conda_modules)):
            if conda_modules[j][:n] == i:
                print(conda_modules[j].rstrip())
except:
    print("CONDA packages not found?")

print("")
try:
    import serial
    import serial.tools.list_ports
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        port_no=port.device
        description=port.description
        vid=port.vid
        pid=port.pid
    print("SERIAL:  port_no, description, VID, PID")
    print("SERIAL: ",port_no, " ", description, " ", vid, " ", pid)
except:
    print("SERIAL: Could not find any serial ports")

print("PABV Version/Crash Report Tool --END--")