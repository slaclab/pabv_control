
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import control_gui
import ambu_control

convert = [None]*2
convert[0] = ambu_control.convertDlcL20dD4
#convert[1] = ambu_control.convertNpa700B02WDFlow
convert[1] = ambu_control.convertSp110Sm02Flow

# Marty with dual = 8090
# Marty with cal = 8099

adjust = [0] *2
adjust[1] = (8192-8105)
#adjust[1] = (8192-7616)
#adjust[1] = (8192-8022)
#adjust[1] = (8192-7612)

if sys.platform == 'linux':
    ambu = ambu_control.AmbuControl("/dev/ttyACM0",convert=convert,adjust=adjust)
else:
    ambu = ambu_control.AmbuControl("COM4",convert=convert,adjust=adjust)

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.setWindowTitle("PABV Control")
guiTop.show()

appTop.exec_()

ambu.stop()

