
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import control_gui
import ambu_control

convert = [None]*6
convert[4] = ambu_control.convertDlcL20dD4
convert[5] = ambu_control.convertNpa700B02WDFlow

if sys.platform == 'linux':
    ambu = ambu_control.AmbuControl("/dev/ttyACM0",convert=convert)
else:
    ambu = ambu_control.AmbuControl("COM3",convert=convert)

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.setWindowTitle("PABV Control")
guiTop.show()

appTop.exec_()

ambu.stop()

