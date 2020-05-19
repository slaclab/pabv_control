#!/usr/bin/env python
import sys
from  arduino_tools import cli

if(len(sys.argv)!=3): 
    print("usage:    board_upload.py COM SKETCHNAME")
    print("example:  board_upload.py COM1 ambu_control_flow_cal")
    exit(1)

c=cli()
com=sys.argv[1]
sketch=sys.argv[2]
print("Uploading sketch %s to port %s" %(sketch,com))
c.upload(com,sketch)

