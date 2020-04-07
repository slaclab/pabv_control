
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

        self.minValue = QLineEdit()
        self.minValue.setText("-10")
        fl.addRow('Min Value:',self.minValue)

        self.maxValue = QLineEdit()
        self.maxValue.setText("50")
        fl.addRow('Max Value:',self.maxValue)

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
        self.plotData.append(data)

        pc = int(self.plotCycles.text())

        if len(self.plotData) > pc:
            self.plotData = self.plotData[-1 * pc:]

        self.updateCount.emit(str(count))

        cnt = len(data['data'])
        xAxis = []
        data = []

        for j in range(cnt):
            data.append([])

        for i in range(len(self.plotData)):
            xAxis.extend(self.plotData[i]['time'])

            for j in range(cnt):
                data[j].extend(self.plotData[i]['data'][j])

        self.plot.axes.cla()
        xa = np.array(xAxis)

        for j in range(cnt):
            self.plot.axes.plot(xa,np.array(data[j]))

        self.plot.axes.set_ylim([float(self.minValue.text()),float(self.maxValue.text())])
        self.plot.draw()

        #self.axes.xlabel('Time')
        #self.axes.ylabel('Volts')


