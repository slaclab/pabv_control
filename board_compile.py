#!/usr/bin/env python
from  arduino_tools import cli
c=cli()
targets=[
"ambu_control_flow_cal",
"ambu_control_superior",
"nano_control_superior"
]

for t in targets:
    c.compile(t)

