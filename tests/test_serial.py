import serial.tools.list_ports

ports = list(serial.tools.list_ports.comports())  

for port_no, description, address in ports:
#    print(port_no,description,address)
    if 'USB-Serial' in description:
        print(port_no)
