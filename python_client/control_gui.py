
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import numpy as np
import matplotlib.pyplot as plt

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure


class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(MplCanvas, self).__init__(fig)

        #self.axes.xlabel('Time')
        #self.axes.ylabel('Volts')

class ControlGui(QWidget):

    updateCount = pyqtSignal(str)

    def __init__(self, *, ambu, parent=None):
        super(ControlGui, self).__init__(parent)

        self.ambu = ambu
        self.ambu.setCallBack(self.plotData)

        top = QVBoxLayout()
        self.setLayout(top)

        self.plot = MplCanvas()
        top.addWidget(self.plot)

        hl = QHBoxLayout()
        top.addLayout(hl)

        # Period Control
        gb = QGroupBox('Period Control')
        hl.addWidget(gb)

        vl = QVBoxLayout()
        gb.setLayout(vl)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        vl.addLayout(fl)

        self.relayPeriod = QLineEdit()
        self.relayPeriod.setText("3000")
        fl.addRow('Relay Period:',self.relayPeriod)

        self.relayOn = QLineEdit()
        self.relayOn.setText("1000")
        fl.addRow('Relay On:',self.relayOn)

        self.cycles = QLineEdit()
        self.cycles.setText("0")
        self.cycles.setReadOnly(True)
        self.updateCount.connect(self.cycles.setText)
        fl.addRow('Cycles:',self.cycles)

        pb = QPushButton('Set Period')
        pb.clicked.connect(self.setPeriod)
        vl.addWidget(pb)

        pb = QPushButton('Clear Count')
        pb.clicked.connect(self.clrCount)
        vl.addWidget(pb)

        # Log File
        gb = QGroupBox('Log File')
        hl.addWidget(gb)

        vl = QVBoxLayout()
        gb.setLayout(vl)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        vl.addLayout(fl)

        self.logFile = QLineEdit()
        fl.addRow('Log File:',self.logFile)

        pb = QPushButton('Open File')
        pb.clicked.connect(self.openPressed)
        vl.addWidget(pb)

        pb = QPushButton('Close File')
        pb.clicked.connect(self.closePressed)
        vl.addWidget(pb)

    #@pyqtSlot
    def setPeriod(self):
        period = int(self.relayPeriod.text())
        on = int(self.relayOn.text())

        self.ambu.setPeriod(period,on)

    def clrCount(self):
        self.ambu.clearCount()

    #@pyqtSlot
    def openPressed(self):
        f = self.logFile.text()
        self.ambu.openLog(f)

    #@pyqtSlot
    def closePressed(self):
        self.ambu.closeLog()

    def plotData(self,data,count):
        self.updateCount.emit(str(count))

        sTime = data['time'][0]

        xAxis = np.array([data['time'][i] - sTime for i in range(len(data['time']))])
        data0 = np.array(data['chan0'])
        data1 = np.array(data['chan1'])
        data2 = np.array(data['chan2'])
        data3 = np.array(data['chan3'])
        data4 = np.array(data['chan4'])

        self.plot.axes.cla()
        self.plot.axes.plot(xAxis,data0,xAxis,data1,xAxis,data2,xAxis,data3,xAxis,data4)
        self.plot.draw()

