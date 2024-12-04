#!/usr/bin/python
import os
import sys
import time
import signal
import struct
import serial
import pyprind
import platform
import argparse
from xmodem import XMODEM1k
import serial.tools.list_ports

from colorama import Fore, Back, Style, init
init(autoreset=True)

#timeout 1ms
default_timeout  = 0.001
default_baudrate = 115200

sync_threshold   = 5

#baudrate, rom_cmd
#baudrate_list = [[2000000, '210a00ef2a3100000080841e00'],
#                 [1000000, '210a005e3d3100000040420f00'],
#                 [921600,  '210a005d503100000000100e00'],
#                 [460800,  '210a0007003100000000080700'],
#                 [115200,  '210a00974b3100000000c20100']]
baudrate_list = [[2000000, b'\x21\x0a\x00\xef\x2a\x31\x00\x00\x00\x80\x84\x1e\x00'],
                 [1000000, b'\x21\x0a\x00\x5e\x3d\x31\x00\x00\x00\x40\x42\x0f\x00'],
                 [921600,  b'\x21\x0a\x00\x5d\x50\x31\x00\x00\x00\x00\x10\x0e\x00'],
                 [460800,  b'\x21\x0a\x00\x07\x00\x31\x00\x00\x00\x00\x08\x07\x00'],
                 [115200,  b'\x21\x0a\x00\x97\x4b\x31\x00\x00\x00\x00\xc2\x01\x00']]

#erase_cmd  = '210a00c335320000000200fe01'
erase_cmd  = b'\x21\x0a\x00\xc3\x35\x32\x00\x00\x00\x02\x00\xfe\x01'
#reboot_cmd = '210600c77c3f000000'
reboot_cmd = b'\x21\x06\x00\xc7\x7c\x3f\x00\x00\x00'

parser = argparse.ArgumentParser(usage=None, description='wmsdk flash tool', prog="flash.py")
parser.add_argument("-p", "--port", help="serial port", default="")
parser.add_argument("-b", "--baudrate", type=int, help="serial baudrate", default=0)
parser.add_argument("-i", "--image", action='append', nargs='+', help="image file and path")
parser.add_argument("-n", "--name", action='append', nargs='+', help="the name of the firmware to be burned, ex: app,bootloader,partition_table,custom...")
parser.add_argument("-e", "--erase", help="erase device flash", action="store_true", default=False)
parser.add_argument("-l", "--list", help="list serial port", action="store_true", default=False)
parser.add_argument("-m", "--manual-reset", help="reset manually after burning", action="store_true", default=False)

class xdownload(object):

    def __init__(self, port="", baud=default_baudrate, timeout=default_timeout):
        self.ser = serial.Serial()
        self.ser.port = port
        self.ser.baudrate = baud
        self.ser.timeout = timeout
        self.ser.dtr = 0
        self.ser.rts = 0
        self.ser.open()

    def set_image(self, image):
        statinfo_bin = os.stat(image)
        self.bar_user = pyprind.ProgBar(statinfo_bin.st_size / 1024, track_time = False)

    def set_port_baudrate(self, baud):
        self.ser.baudrate = baud
    
    def set_timeout(self, timeout):
        self.ser.timeout  = timeout

    def getc(self, size, timeout=1):
        return self.ser.read(size) or None

    def putc(self, data, timeout=1):
        return self.ser.write(data)

    def putc_bar(self, data, timeout=1):
        self.bar_user.update()
        return self.ser.write(data)

    def is_open(self):
        return self.ser.is_open

    def open(self):
        self.ser.open()
    
    def close(self):
        self.ser.flush()
        self.ser.flushInput()
        self.ser.close()

    def reset_device(self):
        self.ser.dtr = 0
        self.ser.rts = 1
        time.sleep(0.05)
        self.ser.dtr = 1
        self.ser.rts = 0
        time.sleep(0.05)
        self.ser.dtr = 0

    def send_esc(self, ms):
        delay = 10
        for x in range(int(ms / delay)):
            self.ser.write(struct.pack('<B', 27))
            time.sleep(delay / 1000)

def use_baudrate(download, baudrate):
    for i in range(len(baudrate_list)):
        if baudrate_list[i][0] == baudrate:
            #download.putc(bytes.fromhex(baudrate_list[i][1]))
            download.putc(baudrate_list[i][1])
            #download.close()
            time.sleep(0.02)
            download.set_port_baudrate(baudrate)
            #download.open()
            time.sleep(0.02)
            break

def try_baudrate(download):
    for i in range(len(baudrate_list)):
        print("trying baudrate %d..." % baudrate_list[i][0])

        #if download.is_open():
        #    download.close()
        #    time.sleep(0.02)

        #download.set_port_baudrate(baudrate_list[i][0])
        try:
            #download.open()
            download.set_port_baudrate(baudrate_list[i][0])
        except IOError:
            continue
        else:
            time.sleep(0.02)

            #download.close()
            #time.sleep(0.02)

            download.set_port_baudrate(default_baudrate)

            #download.open()
            time.sleep(0.02)

            #download.putc(bytes.fromhex(baudrate_list[i][1]))
            download.putc(baudrate_list[i][1])
            time.sleep(0.02)

            #download.close()
            #time.sleep(0.02)

            download.set_port_baudrate(baudrate_list[i][0])
            #download.open()
            time.sleep(0.02)

            # check baudrate
            #c = download.getc(1)
            #if c == b'C':
            #    break
            #else:
            #    print("serial can't use baudrate %d" % baudrate_list[i][0])
            break

