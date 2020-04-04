
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

from control_gui import ControlGui
from ambu_control import AmbuControl

if sys.platform == 'linux':
    ambu = AmbuControl("/dev/ttyACM0")
else:
    ambu = AmbuControl("COM3")

appTop = QApplication(sys.argv)

guiTop = ControlGui(ambu=ambu)
guiTop.setWindowTitle("PABV Control")
guiTop.show()

appTop.exec_()

ambu.stop()



