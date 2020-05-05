import sys
import serial.tools.list_ports
import subprocess
import json
from PyQt5.QtWidgets import *
from PyQt5.QtCore    import *
from PyQt5.QtGui     import *

vendor_list=[
(9025,67)
]
timeout=5000
sketch="ambu_control_dual"

def arduino_upload(port,board,sketch):
    process = subprocess.Popen(['tools/bin/arduino-cli',
                                '--config-file','etc/arduino-cli.yaml',
                                'upload',
                                '-p',port,
                                '-b',board,
                                sketch
                            ],
                    stdout=subprocess.PIPE, 
                    stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    return(process.returncode==0)

def arduino_board_list():
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



state_labels= {
"WAIT": "Connect USB cable to unit",
"CONNECTED": "Device found",
"PROGRAM": "Programming device",
"DONE": "Programming done",
"FAILED" :  "Programming failed"
}


    

class MainWindow(QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setWindowTitle("Arudino Updater")
        self.label = QLabel("Starting: Update Arudino Software")
        self.label.setAlignment(Qt.AlignCenter)
        self.setCentralWidget(self.label)
        self.timer = QTimer(self, interval=timeout)
        self.timer.start(timeout)
        self.timer.timeout.connect(self.process) 
        self.device=None
        self.state="WAIT"

    def arduino_upload(self,port,board,sketch):
        process = subprocess.Popen(['tools/bin/arduino-cli',
                                    '--config-file','etc/arduino-cli.yaml',
                                    'upload',
                                    '-p',port,
                                    '-b',board,
                                    sketch
                                ],
                                   stdout=subprocess.PIPE, 
                                   stderr=subprocess.PIPE)
        stdout, stderr = process.communicate()
        return(process.returncode==0)

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
        self.label.setText(state_labels[self.state])

app = QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec_()
