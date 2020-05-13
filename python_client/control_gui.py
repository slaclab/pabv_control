
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
    updateRR    = pyqtSignal(str)
    updateIT    = pyqtSignal(str)
    updateStart = pyqtSignal(str)
    updateStop  = pyqtSignal(str)
    updateVol   = pyqtSignal(str)
    updateState = pyqtSignal(int)

    def __init__(self, *, ambu, refPlot=False, parent=None):
        super(ControlGui, self).__init__(parent)

        self.refPlot = refPlot

        self.ambu = ambu
        self.ambu.setDataCallBack(self.plotData)
        self.ambu.setConfCallBack(self.confUpdated)

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

        self.rp = QLineEdit()
        self.rp.returnPressed.connect(self.setRate)
        self.updateRR.connect(self.rp.setText)
        fl.addRow('RR (Breaths/Min):',self.rp)

        self.ro = QLineEdit()
        self.ro.returnPressed.connect(self.setOnTime)
        self.updateIT.connect(self.ro.setText)
        fl.addRow('Inhalation Time (S):',self.ro)

        self.st = QLineEdit()
        self.st.returnPressed.connect(self.setStartThold)
        self.updateStart.connect(self.st.setText)
        fl.addRow('Start Thold (cmH20):',self.st)

        self.sp = QLineEdit()
        self.sp.returnPressed.connect(self.setStopThold)
        self.updateStop.connect(self.sp.setText)
        fl.addRow('Stop Thold (cmH20):',self.sp)

        self.vl = QLineEdit()
        self.vl.returnPressed.connect(self.setVolThold)
        self.updateVol.connect(self.vl.setText)
        fl.addRow('Volume Thold (mL):',self.vl)

        rs = QComboBox()
        rs.addItem("Relay Off")
        rs.addItem("Relay On")
        rs.addItem("Relay Cycle")
        rs.setCurrentIndex(2)
        self.updateState.connect(rs.setCurrentIndex)
        rs.currentIndexChanged.connect(self.setState)
        fl.addRow('State:',rs)

        cycles = QLineEdit()
        cycles.setText("0")
        cycles.setReadOnly(True)
        self.updateCount.connect(cycles.setText)
        fl.addRow('Breaths:',cycles)

        sampRate = QLineEdit()
        sampRate.setText("0")
        sampRate.setReadOnly(True)
        self.updateRate.connect(sampRate.setText)
        fl.addRow('Sample Rate:',sampRate)

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

        if self.refPlot:
            fl.addRow('Ref Flow Min Value:',self.pMinValue)
        else:
            fl.addRow('Pres Min Value:',self.pMinValue)

        self.pMaxValue = QLineEdit()
        self.pMaxValue.setText("40")

        if self.refPlot:
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


    @pyqtSlot()
    def setRate(self):
        try:
            self.ambu.cycleRate = float(self.rp.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setOnTime(self):
        try:
            self.ambu.onTime = float(self.ro.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setStartThold(self):
        try:
            self.ambu.startThold = float(self.st.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setStopThold(self):
        try:
            self.ambu.stopThold = float(self.sp.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setVolThold(self):
        try:
            self.ambu.volThold = float(self.vl.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot(int)
    def setState(self, value):
        print(f"set state value = {value}")
        try:
            self.ambu.state = value
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def openPressed(self):
        f = self.logFile.text()
        self.ambu.openLog(f)

    @pyqtSlot()
    def closePressed(self):
        self.ambu.closeLog()

    def confUpdated(self):
        self.updateRR.emit("{:0.1f}".format(self.ambu.cycleRate))
        self.updateIT.emit("{:0.1f}".format(self.ambu.onTime))
        self.updateStart.emit("{:0.1f}".format(self.ambu.startThold))
        self.updateStop.emit("{:0.1f}".format(self.ambu.stopThold))
        self.updateVol.emit("{:0.1f}".format(self.ambu.volThold))
        self.updateState.emit(self.ambu.state)

    def plotData(self,inData,count):
        self.plotData.append(inData)

        pc = int(self.plotCycles.text())

        if len(self.plotData) > pc:
            self.plotData = self.plotData[-1 * pc:]

        self.updateCount.emit(str(count))
        try:
            rate = len(inData['time']) / (inData['time'][-1] - inData['time'][0])
        except:
            rate=0.
        self.updateRate.emit(f"{rate:.1f}")

        xAxis = []
        press = []
        flow  = []
        vol   = []

        for pd in self.plotData:
            xAxis.extend([val - self.rTime for val in pd['time']])
            press.extend(pd['press'])
            flow.extend(pd['flow'])
            vol.extend(pd['vol'])

        try:
            self.plot.axes[0].cla()
            self.plot.axes[1].cla()
            self.plot.axes[2].cla()
            xa = np.array(xAxis)

            self.plot.axes[0].plot(xa,np.array(press),color="yellow",linewidth=2.0)
            self.plot.axes[1].plot(xa,np.array(flow),color="green",linewidth=2.0)
            self.plot.axes[2].plot(xa,np.array(vol),color="blue",linewidth=2.0)

            self.plot.axes[0].set_ylim([float(self.pMinValue.text()),float(self.pMaxValue.text())])
            self.plot.axes[1].set_ylim([float(self.fMinValue.text()),float(self.fMaxValue.text())])
            self.plot.axes[2].set_ylim([float(self.vMinValue.text()),float(self.vMaxValue.text())])

            self.plot.axes[0].set_xlabel('Time')

            if self.refPlot:
                self.plot.axes[0].set_ylabel('Ref Flow SL/Min')
            else:
                self.plot.axes[0].set_ylabel('Press cmH20')

            self.plot.axes[1].set_xlabel('Time')
            self.plot.axes[1].set_ylabel('Flow L/Min')

            self.plot.axes[2].set_xlabel('Time')
            self.plot.axes[2].set_ylabel('Volume mL')

            self.plot.draw()
        except Exception as e:
            print(f"Got plotting exception {e}")

# -3 offset
# Add Run/Stop