def try_reset_device(download):
    cnt = 0
    while True:
        #reset device by rts pin
        download.reset_device()
        download.send_esc(20)

        #reset device by atcmd
        download.putc(bytes("AT+RST\r\n", encoding='ascii'))
        download.putc("AT+RST\r\n".encode('utf-8'))
        download.send_esc(20)

        #reset device by cli
        #download.putc(bytes("reboot\r\n", encoding='ascii'))
        download.putc("reboot\r\n".encode('utf-8'))
        download.send_esc(50)

        cnt = cnt + 1
        if cnt >= 10:
            break

def sync_device(download):
#    print("wait serial sync...")
    cnt = 0
    note = 1
    time_start = time.time()
    while True:
        c = download.getc(1)
        if c == None:
            download.send_esc(30)
        else:
            if c == b'C':
                cnt = cnt + 1
            else:
                cnt = 0
        if cnt >= sync_threshold:
            break

        time_end = time.time()
        timeuse = time_end - time_start
        if timeuse >= 2 and note:
            print(Fore.CYAN + "please reset device")
            note = 0
        elif timeuse > 60:
            print(Fore.YELLOW + "serial sync timeout")
            download.close()
            print(Fore.RED + "flash device failed")
            exit(2)

#    print("serial sync sucess")

def empty_read(download):
    #empty serial read cache
    while True:
        c = download.getc(1)
        if c == None:
            break

def erase_flash(download):
    print("erase device flash...")
    #erase 2M flash cmd
    #download.putc(bytes.fromhex(erase_cmd))
    download.putc(erase_cmd)
    empty_read(download)
    cnt = 0
    time_start = time.time()
    while True:
        c = download.getc(1)
        if c != None:
            if c == b'C':
                cnt = cnt + 1
            else:
                cnt = 0
                download.close()
                print(Fore.RED + "erase device failed: " + str(c))
                exit(4)
        if cnt >= sync_threshold:
            break
        time_end = time.time()
        timeuse = time_end - time_start
        if timeuse > 60:
            print(Fore.YELLOW + "erase device timeout")
            download.close()
            print(Fore.RED + "erase device failed")
            exit(3)
    print("erase device complete")

def is_need_burned(names, image):
    if not names:
        return True;
    need_burned = False
    for i in range(len(names)):
        name = names[i][0]
        image_name = os.path.basename(image).split('.')[0]
        if name == image_name:
            need_burned = True
            break
    return need_burned

def exit_flash(signum, frame):
    print(Fore.CYAN + "\n\nterminate flash\n")
    sys.exit(0)

if __name__ == '__main__':
    parser_args, unknown_args = parser.parse_known_args()

    signal.signal(signal.SIGINT, exit_flash)
    signal.signal(signal.SIGTERM, exit_flash)

    if parser_args.list:
        ports_list = list(serial.tools.list_ports.comports())
        for comport in ports_list:
            print("%s" % list(comport)[1])
        exit(0)

    if not parser_args.port.strip():
        ports_list = list(serial.tools.list_ports.comports())
        if len(ports_list) <= 0:
            print(Fore.YELLOW + "no available serial port found")
            print(Fore.RED + "flash device failed")
            exit(1)
        else:
            for comport in ports_list:
                print(Fore.CYAN + "select serial %s" % list(comport)[1])
                port = list(comport)[0]
                break
    else:
        port = parser_args.port.strip()

    print('connecting serial...')

    try:
        download = xdownload(port=port)
    except Exception:
        print(Fore.RED + "cannot open the serial port")
        exit(1)

    print("serial connected")

    print("trying reset device...")
    try_reset_device(download)

    print("wait serial sync...")
    sync_device(download)
    print("serial sync success")

    if parser_args.erase:
        erase_flash(download)
        if not parser_args.image:
            download.close()
            exit(0)

    if parser_args.baudrate:
        if parser_args.baudrate != default_baudrate:
            use_baudrate(download, parser_args.baudrate)
    else:
        try_baudrate(download)

    print("start download image...")

    result = False
    for i in range(len(parser_args.image)):
        image = parser_args.image[i][0]

        if not is_need_burned(parser_args.name, image):
            result = True
            continue

        print("download %s..." % os.path.basename(image))
        download.set_image(image)

        try:
            stream = open(image, 'rb+')
        except IOError:
            print(Fore.RED + "can't open file: %s" % image)
            download.close()
            raise
        else:
            modem = XMODEM1k(download.getc, download.putc_bar)

            empty_read(download)

            download.set_timeout(1)
            result = modem.send(stream)
            stream.close()

            if not result:
                break

            download.set_timeout(default_timeout)
            if i != (len(parser_args.image) - 1):
                sync_device(download)

    if result:
        if not parser_args.manual_reset:
            #reboot device
            #download.putc(bytes.fromhex(reboot_cmd))
            download.putc(reboot_cmd)
        print("flash device complete")
    else:
        print(Fore.RED + "flash device failed")
    download.close()