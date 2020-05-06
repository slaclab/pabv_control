import sys
import serial.tools.list_ports
import subprocess
import json
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

vendor_list=[
  (0x2341,0x0043), # Original UNO R3
  (0x0403,0x6001)
]
timeout=5000
sketch="ambu_control_dual"





state_labels= {
"START" : "Starting Software update",
"WAIT": "Connect USB cable to unit",
"CONNECTED": "Device found",
"PROGRAM": "Programming device",
"DONE": "Programming done",
"FAILED" :  "Programming failed"
}


    

class MainWindow(QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setWindowTitle("Arduino Updater")
        self.label = QLabel()
        self.state="START"
        self.update()
        self.state="WAIT"
        self.label.setAlignment(Qt.AlignCenter)
        self.setCentralWidget(self.label)
        self.timer = QTimer(self, interval=timeout)
        self.timer.start(timeout)
        self.timer.timeout.connect(self.process) 
        self.device=None
     

    def arduino_upload(self,port,board,sketch):
        process = subprocess.Popen(['tools/bin/arduino-cli',
                                    '--config-file','etc/arduino-cli.yaml',
                                    'upload',
                                    '-p',port,
                                    '-b',board,
                                    '-i',sketch+'/'+sketch+'.hex',
                                    sketch
                                ],
                                   stdout=subprocess.PIPE, 
                                   stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
        return(process.returncode==0)

    def update(self):
        self.label.setText(state_labels[self.state])
        font=self.label.font()
        font.setPointSize(72)
        self.label.setFont(font)

    def arduino_board_list(self):
        result=None
        try:
            process = subprocess.Popen(['tools/bin/arduino-cli',
                                        '--config-file','etc/arduino-cli.yaml',
                                        'board','list','--format','json'],
                                       stdout=subprocess.PIPE, 
                                       stderr=subprocess.PIPE)
            stdout, stderr = process.communicate()
            data = json.loads(stdout)    
            for d in data:
                if("boards" in d):
                    result=dict()
                    result['board']=d["boards"][0]['FQBN']
                    result["port"]=d['address']
                    break
        except: pass
        return result

    def process(self):        
        if(self.state=="WAIT"):
            for port in serial.tools.list_ports.comports():                   
                if( (port.vid,port.pid) in vendor_list): 
                    self.device=self.arduino_board_list()
                    if(self.device): self.state="CONNECTED"
                    break
        elif(self.state=="CONNECTED"):
            self.state="PROGRAM"
        elif(self.state=="PROGRAM"):
            try:
                success=self.arduino_upload(self.device["port"],self.device["board"],sketch)
                if(success): self.state="DONE" 
                else: self.state="fAILED"
                self.state="DONE"
            except: self.state="FAILED"
        elif(self.state=="DONE"):
            pass
        self.update()

app = QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec_()
