
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import control_gui
import ambu_control
#import client_version

if sys.platform == 'linux':
    ambu = ambu_control.AmbuControl("/dev/ttyACM0")
    #ambu = ambu_control.AmbuControl("/dev/ttyUSB0")
elif sys.platform == 'darwin':
    ambu = ambu_control.AmbuControl("/dev/cu.usbmodem1421")
else:
    ambu = ambu_control.AmbuControl("COM3")

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.show()

ambu.start()
appTop.exec_()
ambu.stop()
