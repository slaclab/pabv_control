
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

import numpy as np
import matplotlib.pyplot as plt
import ambu_control
import time

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from matplotlib.figure import Figure

import time

git_version="unknown"
try:
    import version
    git_version=version.version
except:
    pass

class MplCanvas(FigureCanvasQTAgg):

    def __init__(self, parent=None, width=10, height=10, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = [fig.add_subplot(311), fig.add_subplot(312), fig.add_subplot(313)]
        super(MplCanvas, self).__init__(fig)
        fig.tight_layout(pad=3.0)

class PowerSwitch(QPushButton):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.setCheckable(True)
        self.setMinimumWidth(66)
        self.setMinimumHeight(22)

    def paintEvent(self, event):
        label = "ON" if self.isChecked() else "OFF"
        bg_color = Qt.green if self.isChecked() else Qt.red

        radius = 10
        width = 32
        center = self.rect().center()

        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(center)
        painter.setBrush(QColor(0,0,0))

        pen = QPen(Qt.black)
        pen.setWidth(2)
        painter.setPen(pen)

        painter.drawRoundedRect(QRect(-width, -radius, 2*width, 2*radius), radius, radius)
        painter.setBrush(QBrush(bg_color))
        sw_rect = QRect(-radius, -radius, width + radius, 2*radius)
        if not self.isChecked():
            sw_rect.moveLeft(-width)
        painter.drawRoundedRect(sw_rect, radius, radius)
        painter.drawText(sw_rect, Qt.AlignCenter, label)


class ControlGui(QWidget):

    updateCount   = pyqtSignal(str)
    updateRate    = pyqtSignal(str)
    updateTime    = pyqtSignal(str)
    updateRR      = pyqtSignal(str)
    updateIT      = pyqtSignal(str)
    updateStart   = pyqtSignal(str)
    updateStop    = pyqtSignal(str)
    updateVol     = pyqtSignal(str)
    updateState   = pyqtSignal(int)
    updateVersion = pyqtSignal(str)
    updateArTime  = pyqtSignal(str)

    def __init__(self, *, ambu, refPlot=False, parent=None):
        super(ControlGui, self).__init__(parent)

        self.refPlot = refPlot
        self.setWindowTitle("SLAC Accute Shortage Ventilator")

        self.ambu = ambu
        self.ambu.setDataCallBack(self.dataUpdated)
        self.ambu.setConfCallBack(self.confUpdated)

        self.rateInput    = None
        self.inTimeInput  = None
        self.startThInput = None
        self.stopThInput  = None
        self.volThInput   = None
        self.stateControl = None
        self.runControl   = None

        top = QVBoxLayout()
        self.setLayout(top)

        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tab3 = QWidget()

        self.tabs.addTab(self.tab1,"AMBU Control")
        self.tabs.addTab(self.tab2,"AMBU Expert")
        self.tabs.addTab(self.tab3,"Calibration and Test")

        self.setupPageOne()
        self.setupPageTwo()
        self.setupPageThree()

        top.addWidget(self.tabs)

    def setupPageOne(self):

        top = QHBoxLayout()
        self.tab1.setLayout(top)

        left = QVBoxLayout()
        top.addLayout(left)

        # Plot on right
        self.plot = MplCanvas()
        top.addWidget(self.plot)

        # Controls on left
        gb = QGroupBox('Control')
        left.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        self.rateInput = QLineEdit()
        self.rateInput.returnPressed.connect(self.setRate)
        self.updateRR.connect(self.rateInput.setText)
        fl.addRow('RR (Breaths/Min):',self.rateInput)

        self.inTimeInput = QLineEdit()
        self.inTimeInput.returnPressed.connect(self.setOnTime)
        self.updateIT.connect(self.inTimeInput.setText)
        fl.addRow('Inhalation Time (S):',self.inTimeInput)

        self.startThInput = QLineEdit()
        self.startThInput.returnPressed.connect(self.setStartThold)
        self.updateStart.connect(self.startThInput.setText)
        fl.addRow('Vol Inh P (cmH20):',self.startThInput)

        self.stopThInput = QLineEdit()
        self.stopThInput.returnPressed.connect(self.setStopThold)
        self.updateStop.connect(self.stopThInput.setText)
        fl.addRow('Pip Max (cmH20):',self.stopThInput)

        self.volThInput = QLineEdit()
        self.volThInput.returnPressed.connect(self.setVolThold)
        self.updateVol.connect(self.volThInput.setText)
        fl.addRow('V Max (mL):',self.volThInput)

        self.runControl = PowerSwitch()
        self.runControl.clicked.connect(self.setRunState)
        fl.addRow('Run Enable:',self.runControl)

        # Status
        gb = QGroupBox('Status')
        left.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

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

        timeSinceStart=QLineEdit()
        timeSinceStart.setText("0")
        timeSinceStart.setReadOnly(True)
        self.updateTime.connect(timeSinceStart.setText)
        fl.addRow('Seconds since start:',timeSinceStart)
        
        arduinoUptime=QLineEdit()
        arduinoUptime.setText("0")
        arduinoUptime.setReadOnly(True)
        self.updateArTime.connect(arduinoUptime.setText)
        fl.addRow('Arduino uptime (s):',arduinoUptime)

        guiVersion = QLineEdit()
        guiVersion.setText(git_version)
        guiVersion.setReadOnly(True)
        fl.addRow('GUI version:',guiVersion)

        hwVersion = QLineEdit()
        hwVersion.setText("unknown")
        hwVersion.setReadOnly(True)
        self.updateVersion.connect(hwVersion.setText)
        fl.addRow('Control SW version:',hwVersion)



    def setupPageTwo(self):

        top = QHBoxLayout()
        self.tab2.setLayout(top)

        # Period Control
        gb = QGroupBox('GUI Control')
        top.addWidget(gb)

        vl = QVBoxLayout()
        gb.setLayout(vl)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        vl.addLayout(fl)

        self.stateControl = QComboBox()
        self.stateControl.addItem("Relay Force Off")
        self.stateControl.addItem("Relay Force On")
        self.stateControl.addItem("Relay Run Off")
        self.stateControl.addItem("Relay Run On")
        self.stateControl.setCurrentIndex(3)
        self.updateState.connect(self.stateControl.setCurrentIndex)
        self.stateControl.currentIndexChanged.connect(self.setState)
        fl.addRow('State:',self.stateControl)

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
        self.fMinValue.setText("-50")
        fl.addRow('Flow Min Value:',self.fMinValue)

        self.fMaxValue = QLineEdit()
        self.fMaxValue.setText("100")
        fl.addRow('Flow Max Value:',self.fMaxValue)

        self.vMinValue = QLineEdit()
        self.vMinValue.setText("-200")
        fl.addRow('Vol Min Value:',self.vMinValue)

        self.vMaxValue = QLineEdit()
        self.vMaxValue.setText("800")
        fl.addRow('Vol Max Value:',self.vMaxValue)

        # Log File
        gb = QGroupBox('Log File')
        top.addWidget(gb)

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

    def setupPageThree(self):
        self.timeoutabort=0
        top = QHBoxLayout()
        self.tab3.setLayout(top)
    
        left = QVBoxLayout()
        top.addLayout(left)
        
        right = QVBoxLayout()
        top.addLayout(right)
    
        # Controls and plot on right
        
        #controls group box{
        gb = QGroupBox('Relay Control (Disabled for now to avoid conflict)')
        right.addWidget(gb)

        right.addSpacing(50)
        
        #f1 defines layout for group box gb
        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)
        
        #relay control
        self.stateControl2 = QComboBox()
        self.stateControl2.addItem("Relay Force Off")
        self.stateControl2.addItem("Relay Force On")
        self.stateControl2.addItem("Relay Run Off")
        self.stateControl2.addItem("Relay Run On")
        #self.stateControl.setCurrentIndex(3)
        #self.updateState.connect(self.stateControl.setCurrentIndex)
        #self.stateControl.currentIndexChanged.connect(self.setState)
        fl.addRow('State:',self.stateControl2)
        

        #} end controls gb
    
        #using self.plot makes it disappear from other page... this is placeholder for now
        
        self.plot2 = MplCanvas()
        right.addWidget(self.plot2)
        
        
        # left
        gb = QGroupBox('Calibration Instructions')
        left.addWidget(gb)
        left.addSpacing(300)
        gb.setMinimumWidth(450)
        gb.setMaximumHeight(300)
        
        
        #f1 defines layout for group box gb
        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        #gb.setLayout(fl)
        
        vbox = QVBoxLayout()

        #Text field and control buttons for instructions
        
        self.instructions = []
        
        self.instructions.append("Click 'Next' to begin calibration procedure")
        
        self.instructions.append("The ASV should be off and the paddle up. Please check that the patient circuit is connected and a test lung in place. Make sure the compressed air supply is connected and on.")
        
        self.instructions.append("Press the paddle down by hand. The pressure, flow, and volume plots should indicate the bag compression.")
        
        self.instructions.append("The ASV is now cycling. Check that the paddle pushes the AMBU bag down smoothly before the paddle comes up. Adjust the air supply valve as needed. Adjust the exhaust valve so paddle rises smoothly.")
        
        
        self.instructions.append(" Leak Check. After the paddle goes down, observe the pressure plot. The pressure should decline slowly, taking at least 10 seconds to reach 0. Faster indicates a leak in the patient circuit.")
        
        self.instructions.append( " PIP Check: Check that the  PIP valve has marked cap indicating it has been modified for higher pressure. Set PIP for 40 cm H2O using calibration plot.")
        
        self.instructions.append(" Checking PIP.")

        self.instructions.append(" Please set PIP to 30 cm H2O.")

        self.instructions.append("Checking PIP")

        self.instructions.append(" Please set PIP to 20 cm H2O.")
            
        self.instructions.append(" Checking PIP.")
        
        self.instructions.append(" Below are the data. The values should be consistent with the calibration plot.")
            
        self.instructions.append("Please set PIP valve back to 30 cm H2O. Please set PIPmax parameter to 25 cm H2O.")
       
        self.instructions.append(" The ASV is running. Check the pressure plot to see that the paddle cycle stops when PIPmax is reached.")
        
        self.instructions.append("Calibration cycle compelete.")
        
        self.instlength = len(self.instructions)
        
        self.index=0
        
        self.textfield = QTextEdit()
        self.textfield.setReadOnly(True)

        self.textfield.setText(self.instructions[self.index])
        
        self.performAction()

        vbox.addWidget(self.textfield)
        gb.setLayout(vbox)

        buttongroup = QHBoxLayout()

        prevbutton = QPushButton('Previous')
        prevbutton.clicked.connect(self.prevPressed)

        nextbutton = QPushButton('Next')
        nextbutton.clicked.connect(self.nextPressed)

        repbutton = QPushButton('Repeat')
        #repbutton.clicked.connect(self.repPressed)
        
        buttongroup.addWidget(prevbutton)
        buttongroup.addWidget(repbutton)
        buttongroup.addWidget(nextbutton)

        vbox.addLayout(buttongroup)
    
    def performAction(self):
        try:
            if self.index == 1 or self.index == 2 or self.index == 5 or self.index == 6  or self.index == 9 or self.index == 11 or self.index == 12:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Paddle up")
                self.stateControl.setCurrentIndex(0)
    
            if self.index == 3 or self.index == 13 or self.index == 14:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Paddle cycling")
                self.stateControl.setCurrentIndex(3)
                      
            if self.index == 4:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Paddle up for 5 seconds, then Paddle down")
                self.stateControl.setCurrentIndex(0)
                sleeptime=5
                sleeptimer=0
                self.timeoutabort=0
                while sleeptimer<sleeptime:
                    time.sleep(0.1)
                    QCoreApplication.processEvents()
                    if self.timeoutabort>0:
                        break
                    sleeptimer=sleeptimer+0.1
                if self.timeoutabort==0:
                    self.stateControl.setCurrentIndex(1)
                                       
            if self.index == 6 or self.index == 8 or self.index == 10:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Running 10 cycles; calculating observed PIP from pressure plot.  Results will appear below.")
    
                                       
        except Exception as e:
            print(f"Got GUI value error {e}")
        
    @pyqtSlot()
    def nextPressed(self):
        try:
            self.timeoutabort=1
            if self.index < self.instlength-1:
                self.index=self.index+1
                self.textfield.setText(self.instructions[self.index])
                self.performAction()
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def prevPressed(self):
        try:
            self.timeoutabort=1
            if self.index > 0:
                self.index=self.index-1
                self.textfield.setText(self.instructions[self.index])
                self.performAction()
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def repPressed(self):
        try:
            self.timeoutabort=1
            self.textfield.setText("Filler text for repeat button")
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setRate(self):
        try:
            self.ambu.cycleRate = float(self.rateInput.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setOnTime(self):
        try:
            self.ambu.onTime = float(self.inTimeInput.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setStartThold(self):
        try:
            self.ambu.startThold = float(self.startThInput.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setStopThold(self):
        try:
            self.ambu.stopThold = float(self.stopThInput.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setVolThold(self):
        try:
            self.ambu.volThold = float(self.volThInput.text())
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot(int)
    def setState(self,value):
        try:
            self.ambu.state = value

            if value == 3:
                self.runControl.setChecked(True)
            else:
                self.runControl.setChecked(False)

        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot(bool)
    def setRunState(self,st):
        if st:
            self.stateControl.setCurrentIndex(3)
        elif self.stateControl.currentIndex() >= 2:
            self.stateControl.setCurrentIndex(2)

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

        if self.ambu.state == 3:
            self.runControl.setChecked(True)
        else:
            self.runControl.setChecked(False)

    def dataUpdated(self,inData,count,rate,stime,version,artime):
        self.updateCount.emit(str(count))
        self.updateRate.emit(f"{rate:.1f}")
        self.updateTime.emit(f"{stime:.1f}")
        self.updateVersion.emit(str(version))
        self.updateArTime.emit(f"{artime:.1f}")

        try:
            self.plot.axes[0].cla()
            self.plot.axes[1].cla()
            self.plot.axes[2].cla()
            #xa = np.array(inData['time'])
            ambu_data = inData.get_data()
            xa = ambu_data[0,:]

            # self._data.append([diffT, count, press, flow, vol, self.startThold, self.stopThold, self.volThold])
            #self._data['time'].append(diffT)
            #self._data['count'].append(count)
            #self._data['press'].append(press)
            #self._data['flow'].append(flow)
            #self._data['vol'].append(vol)
            #self._data['inhP'].append(self.startThold)
            #self._data['maxP'].append(self.stopThold)
            #self._data['maxV'].append(self.volThold)
            self.plot.axes[0].plot(xa, ambu_data[2,:],color="magenta",linewidth=2.0)   # press
            self.plot.axes[0].plot(xa, ambu_data[5,:],color="red",linewidth=1.0)       # p-threshold high
            self.plot.axes[0].plot(xa, ambu_data[6,:],color="red",linewidth=1.0)       # p-threshold low
            self.plot.axes[1].plot(xa, ambu_data[3,:],color="green",linewidth=2.0)     # flow
            self.plot.axes[2].plot(xa, ambu_data[4,:],color="blue",linewidth=2.0)      # volume
            self.plot.axes[2].plot(xa, ambu_data[7,:],color="red",linewidth=1.0)       # volume threshold

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
