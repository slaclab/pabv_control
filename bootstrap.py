#!/usr/bin/env python
install_platforms=["arduino:samd","arduino:avr","arduino:mbed"]

from  arduino_tools import cli
c=cli()
c.install(install_platforms)

