
import sys

from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *
QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)
import control_gui
import ambu_control
ambu = ambu_control.AmbuControl()
appTop = QApplication(sys.argv)
font=appTop.font()
font.setPointSizeF(12)
appTop.setFont(font)

guiTop = control_gui.ControlGui(ambu=ambu)
guiTop.show()

ambu.start()
appTop.exec_()
ambu.stop()
