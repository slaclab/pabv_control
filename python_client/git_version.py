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
