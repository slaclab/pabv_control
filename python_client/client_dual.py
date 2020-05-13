
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
else:
    ambu = ambu_control.AmbuControl("COM4")

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.setWindowTitle("PABV Control")
guiTop.show()

appTop.exec_()

ambu.stop()

