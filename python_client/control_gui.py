
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *
import queue

import numpy as np
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg


import ambu_control
import time
import traceback


import time

git_version="unknown"

try:
    import version
    git_version=version.version
except:
    pass


class PowerSwitch(QPushButton):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.setCheckable(True)
        self.setMinimumWidth(100)
        self.setMinimumHeight(40)

    def paintEvent(self, event):
        label = "ON" if self.isChecked() else "OFF"
        bg_color = Qt.green if self.isChecked() else Qt.red

        radius = 14
        width = 50
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

class ModeSwitch(QPushButton):
    def __init__(self, parent = None):
        super().__init__(parent)
        self.setCheckable(True)
        self.setMinimumWidth(100)
        self.setMinimumHeight(40)

    def paintEvent(self, event):
        label = "Press" if self.isChecked() else "Vol"
        bg_color = Qt.magenta if self.isChecked() else Qt.cyan

        radius = 14
        width = 50
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

    updateCount       = pyqtSignal(str)
    updateRate        = pyqtSignal(str)
    updateTime        = pyqtSignal(str)
    updateOnTime      = pyqtSignal(str)
    updateIeRatio     = pyqtSignal(str)

    updateRespRate    = pyqtSignal(str)
    updateInhTime     = pyqtSignal(str)
    updatePipMax      = pyqtSignal(str)
    updatePipOffset   = pyqtSignal(str)
    updateVolMax      = pyqtSignal(str)
    updateVolFactor   = pyqtSignal(str)
    updateVolInhThold = pyqtSignal(str)
    updatePeepMin     = pyqtSignal(str)
    updateState       = pyqtSignal(int)
    updateStateSwitch = pyqtSignal(bool)
    updateMode        = pyqtSignal(bool)

    updateVersion     = pyqtSignal(str)
    updateArTime      = pyqtSignal(str)
    updateCycVolMax   = pyqtSignal(str)
    updateCycPipMax   = pyqtSignal(str)

    updateAlarmPipMax  = pyqtSignal(str)
    updateAlarmVolLow  = pyqtSignal(str)
    updateAlarm12V     = pyqtSignal(str)
    updateWarn9V       = pyqtSignal(str)
    updateAlarmPresLow = pyqtSignal(str)
    updateWarnPeepMin  = pyqtSignal(str)
    updateWarnVolLow   = pyqtSignal(str)
    updateWarnVolMax   = pyqtSignal(str)

    updateSerial       = pyqtSignal(str)
    updateCom          = pyqtSignal(str)

    def __init__(self, *, ambu, refPlot=False, parent=None):
        super(ControlGui, self).__init__(parent)
        self.refPlot = refPlot
        self.setWindowTitle("SLAC Accute Shortage Ventilator")

        self.ambu = ambu
        self.ambu.setConfigCallBack(self.configUpdated)
        self._queue=queue.Queue(1)
        self.ambu.setQueue(self._queue)
        self.blink_time   = time.time()

        top = QVBoxLayout()
        self.setLayout(top)
        self.alarmStatus = QLabel()
        self.blinkStat=True
        self.alarmStatus.setStyleSheet("""QLabel { background-color: lime; color: black }""")
        self.alarmStatus.setText("")
        self.alarmStatus.setAlignment(Qt.AlignCenter);
        font = self.alarmStatus.font();
        font.setPointSize(24);
        font.setBold(True);
        self.alarmStatus.setFont(font)
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tab3 = QWidget()
        self.tab4 = QWidget()
        self.tabs.addTab(self.tab1,"AMBU Control")
        self.tabs.addTab(self.tab2,"AMBU Expert")
        self.tabs.addTab(self.tab3,"Setup and Test")
        self.tabs.addTab(self.tab4,"About")
        self.tabs.setTabPosition(QTabWidget.East)
        self.setupPlots()
        self.setupPageOne()
        self.setupPageTwo()
        self.setupPageThree()
        self.setupPageFour()
        top.addWidget(self.alarmStatus)
        top.addWidget(self.tabs)
        self.last_update=time.time()
        QTimer.singleShot(100,self.updateAll)
    def setupPlots(self):
        self.gl1=pg.GraphicsLayoutWidget(border=pg.mkPen(color=None,width=2))
        self.gl1.setBackground("w")
        self.gl1.setFrameStyle(QFrame.Box)
        self.gl2=pg.GraphicsLayoutWidget(border=pg.mkPen(color=None,width=2))
        self.gl2.setBackground("w")
        self.gl2.setFrameStyle(QFrame.Box)
        self.plot1=[None]*3
        self.plot1[0]=self.gl1.addPlot(row=1,col=1)
        self.plot1[1]=self.gl1.addPlot(row=2,col=1)
        self.plot1[2]=self.gl1.addPlot(row=3,col=1)
        self.plot1[0].setLabel('bottom',"Time",color='black')
        self.plot1[1].setLabel('bottom',"Time",color='black')
        self.plot1[2].setLabel('bottom',"Time",color='black')
        self.plot2=[None]*3
        self.plot2[0]=self.gl2.addPlot(row=1,col=1)
        self.plot2[1]=self.gl2.addPlot(row=2,col=1)
        self.plot2[2]=self.gl2.addPlot(row=3,col=1)
        self.plot2[0].setLabel('bottom',"Time",color='black')
        self.plot2[1].setLabel('bottom',"Time",color='black')
        self.plot2[2].setLabel('bottom',"Time",color='black')
        self.legend1=[None]*3
        self.legend2=[None]*3
        for i in range(3):
            self.legend1[i]=self.plot1[i].addLegend(offset=(15,0),brush=pg.mkBrush((0,0,0,0)))
            self.legend1[i].anchor((0,0), (0,0))
        for i in range(3):
            self.legend2[i]=self.plot2[i].addLegend(offset=(15,0),brush=pg.mkBrush((0,0,0,0)))
            self.legend2[i].anchor((0,0), (0,0))
        if self.refPlot:
            self.plot1[0].setLabel('left',"Ref Flow SL/Min",color='black')
        else:
            self.plot1[0].setLabel('left',"Press cmH20",color='black')

        self.plot1[1].setLabel('left',"Flow L/Min",color='black')
        self.plot1[2].setLabel('left',"Volume mL",color='black')
        if self.refPlot:
            self.plot2[0].setLabel('left',"Ref Flow SL/Min",color='black')
        else:
            self.plot2[0].setLabel('left',"Press cmH20",color='black')

        self.plot2[1].setLabel('left',"Flow L/Min",color='black')
        self.plot2[2].setLabel('left',"Volume mL",color='black')
        for p in self.plot1:
            p.setXRange(-60,0)
            p.setAutoVisible(x=False,y=False)
            p.enableAutoRange('x',False)
            p.enableAutoRange('y',False)
            p.setMouseEnabled(x=False,y=False)
            p.setMenuEnabled(False)
            p.hideButtons()
        for p in self.plot2:
            p.setXRange(-60,0)
            p.setAutoVisible(x=False,y=False)
            p.enableAutoRange('x',False)
            p.enableAutoRange('y',False)
            p.setMouseEnabled(x=False,y=False)
            p.setMenuEnabled(False)
            p.hideButtons()

        self.curve1=[None]*7
        self.curve2=[None]*7
        width=3
        self.curve1[0]=self.plot1[0].plot(pen=pg.mkPen("m",width=width),name="Pressure")
        self.curve1[1]=self.plot1[0].plot(pen=pg.mkPen("r",width=width),name="PMax")
        self.curve1[2]=self.plot1[0].plot(pen=pg.mkPen("g",width=width),name="Trg Thresh")
        self.curve1[3]=self.plot1[0].plot(pen=pg.mkPen("r",width=width),name="PEEP min")
        self.curve1[4]=self.plot1[1].plot(pen=pg.mkPen("g",width=width),name="Flow")
        self.curve1[5]=self.plot1[2].plot(pen=pg.mkPen("b",width=width),name="Volume")
        self.curve1[6]=self.plot1[2].plot(pen=pg.mkPen("r",width=width),name="VMax")
        self.curve2[0]=self.plot2[0].plot(pen=pg.mkPen("m",width=width),name="Pressure")
        self.curve2[1]=self.plot2[0].plot(pen=pg.mkPen("r",width=width),name="PMax")
        self.curve2[2]=self.plot2[0].plot(pen=pg.mkPen("g",width=width),name="Trg Thresh")
        self.curve2[3]=self.plot2[0].plot(pen=pg.mkPen("r",width=width),name="PEEP min")
        self.curve2[4]=self.plot2[1].plot(pen=pg.mkPen("g",width=width),name="Flow")
        self.curve2[5]=self.plot2[2].plot(pen=pg.mkPen("b",width=width),name="Volume")
        self.curve2[6]=self.plot2[2].plot(pen=pg.mkPen("r",width=width),name="VMax")


    def setupPageOne(self):

        top = QHBoxLayout()
        self.tab1.setLayout(top)

        left = QVBoxLayout()
        top.addLayout(left)

        # Plot on right
        top.addWidget(self.gl1)
        # Controls on left
        gb = QGroupBox('Control')
        gb.setFixedWidth(320)
        left.addWidget(gb)

        fl = QFormLayout()
        fl.setFieldGrowthPolicy(QFormLayout.AllNonFixedFieldsGrow)
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        self.respRate = QLineEdit()
        self.respRate.returnPressed.connect(self.setRespRate)
        self.updateRespRate.connect(self.respRate.setText)
        fl.addRow('RR (Breaths/Min):',self.respRate)

        self.inhTime = QLineEdit()
        self.inhTime.returnPressed.connect(self.setInhTime)
        self.updateInhTime.connect(self.inhTime.setText)
        fl.addRow('Inhalation Time (sec):',self.inhTime)

        self.volInhThold = QLineEdit()
        self.volInhThold.returnPressed.connect(self.setVolInhThold)
        self.updateVolInhThold.connect(self.volInhThold.setText)
        fl.addRow('Trg. Thresh. (cmH20):',self.volInhThold)

        self.pipMaxA = QLineEdit()
        self.pipMaxA.returnPressed.connect(self.setPipMaxA)
        self.updatePipMax.connect(self.pipMaxA.setText)
        fl.addRow('PMax (cmH20):',self.pipMaxA)

        self.volMaxA = QLineEdit()
        self.volMaxA.returnPressed.connect(self.setVolMaxA)
        self.updateVolMax.connect(self.volMaxA.setText)
        fl.addRow('VMax (mL):',self.volMaxA)

        self.peepMinA = QLineEdit()
        self.peepMinA.returnPressed.connect(self.setPeepMinA)
        self.updatePeepMin.connect(self.peepMinA.setText)
        fl.addRow('PEEP Min (cmH20):',self.peepMinA)

        self.modeControlA = ModeSwitch()
        self.modeControlA.clicked.connect(self.setMode)
        self.updateMode.connect(self.modeControlA.setChecked)
        fl.addRow('Volume - Pressure:',self.modeControlA)

        self.runControlA = PowerSwitch()
        self.runControlA.clicked.connect(self.setRunState)
        self.updateStateSwitch.connect(self.runControlA.setChecked)
        fl.addRow('Run Enable:',self.runControlA)

        muteAlarm = QPushButton("Mute Alarm")
        muteAlarm.pressed.connect(self.muteAlarm)
        fl.addRow('',muteAlarm)

        # Status
        gb = QGroupBox('Status')
        gb.setFixedWidth(300)
        left.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        #this will be a switch that will display true and turn red if any of the alarm conditions are met.  Hovering or looking at expert page will say which.  maybe even alarms settings page? or just alarm settings group box on expert page?
        #fl.addRow('Alarm Status:',self.alarmStatus)

        cycVolMax = QLineEdit()
        cycVolMax.setText("0")
        cycVolMax.setReadOnly(True)
        self.updateCycVolMax.connect(cycVolMax.setText)
        fl.addRow('Max Volume (mL):',cycVolMax)

        cycPipMax = QLineEdit()
        cycPipMax.setText("0")
        cycPipMax.setReadOnly(True)
        self.updateCycPipMax.connect(cycPipMax.setText)
        fl.addRow('Max Pip (cmH20):',cycPipMax)

        cycleRunTime=QLineEdit()
        cycleRunTime.setText("0")
        cycleRunTime.setReadOnly(True)
        self.updateOnTime.connect(cycleRunTime.setText)
        fl.addRow('Cycle run time (s):',cycleRunTime)
        cycles = QLineEdit()
        cycles.setText("0")
        cycles.setReadOnly(True)
        self.updateCount.connect(cycles.setText)
        fl.addRow('Breaths:',cycles)

        ieRatio=QLineEdit()
        ieRatio.setText("0")
        ieRatio.setReadOnly(True)
        self.updateIeRatio.connect(ieRatio.setText)
        fl.addRow('IE Ratio:',ieRatio)


    def setupPageTwo(self):

        top = QHBoxLayout()
        self.tab2.setLayout(top)

        left = QVBoxLayout()
        top.addLayout(left)

        right = QVBoxLayout()
        top.addLayout(right)

        # Period Control
        gb = QGroupBox('Control Parameters')
        left.addWidget(gb)

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

        #self.pipOffset = QLineEdit()  #Issue111 - no longer used
        #self.pipOffset.returnPressed.connect(self.setPipOffset)
        #self.updatePipOffset.connect(self.pipOffset.setText)
        #fl.addRow('PIP Offset (cmH20):',self.pipOffset)

        self.volFactor = QLineEdit()
        self.volFactor.returnPressed.connect(self.setVolFactor)
        self.updateVolFactor.connect(self.volFactor.setText)
        fl.addRow('Vol Factor:',self.volFactor)
        gb = QGroupBox('Vertical Limits for Time Plots')
        left.addWidget(gb)

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

        # moved from front page
        # Status
        gb = QGroupBox('Alarms')
        right.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        alarmPipMax = QLineEdit()
        alarmPipMax.setText("0")
        alarmPipMax.setReadOnly(True)
        self.updateAlarmPipMax.connect(alarmPipMax.setText)
        fl.addRow('Press High Alarm:',alarmPipMax)

        alarmVolLow = QLineEdit()
        alarmVolLow.setText("0")
        alarmVolLow.setReadOnly(True)
        self.updateAlarmVolLow.connect(alarmVolLow.setText)
        fl.addRow('Vol Low Alarm:',alarmVolLow)

        alarmPresLow = QLineEdit()
        alarmPresLow.setText("0")
        alarmPresLow.setReadOnly(True)
        self.updateAlarmPresLow.connect(alarmPresLow.setText)
        fl.addRow('Press Low Alarm:',alarmPresLow)


        alarm12V = QLineEdit()
        alarm12V.setText("0")
        alarm12V.setReadOnly(True)
        self.updateAlarm12V.connect(alarm12V.setText)
        fl.addRow('12V Alarm:',alarm12V)

        gb = QGroupBox('Warnings')
        right.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        warnPeepMin = QLineEdit()
        warnPeepMin.setText("0")
        warnPeepMin.setReadOnly(True)
        self.updateWarnPeepMin.connect(warnPeepMin.setText)
        fl.addRow('PEEP Min Warning:',warnPeepMin)

        warnVolLow  = QLineEdit()
        warnVolLow.setText("0")
        warnVolLow.setReadOnly(True)
        self.updateWarnVolLow.connect(warnVolLow.setText)
        fl.addRow('Vol Low Warning:',warnVolLow)

        warnVolMax  = QLineEdit()
        warnVolMax.setText("0")
        warnVolMax.setReadOnly(True)
        self.updateWarnVolMax.connect(warnVolMax.setText)
        fl.addRow('Vol Max Warning:',warnVolMax)

        warn9V = QLineEdit()
        warn9V.setText("0")
        warn9V.setReadOnly(True)
        self.updateWarn9V.connect(warn9V.setText)
        fl.addRow('9V Warn:',warn9V)

        gb = QGroupBox('Status')
        right.addWidget(gb)

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

        cycVolMax = QLineEdit()
        cycVolMax.setText("0")
        cycVolMax.setReadOnly(True)
        self.updateCycVolMax.connect(cycVolMax.setText)
        fl.addRow('Max Volume (mL):',cycVolMax)

        cycPipMax = QLineEdit()
        cycPipMax.setText("0")
        cycPipMax.setReadOnly(True)
        self.updateCycPipMax.connect(cycPipMax.setText)
        fl.addRow('Max PIP (cmH20):',cycPipMax)

        # Log File
        gb = QGroupBox('Log File')
        left.addWidget(gb)

        vl = QVBoxLayout()
        gb.setLayout(vl)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        vl.addLayout(fl)



        self.logFile = QLineEdit()
        self.logFile.setReadOnly(True);
        pb = QPushButton('Select output File')
        pb.clicked.connect(self.selectFile)
        vl.addWidget(pb)
        self.selectLog=pb
        fl.addRow('Log File:',self.logFile)

        pb = QPushButton('Begin Recording Data')
        pb.clicked.connect(self.openPressed)
        self.beginLog=pb
        vl.addWidget(pb)

        pb = QPushButton('Stop Recording Data')
        pb.clicked.connect(self.closePressed)
        self.endLog=pb
        vl.addWidget(pb)

        self.plotData = []
        self.rTime = time.time()
        self.beginLog.setEnabled(False)
        self.endLog.setEnabled(False)

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
        gb = QGroupBox('Relay Control')
        left.addWidget(gb)

        right.addSpacing(20)

        #f1 defines layout for group box gb
        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        self.pipMaxB = QLineEdit()
        self.pipMaxB.returnPressed.connect(self.setPipMaxB)
        self.updatePipMax.connect(self.pipMaxB.setText)
        fl.addRow('PMax (cmH20):',self.pipMaxB)

        self.volMaxB = QLineEdit()
        self.volMaxB.returnPressed.connect(self.setVolMaxB)
        self.updateVolMax.connect(self.volMaxB.setText)
        fl.addRow('VMax (mL):',self.volMaxB)

        self.peepMinB = QLineEdit()
        self.peepMinB.returnPressed.connect(self.setPeepMinB)
        self.updatePeepMin.connect(self.peepMinB.setText)
        fl.addRow('PEEP Min (cmH20):',self.peepMinB)

        self.modeControlB = ModeSwitch()
        self.modeControlB.clicked.connect(self.setMode)
        self.updateMode.connect(self.modeControlB.setChecked)
        fl.addRow('Volume - Pressure:',self.modeControlB)

        self.runControlB = PowerSwitch()
        self.runControlB.clicked.connect(self.setRunState)
        fl.addRow('Run Enable:',self.runControlB)

        gb = QGroupBox('Calibration Instructions')
        left.addWidget(gb)
        gb.setMinimumWidth(450)
        gb.setMaximumHeight(500)

        vbox = QVBoxLayout()

        #Text field and control buttons for instructions

        self.alarmsText= {
            "alarmPipMax" : "Alarm: Peak Patient Inspiratory pressure exceeded. Check PIP Valve!",
            "alarmVolLow" :  "Alarm: Low Tidal Volume. Check Ventilator, patient circuit!",
            "alarm12V" : "Alarm: Electrical power lost!",
            "alarmPresLow" : "Alarm: Patient Inspiratory Pressure low, Check patient circuit!",
            "warn9V" : "Warning: Battery low. Replace soon!",
            "warnPeepMin" : "Warning: Pressure below PEEP. Check patient circuit!",
            "warnVolLow"  : "Warning: Low tidal Volume.!",
            "warnVolMax"  : "Warning: High tidal Volume.!"
        }
        self.alarmsActive= list()
        self.alarmCount=0

        self.instructions = []
        #0
        self.instructions.append("Click 'Next' to begin calibration procedure")

        #1:paddle up
        self.instructions.append("The ASV should be off and the plunger up. Please check that the patient circuit is connected and a test lung in place. Make sure the compressed air supply is connected and on. On the control tab set RR to 20 and Inhalation time to 1.0.")

        #2: paddle up
        self.instructions.append("Press the plunger down by hand. The pressure, flow, and volume plots should indicate the bag compression.")

        #3 Settings
        self.instructions.append("3) Set the mode to Pressure, set PMax to 40, and set PEEP Min to 10.")

        #4: paddle cycling
        self.instructions.append("The ASV is now cycling. Check that the plunger pushes the AMBU bag down smoothly, about ½ second,  before the plunger comes up. Adjust the air supply valve as needed. Adjust the exhaust valve so paddle rises smoothly")

        #5: paddle up for 5s
        self.instructions.append("Leak Check. After the plunger goes down, observe the pressure plot. The pressure should decline slowly, taking at least 10 seconds to reach 0. Faster indicates a leak in the patient circuit.")

        #6: paddle up
        self.instructions.append( "PIP Check: Check that the  PIP valve has a marked cap indicating it has been modified for higher pressure. Set PIP to maximum (clockwise) position")

        #7: run 10 cycles
        self.instructions.append("Check on pressure vs time display that peak pressure is not more than about 40 cm H2O. If higher, PIP Valve is not working properly. Replace.")

        #8: paddle up
        self.instructions.append("Set PIP valve back to 30 cm H2O. Use the provided calibration curve.  Set PMax parameter to 20 cm H2O")

        #9: Cycling
        self.instructions.append("The ASV is running. Check the pressure plot to see that the paddle cycle stops when PIP is reached, and that an alarm is generated.")

        #10: Paddle up
        self.instructions.append("Reset the Pmax parameter to 30, matching the valve. Set mode = Volume. Set VMax to 300")

        #11: Cycling
        self.instructions.append("The ASV is cycling. The volume should home in on VMax.")

        #12: Run off
        self.instructions.append("The ASV should be ready for use")

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

        resetbutton = QPushButton('Restart')
        resetbutton.clicked.connect(self.resetPressed)

        buttongroup.addWidget(prevbutton)
        buttongroup.addWidget(resetbutton)
        buttongroup.addWidget(nextbutton)

        vbox.addLayout(buttongroup)



        #add results groupbox
        gb_results = QGroupBox('Results')
        #left.addWidget(gb_results)
        right.addWidget(self.gl2)
        #left.addSpacing(300)
        gb_results.setMinimumWidth(450)
        gb_results.setMinimumHeight(300)

        results_hbox = QVBoxLayout()
        gb_results.setLayout(results_hbox)

        #results_hbox.addLayout(nameofthing)
        self.doInit=True
        self.line=[None]*7
    def setupPageFour(self):
        top = QHBoxLayout()
        self.tab4.setLayout(top)

        left = QVBoxLayout()
        top.addLayout(left)

        right = QVBoxLayout()
        top.addLayout(right)
        gb = QGroupBox('Status')
        gb.setFixedWidth(800)
        left.addWidget(gb)

        fl = QFormLayout()
        fl.setRowWrapPolicy(QFormLayout.DontWrapRows)
        fl.setFormAlignment(Qt.AlignHCenter | Qt.AlignTop)
        fl.setLabelAlignment(Qt.AlignRight)
        gb.setLayout(fl)

        sampRate = QLineEdit()
        sampRate.setText("0")
        sampRate.setReadOnly(True)
        self.updateRate.connect(sampRate.setText)
        fl.addRow('Sample Rate:',sampRate)

        timeSinceStart=QLineEdit()
        timeSinceStart.setText("0")
        timeSinceStart.setReadOnly(True)
        self.updateTime.connect(timeSinceStart.setText)
        fl.addRow('GUI uptime (s):',timeSinceStart)

        arduinoUptime=QLineEdit()
        arduinoUptime.setText("0")
        arduinoUptime.setReadOnly(True)
        self.updateArTime.connect(arduinoUptime.setText)
        fl.addRow('Device uptime (s):',arduinoUptime)

        guiVersion = QLineEdit()
        guiVersion.setText(git_version)
        guiVersion.setReadOnly(True)
        fl.addRow('GUI version:',guiVersion)

        deviceVersion = QLineEdit()
        deviceVersion.setText("")
        deviceVersion.setReadOnly(True)
        self.updateVersion.connect(deviceVersion.setText)
        fl.addRow('Control SW version:',deviceVersion)

        deviceSerial = QLineEdit()
        deviceSerial.setText("")
        deviceSerial.setReadOnly(True)
        self.updateSerial.connect(deviceSerial.setText)
        fl.addRow('Device Serial Nr:',deviceSerial)

        comPort = QLineEdit()
        comPort.setText("")
        comPort.setReadOnly(True)
        self.updateCom.connect(comPort.setText)
        fl.addRow('COM port',comPort)


    def performAction(self):
        try:
            if self.index == 1 or self.index == 2 or self.index == 6 or self.index == 8  or self.index == 10 or self.index == 12:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Plunger up")
                self.stateControl.setCurrentIndex(0)

            if self.index == 4:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Plunger cycling")
                self.stateControl.setCurrentIndex(3)

            if self.index == 5:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Plunger up for 5 seconds, then Plunger down")
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

            if self.index == 7:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Running 10 cycles.")
                self.stateControl.setCurrentIndex(3)
                sleeptime=30
                sleeptimer=0
                self.timeoutabort=0
                while sleeptimer<sleeptime:
                    time.sleep(0.1)
                    QCoreApplication.processEvents()
                    if self.timeoutabort>0:
                        break
                    sleeptimer=sleeptimer+0.1
                if self.timeoutabort==0:
                    self.stateControl.setCurrentIndex(0)

            if self.index == 9 or self.index ==11:
                self.textfield.setText(str(self.index)+") "+self.textfield.toPlainText()+"\n\nState: Plunger cycling.")
                self.stateControl.setCurrentIndex(3)

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
    def resetPressed(self):
        try:
            self.timeoutabort=1
            self.index=1
            self.textfield.setText(self.instructions[self.index])
            self.performAction()
        except Exception as e:
            print(f"Got GUI value error {e}")

    @pyqtSlot()
    def setRespRate(self):
        try:
            self.ambu.respRate = float(self.respRate.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setInhTime(self):
        try:
            self.ambu.inhTime = float(self.inhTime.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setVolInhThold(self):
        try:
            self.ambu.volInThold = float(self.volInhThold.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setPipMaxA(self):
        try:
            self.ambu.pipMax = float(self.pipMaxA.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setPipMaxB(self):
        try:
            self.ambu.pipMax = float(self.pipMaxB.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setPipOffset(self):
        try:
            self.ambu.pipOffset = float(self.pipOffset.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setPeepMinA(self):
        try:
            self.ambu.peepMin = float(self.peepMinA.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setPeepMinB(self):
        try:
            self.ambu.peepMin = float(self.peepMinB.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setVolMaxA(self):
        try:
            self.ambu.volMax = float(self.volMaxA.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setVolMaxB(self):
        try:
            self.ambu.volMax = float(self.volMaxB.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot()
    def setVolFactor(self):
        try:
            self.ambu.volFactor = float(self.volFactor.text())
        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot(int)
    def setState(self,value):
        try:
            self.ambu.runState = value

            if value == 3:
                self.runControlA.setChecked(True)
                self.runControlB.setChecked(True)
            else:
                self.runControlA.setChecked(False)
                self.runControlB.setChecked(False)

        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot(bool)
    def setMode(self,st):
        try:

            if st:
                self.ambu.runMode = 1
            else:
                self.ambu.runMode = 0

        except Exception as e:
            #print(f"Got GUI value error {e}")
            pass

    @pyqtSlot(bool)
    def setRunState(self,st):
        pass
        if st and self.stateControl.currentIndex() != 3:
            self.stateControl.setCurrentIndex(3)
        elif self.stateControl.currentIndex() > 2:
            self.stateControl.setCurrentIndex(2)

    @pyqtSlot()
    def muteAlarm(self):
        self.ambu.muteAlarm()

    @pyqtSlot()
    def openPressed(self):
        f = self.logFile.text()
        self.ambu.openLog(f)
        self.beginLog.setEnabled(False)
        self.endLog.setEnabled(True)
        self.selectLog.setEnabled(False)

    @pyqtSlot()
    def closePressed(self):
        self.ambu.closeLog()
        self.endLog.setEnabled(False)
        self.beginLog.setEnabled(True)
        self.selectLog.setEnabled(True)

    @pyqtSlot()
    def selectFile(self):
        dlg = QFileDialog()
        f=dlg.getSaveFileName(self, 'Save ventillator data:')[0]
        if(len(f)>0):
            state=not self.beginLog.isEnabled() and not self.endLog.isEnabled()
            if(state):
                self.beginLog.setEnabled(True)
            self.logFile.setText(f)
            self.logFile.update()


    def configUpdated(self):
        self.updateRespRate.emit("{:0.1f}".format(self.ambu.respRate))
        self.updateInhTime.emit("{:0.1f}".format(self.ambu.inhTime))
        self.updateVolInhThold.emit("{:0.1f}".format(self.ambu.volInThold))
        self.updatePipMax.emit("{:0.1f}".format(self.ambu.pipMax))
        self.updateVolMax.emit("{:0.1f}".format(self.ambu.volMax))
        self.updatePipOffset.emit("{:0.1f}".format(self.ambu.pipOffset))
        self.updateVolFactor.emit("{:0.1f}".format(self.ambu.volFactor))
        self.updatePeepMin.emit("{:0.1f}".format(self.ambu.peepMin))

        self.updateState.emit(self.ambu.runState)
        self.updateMode.emit(self.ambu.runMode==1)

        self.updateStateSwitch.emit(self.ambu.runState == 3)

    def setAlarm(self,tag,cond):
        if(cond):
            if(tag not in self.alarmsActive):
                self.alarmsActive.append(tag)
        else:
            if(tag in self.alarmsActive):
                self.alarmsActive.remove(tag)

    def calculateIERatio(self, ie_float):
        if ie_float < 1.0:
            # Then we want to display 1:1.1 or wiatever
            return '1 : %.1f'%(1.0/ie_float)
        else: #ie_float<1.0
            # then we want to display "2:1 or whatever"
            return '%.1f : 1'%(ie_float)

    def updateDisplay(self,count,rate,stime,artime,volMax,pipMax,ieRatio,onTime):
        self.updateCount.emit(str(count))
        self.updateRate.emit(f"{rate:.1f}")
        self.updateTime.emit(f"{stime:.1f}")
        self.updateArTime.emit(f"{artime:.1f}")
        self.updateCycVolMax.emit(f"{volMax:.1f}")
        self.updateCycPipMax.emit(f"{pipMax:.1f}")
        self.updateVersion.emit(str(self.ambu.version))
        self.updateSerial.emit(self.ambu.cpuId)
        self.updateCom.emit(self.ambu.com)
        self.updateOnTime.emit(f"{onTime:.1f}")
        self.updateIeRatio.emit(str(self.calculateIERatio(ieRatio)))

    def updateAlarms(self):
        self.updateAlarmPipMax.emit("{}".format(self.ambu.alarmPipMax))
        self.updateAlarmVolLow.emit("{}".format(self.ambu.alarmVolLow))
        self.updateAlarm12V.emit("{}".format(self.ambu.alarm12V))
        self.updateWarn9V.emit("{}".format(self.ambu.warn9V))
        self.updateAlarmPresLow.emit("{}".format(self.ambu.alarmPresLow))
        self.updateWarnPeepMin.emit("{}".format(self.ambu.warnPeepMin))
        self.updateWarnVolLow.emit("{}".format(self.ambu.warnVolLow))
        self.updateWarnVolMax.emit("{}".format(self.ambu.warnVolMax))

        ts=time.time()
        dt=ts-self.blink_time
        if(dt > 1):
            self.blink_time=ts
            self.setAlarm("alarmPipMax",self.ambu.alarmPipMax)
            self.setAlarm("alarmVolLow",self.ambu.alarmVolLow)
            self.setAlarm("alarm12V",self.ambu.alarm12V)
            self.setAlarm("alarmPresLow",self.ambu.alarmPresLow)
            self.setAlarm("warn9V",self.ambu.warn9V)
            self.setAlarm("warnPeepMin",self.ambu.warnPeepMin)
            self.setAlarm("warnVolLow",self.ambu.warnVolLow)
            self.setAlarm("warnVolMax",self.ambu.warnVolMax)
            nAlarms=len(self.alarmsActive)
            if(nAlarms==0):
                self.alarmStatus.setStyleSheet("""QLabel { background-color: lime; color: black }""")
                self.alarmStatus.setText("")
                self.alarmCount=0
            else:
                if(self.alarmCount>=nAlarms): self.alarmCount=0
                text=self.alarmsText[self.alarmsActive[self.alarmCount]]
                self.alarmStatus.setText(text)
                if(text.startswith("Alarm")):
                    label=self.alarmStatus
                    if(self.blinkStat):
                        self.alarmStatus.setStyleSheet("""QLabel { background-color: red; color: black }""")
                        self.blinkStat=False
                    else:
                        self.alarmStatus.setStyleSheet("""QLabel { background-color: rgba(0, 0, 0, 0); color: black  }""")
                        self.blinkStat=True
                else:
                    label=self.alarmStatus
                    if(self.blinkStat):
                        self.alarmStatus.setStyleSheet("""QLabel { background-color: #FFC200; color: black }""")
                        self.blinkStat=False
                    else:
                        self.alarmStatus.setStyleSheet("""QLabel { background-color: rgba(0, 0, 0, 0); color: black  }""")
                        self.blinkStat=True
            if(self.blinkStat):
                self.alarmCount=self.alarmCount+1

    def updatePlot(self,inData):
        ambu_data = inData.get_data()
        if type(ambu_data) == type(None):
            return
        xa =  ambu_data[0,:]
        xa=xa-xa[-1]
        try:
            index=self.tabs.currentIndex()
            pMin=float(self.pMinValue.text())
            pMax=float(self.pMaxValue.text())
            fMin=float(self.fMinValue.text())
            fMax=float(self.fMaxValue.text())
            vMin=float(self.vMinValue.text())
            vMax=float(self.vMaxValue.text())
            if(index==0):
                self.plot1[0].setYRange(pMin,pMax)
                self.plot1[1].setYRange(fMin,fMax)
                self.plot1[2].setYRange(vMin,vMax)
                self.plot1[0].getAxis('left').setTickSpacing(10, 5)
                self.plot1[1].getAxis('left').setTickSpacing(25, 10)
                self.plot1[2].getAxis('left').setTickSpacing(200, 50)
            else:
                self.plot2[0].setYRange(pMin,pMax)
                self.plot2[1].setYRange(fMin,fMax)
                self.plot2[2].setYRange(vMin,vMax)
                self.plot2[0].getAxis('left').setTickSpacing(10, 5)
                self.plot2[1].getAxis('left').setTickSpacing(25, 10)
                self.plot2[2].getAxis('left').setTickSpacing(200, 50)

            data=[None]*7
            data[0]=ambu_data[2,:]
            data[1]=ambu_data[6,:]
            data[2]=ambu_data[5,:]
            data[3]=ambu_data[8,:]
            data[4]=ambu_data[3,:]
            data[5]=ambu_data[4,:]
            data[6]=ambu_data[7,:]

            for i in range(7):
                if(index==0):
                    self.curve1[i].setData(xa,data[i])
                else:
                    self.curve2[i].setData(xa,data[i])
        except Exception as e:
            #print(e)
            pass

    def updateAll(self):
        ts=time.time()
        rate=100
        try:
            (data, count, rate, stime, artime, volMax, pipMax, ieRatio, onTime) = self._queue.get(block=False)
            self.updateDisplay(count,rate,stime,artime,volMax,pipMax,ieRatio,onTime)
            self.updatePlot(data)
            self.updateAlarms()
        except:
            pass
        dt=(time.time()-self.last_update)*1000
        corr=dt-rate
        if(corr>=(rate/2) or dt>=rate): corr=0
        self.last_update=time.time()
        QTimer.singleShot(rate-corr, self.updateAll)
