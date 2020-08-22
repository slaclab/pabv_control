import subprocess
import os
tag = subprocess.check_output(["git", "describe","--tags"]).strip()
_status=subprocess.check_output(["git", "status","--porcelain"]).strip()

if(len(_status)!=0): _status="-dirty"
else: _status=""
tag=str("%s%s"%(repr(tag)[2:-1],_status))

_filename=os.path.dirname(__file__)+"/version.py"
_f=open(_filename,"w")
print("version='%s'" %tag,file=_f  )
_f.close()
version=tag.split('-')[0].split('.')
major=int(version[0].replace('v',''))
minor=int(version[1])
build=int(version[2])
_filename=os.path.dirname(__file__)+"/config.nsh"
_f=open(_filename,"w")
print("!define VERSIONMAJOR %u" %major,file=_f  )
print("!define VERSIONMINOR %u" %minor,file=_f  )
print("!define VERSIONBUILD %u" %build,file=_f  )
print('Outfile dist\SLAC_ASV_Client_Installer_%s.exe'  %tag,file=_f  )
_f.close()
