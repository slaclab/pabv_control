#!/usr/bin/env python
from  arduino_tools import cli
c=cli()
targets=[
("ambu_control_flow_cal", "arduino:avr:uno" ),
("ambu_control_superior", "arduino:avr:uno" ),
("nano_control_superior", "arduino:samd:nano_33_iot"   ),
("AmBuArduinoGUI", "arduino:mbed:nano33ble" ),
("MasterSim", "arduino:mbed:nano33ble" ),
("test_base64", "arduino:mbed:nano33ble" ),
]

for t in targets:
    c.compile(t[0],t[1])

