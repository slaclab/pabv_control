
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import numpy as np
import matplotlib.pyplot as plt
import ambu_control

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure

import time

class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=10, height=10, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = [fig.add_subplot(311), fig.add_subplot(312), fig.add_subplot(313)]
        super(MplCanvas, self).__init__(fig)
        fig.tight_layout(pad=3.0)


class ControlGui(QWidget):

    updateCount = pyqtSignal(str)
    updateRate  = pyqtSignal(str)

    def __init__(self, *, ambu, refPlot=False, parent=None):
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
        self.relayPeriod.setText("20")
        fl.addRow('RR (Breaths/Min):',self.relayPeriod)

        self.relayOn = QLineEdit()
        self.relayOn.setText("1")
        fl.addRow('Inhalation Time (S):',self.relayOn)

        self.startThold = QLineEdit()
        self.startThold.setText("-5")
        fl.addRow('Start Thold (cmH20):',self.startThold)

        self.cycles = QLineEdit()
        self.cycles.setText("0")
        self.cycles.setReadOnly(True)
        self.updateCount.connect(self.cycles.setText)
        fl.addRow('Breaths:',self.cycles)

        self.sampRate = QLineEdit()
        self.sampRate.setText("0")
        self.sampRate.setReadOnly(True)
        self.updateRate.connect(self.sampRate.setText)
        fl.addRow('Sample Rate:',self.sampRate)

        pb = QPushButton('Update Settings')
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
        self.pMinValue.setText("-5")

        if refPlot:
            fl.addRow('Ref Flow Min Value:',self.pMinValue)
        else:
            fl.addRow('Pres Min Value:',self.pMinValue)

        self.pMaxValue = QLineEdit()
        self.pMaxValue.setText("40")

        if refPlot:
            fl.addRow('Ref Flow Max Value:',self.pMaxValue)
        else:
            fl.addRow('Pres Max Value:',self.pMaxValue)

        self.fMinValue = QLineEdit()
        self.fMinValue.setText("-5")
        fl.addRow('Flow Min Value:',self.fMinValue)

        self.fMaxValue = QLineEdit()
        self.fMaxValue.setText("250")
        fl.addRow('Flow Max Value:',self.fMaxValue)

        self.vMinValue = QLineEdit()
        self.vMinValue.setText("-5")
        fl.addRow('Vol Min Value:',self.vMinValue)

        self.vMaxValue = QLineEdit()
        self.vMaxValue.setText("500")
        fl.addRow('Vol Max Value:',self.vMaxValue)

        self.plotCycles = QLineEdit()
        self.plotCycles.setText("10")
        fl.addRow('Plot Breaths:',self.plotCycles)

        self.fOffset = QLineEdit()
        self.fOffset.setText("-33.0")
        fl.addRow('Flow Offset:',self.fOffset)

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
        rate = float(self.relayPeriod.text())
        period = (1.0 / rate) * 1000.0 * 60.0

        on = float(self.relayOn.text()) * 1000.0

        thold = ambu_control.convertDlcL20dD4Reverse(float(self.startThold.text())) / 256

        if ( thold > 0xFFFF):
            thold = 0xFFFF
        elif (thold < 0 ):
            thold = 0

        self.ambu.setPeriod(int(period),int(on), int(thold))

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

        if len(inData['data']) != 2:
            print("Invalid data value count. Exepected 2")
            return

        # Add third data = volume
        inData['data'].append([])

        off = float(self.fOffset.text())
        refT = inData['time'][0]
        intSum = 0

        for i,v in enumerate(zip(inData['time'],inData['data'][1])):
            durr = (v[0] - refT) / 60.0
            refT = v[0]

            if durr > 0:
                intSum += ((v[1]-off) * durr) * 1000.0

            inData['data'][2].append(intSum)
            inData['data'][1][i] = v[1]-off

        pc = int(self.plotCycles.text())

        if len(self.plotData) > pc:
            self.plotData = self.plotData[-1 * pc:]

        self.updateCount.emit(str(count))

        rate = len(inData['time']) / (inData['time'][-1] - inData['time'][0])
        self.updateRate.emit(f"{rate:.1f}")

        xAxis = []
        data = []

        for _ in range(3):
            data.append([])

        for pd in self.plotData:
            xAxis.extend([val - self.rTime for val in pd['time']])

            for i,d in enumerate(data):
                d.extend(pd['data'][i])

        self.plot.axes[0].cla()
        self.plot.axes[1].cla()
        self.plot.axes[2].cla()
        xa = np.array(xAxis)

        self.plot.axes[0].plot(xa,np.array(data[0]),color="yellow")
        self.plot.axes[1].plot(xa,np.array(data[1]),color="green")
        self.plot.axes[2].plot(xa,np.array(data[2]),color="blue")

        self.plot.axes[0].set_ylim([float(self.pMinValue.text()),float(self.pMaxValue.text())])
        self.plot.axes[1].set_ylim([float(self.fMinValue.text()),float(self.fMaxValue.text())])
        self.plot.axes[2].set_ylim([float(self.vMinValue.text()),float(self.vMaxValue.text())])

        self.plot.axes[0].set_xlabel('Time')

        if refPlot:
            self.plot.axes[0].set_ylabel('Ref Flow SL/Min')
        else:
            self.plot.axes[0].set_ylabel('Press cmH20')

        self.plot.axes[1].set_xlabel('Time')
        self.plot.axes[1].set_ylabel('Flow L/Min')

        self.plot.axes[2].set_xlabel('Time')
        self.plot.axes[2].set_ylabel('Volume mL')

        self.plot.draw()

# -3 offset
# Add Run/Stop

