#!/usr/bin/python
import sys
import time
import serial
import pyprind
import argparse
import serial.tools.list_ports

from colorama import Fore, Back, Style, init
init(autoreset=True)

parser = argparse.ArgumentParser(usage=None, description='wmsdk ramdump tool', prog="ramdump.py")
parser.add_argument("-p", "--port", help="serial port", default="")
parser.add_argument("-b", "--baudrate", type=int, help="serial baudrate", default=0)
parser.add_argument("-o", "--output", help="output ramdump file", default="")

if __name__ == '__main__':
    parser_args = parser.parse_args()
    
    if not parser_args.output.strip():
        print(Fore.RED + "invalid params")
        exit(1)

    if not parser_args.port.strip():
        ports_list = list(serial.tools.list_ports.comports())
        if len(ports_list) <= 0:
            print(Fore.RED + "no available serial port found")
            exit(1)
        else:
            for comport in ports_list:
                print("select serial %s" % list(comport)[1])
                port = list(comport)[0]
                break
    else:
        port = parser_args.port.strip()

    print('connecting serial...')

    if parser_args.baudrate:
        baudrate = parser_args.baudrate
    else:
        baudrate = 115200

    ser = serial.Serial()
    ser.port = port
    ser.baudrate = baudrate
    ser.timeout = 1
    ser.dtr = 0
    ser.rts = 0
    ser.open()

    print("serial connected")

    print("start save ramdump")

    try_cnt = 0
    total_len = 0x48 + 0x48000 #general r0-r15, psr, pc, ram 0x48000

    while True:

        ramdump_content = bytearray()
        save_bar = pyprind.ProgBar(total_len, track_time = False)

        ser.write("#*#*\n".encode('utf-8'))
        ser.write("ramdump\n".encode('utf-8'))

        while True:
            _sdata = ser.read(1) or None
            if None == _sdata:
                break
            ramdump_content.extend(_sdata)
            save_bar.update()

        if total_len == len(ramdump_content):
            ramdump_file = open(parser_args.output.strip(), 'wb+')
            if ramdump_file:
                ramdump_file.write(ramdump_content)
                ramdump_file.close()
                print("ramdump file saved")
            else:
                print(Fore.RED + "write ramdump file failed")
            break;
        else:
            print("error, try again...")
            try_cnt = try_cnt + 1
            if try_cnt > 10:
                print(Fore.RED + "save ramdump file failed")
                break;
            else:
                time.sleep(3)

    ser.flush()
    ser.flushInput()
    ser.close()