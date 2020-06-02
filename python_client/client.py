
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import control_gui
import ambu_control
ambu = ambu_control.AmbuControl()

appTop = QApplication(sys.argv)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.show()

ambu.start()
appTop.exec_()
ambu.stop()
