#!/usr/bin/env python3 
from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import tarfile
import os
import platform

target=platform.system()+"-"+platform.machine()

arduino_url= "https://downloads.arduino.cc/arduino-cli"
images={ 
    "win64": "arduino-cli_latest_Windows_64bit.zip",
    "win32": "arduino-cli_latest_Windows_64bit.zip",
    "Linux-x86_64" : "arduino-cli_latest_Linux_64bit.tar.gz"
}

destdir="tools/bin"

try:
    os.mkdir("tools")
except:
    pass

filename=images[target]
r = urlopen(arduino_url+"/"+images[target])
data=BytesIO(r.read())
if(filename.endswith(".tar.gz") or filename.endwith(".tgz")):
    tar = tarfile.open(fileobj=data,mode="r")
    content=tar.extractall(path=destdir)    
elif (filename.endswith(".zip")):
    zipfile = ZipFile(data)
    zipfile.extractall(path=destdir)


