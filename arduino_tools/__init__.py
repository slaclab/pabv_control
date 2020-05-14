from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import tarfile
import os
import platform
import subprocess
import glob
import sys

class cli:
    def __init__(self,board):
        self.board=board
        avr=board.split(":")
        self.avr="%s:%s"%(avr[0],avr[1])
        self.__exe__= {
            "Windows-64bit": ".exe",
            "Windows-32bit": ".exe",
            "Linux-64bit" : ""
        }
        self.__arduino_url__= "https://downloads.arduino.cc/arduino-cli"
        self.__images__={
            "Windows-64bit": "arduino-cli_latest_Windows_64bit.zip",
            "Windows-32bit": "arduino-cli_latest_Windows_32bit.zip",
            "Linux-64bit" : "arduino-cli_latest_Linux_64bit.tar.gz"
        }
        self.target=platform.system()+"-"+platform.architecture()[0]

    def call(self,command):
        cmd=[ 'bin/arduino-cli'+self.__exe__[self.target] ,'--config-file','etc/arduino-cli.yaml' ]
        cmd=cmd+command.split(" ")
        process = subprocess.run(cmd,
                                   stdout=sys.stdout, stderr=sys.stderr)

    def install(self):
        destdir="bin"
        datadir="data"
        try:
            os.mkdir(destdir)
            os.mkdir(datadir)
        except:pass
        filename=self.__images__[self.target]
        r = urlopen(self.__arduino_url__+"/"+self.__images__[self.target])
        data=BytesIO(r.read())
        if(filename.endswith(".tar.gz") or filename.endswith(".tgz")):
            tar = tarfile.open(fileobj=data,mode="r")
            content=tar.extractall(path=destdir)
        elif (filename.endswith(".zip")):
            zipfile = ZipFile(data)
            zipfile.extractall(path=destdir)
        self.call("cache clean")
        self.call("core update-index")
        self.call("core install "+self.avr)

    def compile(self):
        label = subprocess.check_output(["git", "describe","--tags"]).strip()
        dirs=glob.glob("arduino/ambu*")
        for d in dirs:
            bn=os.path.basename(d)
            cmd="compile -b %s --libraries arduino/libraries -o %s/%s.hex %s" % (self.board,d,bn,d)
            self.call(cmd)
    def upload(self,com,sketch):
        hex="arduino/%s/%s.hex" % (sketch,sketch)
        dir="arduino/%s" % sketch
        cmd="upload -b %s -p %s -i %s %s" %(self.board,com,hex,dir)
        self.call(cmd)
    def list(self):
        self.call("board list")
