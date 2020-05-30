#!/usr/bin/env python
import sys
import json
from  arduino_tools import cli
data=None
with open('targets.json') as f:
    data = json.load(f)
if(len(sys.argv)!=3): 
    print("usage:    board_upload.py COM SKETCHNAME")
    print("example:  board_upload.py COM1 ambu_control_flow_cal")
    exit(1)

c=cli()
com=sys.argv[1]
sketch=sys.argv[2]
board=data[sketch]
print("Uploading sketch %s to port %s" %(sketch,com))
c.upload(com,sketch,board)

