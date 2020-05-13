
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import control_gui
import ambu_control

if sys.platform == 'linux':
    ambu = ambu_control.AmbuControl("/dev/ttyACM0",convert=convert,adjust=adjust)
else:
    ambu = ambu_control.AmbuControl("COM4",convert=convert,adjust=adjust)

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu,refPlot=True)
guiTop.setWindowTitle("PABV Control")
guiTop.show()

appTop.exec_()

ambu.stop()

