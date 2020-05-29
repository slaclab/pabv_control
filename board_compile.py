#!/usr/bin/env python
import json
from  arduino_tools import cli
c=cli()
data=None
with open('targets.json') as f:
    data = json.load(f)


for t in data:
    c.compile(t,data[t])

