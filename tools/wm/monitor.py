#!/usr/bin/python
import sys
import signal
import serial
import pyprind
import argparse
import threading
import serial.tools.list_ports

from colorama import Fore, Back, Style, init
init(autoreset=True)

default_baudrate = 115200

parser = argparse.ArgumentParser(usage=None, description='wmsdk monitor tool', prog="monitor.py")
parser.add_argument("-p", "--port", help="serial port", default="")
parser.add_argument("-b", "--baudrate", type=int, help="serial baudrate", default=0)
parser.add_argument("-l", "--list", help="list serial port", action="store_true", default=False)

def exit_monitor(signum, frame):
    print(Fore.CYAN + "\n\nterminate monitor\n")
    sys.exit(0)

def op_thread_func(s):
    while True:
        c = sys.stdin.read(1) or None
        if c != None:
            s.write(c.encode('utf-8'))

if __name__ == '__main__':
    signal.signal(signal.SIGINT, exit_monitor)
    signal.signal(signal.SIGTERM, exit_monitor)

    parser_args, unknown_args = parser.parse_known_args()

    if parser_args.list:
        ports_list = list(serial.tools.list_ports.comports())
        for comport in ports_list:
            print("%s" % list(comport)[1])
        sys.exit(0)

    if not parser_args.port.strip():
        ports_list = list(serial.tools.list_ports.comports())
        if len(ports_list) <= 0:
            print(Fore.YELLOW + "no available serial port found")
            print(Fore.RED + "monitor device failed")
            sys.exit(0)
        else:
            for comport in ports_list:
                print("select serial %s" % list(comport)[1])
                port = list(comport)[0]
                break
    else:
        port = parser_args.port.strip()

    if parser_args.baudrate:
        baud = parser_args.baudrate
    else:
        baud = default_baudrate

    print('connecting serial...')
    s = serial.Serial()
    s.port = port
    s.baudrate = baud
    s.timeout = 0.001
    s.dtr = 0
    s.rts = 0
    s.open()
    print("serial connected")

    op_thread = threading.Thread(target=op_thread_func, kwargs={"s": s})
    op_thread.daemon = True
    op_thread.start()

    while True:
        try:
            c = s.read(1) or None
        except IOError:
            print(Fore.RED + "read serial error")
            print(Fore.CYAN + "terminate monitor...")
            break
        else:
            if c != None and c != b'\x00':
                try:
                    sys.stdout.write(c.decode())
                    sys.stdout.flush()
                except UnicodeDecodeError:
                    pass
    s.close()
