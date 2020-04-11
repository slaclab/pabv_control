
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import numpy as np
import matplotlib.pyplot as plt

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure

import time

class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = [fig.add_subplot(211), fig.add_subplot(212)]
        super(MplCanvas, self).__init__(fig)


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

        # Period Control
        gb = QGroupBox('GUI Control')
        hl.addWidget(gb)

        vl = QVBoxLayout()
        gb.setLayout(vl)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        vl.addLayout(fl)

        self.pMinValue = QLineEdit()
        self.pMinValue.setText("-10")
        fl.addRow('Pres Min Value:',self.pMinValue)

        self.pMaxValue = QLineEdit()
        self.pMaxValue.setText("50")
        fl.addRow('Pres Max Value:',self.pMaxValue)

        self.fMinValue = QLineEdit()
        self.fMinValue.setText("-10")
        fl.addRow('Flow Min Value:',self.fMinValue)

        self.fMaxValue = QLineEdit()
        self.fMaxValue.setText("50")
        fl.addRow('Flow Max Value:',self.fMaxValue)

        self.plotCycles = QLineEdit()
        self.plotCycles.setText("10")
        fl.addRow('Plot Cycles:',self.plotCycles)

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

        self.plotData = []
        self.rTime = time.time()


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

    def plotData(self,inData,count):
        self.plotData.append(inData)
        vals = len(inData['data'])

        pc = int(self.plotCycles.text())

        if len(self.plotData) > pc:
            self.plotData = self.plotData[-1 * pc:]

        self.updateCount.emit(str(count))

        xAxis = []
        data = []

        for _ in range(vals):
            data.append([])

        for pd in self.plotData:
            xAxis.extend([val - self.rTime for val in pd['time']])

            for i,d in enumerate(data):
                d.extend(pd['data'][i])

        self.plot.axes[0].cla()
        self.plot.axes[1].cla()
        xa = np.array(xAxis)

        for i,d in enumerate(data):
            if i < len(self.plot.axes):
                self.plot.axes[i].plot(xa,np.array(d))

        self.plot.axes[0].set_ylim([float(self.pMinValue.text()),float(self.pMaxValue.text())])
        self.plot.axes[1].set_ylim([float(self.fMinValue.text()),float(self.fMaxValue.text())])

        self.plot.axes[0].set_xlabel('Time')
        self.plot.axes[0].set_ylabel('cmH20')

        self.plot.axes[1].set_xlabel('Time')
        self.plot.axes[1].set_ylabel('cmH20')

        self.plot.draw()


