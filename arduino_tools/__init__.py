from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import tarfile
import os
import platform
import subprocess
import glob
import sys
import json
sys.path.insert(0,"python_client")
import git_version

class cli:
    def __init__(self):
        self.__exe__= {
            "Windows-64bit": ".exe",
            "Windows-32bit": ".exe",
            "Linux-64bit" : "",
            "Darwin-64bit" : ""
        }
        self.__arduino_url__= "https://downloads.arduino.cc/arduino-cli"
        self.__images__={
            "Windows-64bit": "arduino-cli_latest_Windows_64bit.zip",
            "Windows-32bit": "arduino-cli_latest_Windows_32bit.zip",
            "Linux-64bit" : "arduino-cli_latest_Linux_64bit.tar.gz",
            "Darwin-64bit" : "arduino-cli_latest_macOS_64bit.tar.gz"
        }
        self.target=platform.system()+"-"+platform.architecture()[0]

    def call(self,command):
        cmd=[ 'bin/arduino-cli'+self.__exe__[self.target] ,'--config-file','etc/arduino-cli.yaml' ]
        cmd=cmd+command.split(" ")
        print(*cmd)
        process = subprocess.run(cmd,
                                   stdout=sys.stdout, stderr=sys.stderr)

    def install(self,platform=["arduino:samd","arduino:avr"]):
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
        for p in platform:            
            self.call("core install "+p)


    def compile(self,sketch):
        label = subprocess.check_output(["git", "describe","--tags"]).strip()
        _base='arduino/'+sketch
        _config=_base+'/'+sketch+'.json'
        _hex=_base+'/'+sketch+'.hex'
        _data=None
        with open(_config) as f:
            _data = json.load(f)
        board=_data["board"]
        cmd='compile -b %s --build-properties "compiler.cpp.extra_flags=-DGIT_VERSION=%s" --libraries arduino/libraries -o %s %s' \
            % (board,git_version.tag,_hex,_base)        
        self.call(cmd)
    def upload(self,com,sketch):
        _hex="arduino/%s/%s.hex" % (sketch,sketch)
        _config="arduino/%s/%s.json" % (sketch,sketch)
        _data=None
        with open(_config) as f:
            _data = json.load(f)
        board=_data["board"]
        _dir="arduino/%s" % sketch
        cmd="upload -b %s -p %s -i %s %s" %(board,com,_hex,_dir)
        self.call(cmd)
    def list(self):
        self.call("board list")
