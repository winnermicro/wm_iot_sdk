#!/usr/bin/python
import os
import sys
import time
import struct
import serial
import threading
from xmodem import XMODEM1k
import serial.tools.list_ports

_PY2 = sys.version_info[0] < 3

if _PY2:
    from Tkinter import *
    import ttk
    import tkMessageBox as messagebox
else:
    # Python 3
    from tkinter import *
    import tkinter.ttk as ttk
    from tkinter import messagebox

baudrate_list = [[2000000, b'\x21\x0a\x00\xef\x2a\x31\x00\x00\x00\x80\x84\x1e\x00'],
                 [1000000, b'\x21\x0a\x00\x5e\x3d\x31\x00\x00\x00\x40\x42\x0f\x00'],
                 [921600,  b'\x21\x0a\x00\x5d\x50\x31\x00\x00\x00\x00\x10\x0e\x00'],
                 [460800,  b'\x21\x0a\x00\x07\x00\x31\x00\x00\x00\x00\x08\x07\x00'],
                 [115200,  b'\x21\x0a\x00\x97\x4b\x31\x00\x00\x00\x00\xc2\x01\x00']]

erase_cmd  = b'\x21\x0a\x00\xc3\x35\x32\x00\x00\x00\x02\x00\xfe\x01'

reboot_cmd = b'\x21\x06\x00\xc7\x7c\x3f\x00\x00\x00'

default_baudrate = 115200
default_button = "start"

sync_threshold   = 5

class SerialDownload:
    def __init__(self,band=default_baudrate):
        self.port = None
        self.port_list = list(serial.tools.list_ports.comports())
        assert (len(self.port_list) != 0),"invlid serial port"
        self.bandRate = band
        pass

    def get_port(self):
        return self.port_list

    def open_port(self,port):
        self.port = serial.Serial()
        self.port.port = port
        self.port.baudrate = self.bandRate
        self.port.timeout = None
        self.port.dtr = 0
        self.port.rts = 0
        try:
            self.port.open()
        except IOError:
            messagebox.showerror("flash failed", "Failed to open the serial port")
            self.delete_port()
            os._exit(0)
        else:
            pass

    def delete_port(self):
        if self.port != None:
            self.port.close()
            print("serial close")
        else:
            pass
    
    def send_esc(self, ms):
        delay = 10
        for x in range(int(ms / delay)):
            try:
                self.port.write(struct.pack('<B', 27))
            except IOError:
                messagebox.showerror("flash failed", "send esc write failed")
                os._exit(0)
            else:
                pass
            time.sleep(delay / 1000)

    def use_baudrate(self, baudrate):
        for i in range(len(baudrate_list)):
            if baudrate_list[i][0] == baudrate:
                self.port.write(baudrate_list[i][1])
                #self.port.close()
                time.sleep(0.02)
                self.port.baudrate = baudrate
                #dself.port.open()
                time.sleep(0.02)
                break
            if i == len(baudrate_list) - 1:
               messagebox.showerror("flash failed", "The baudrate is not supported")
               self.delete_port()
               os._exit(0)

    def try_baudrate(self):
        for i in range(len(baudrate_list)):
            print("trying baudrate %d..." % baudrate_list[i][0])

            #if self.port.is_open:
            #    self.port.close()
            #    time.sleep(0.02)

            #self.port.baudrate = baudrate_list[i][0]
            try:
                #self.port.open()
                self.port.baudrate = baudrate_list[i][0]
            except IOError:
                continue
            else:
                time.sleep(0.02)

                #self.port.close()
                #time.sleep(0.02)

                self.port.baudrate = default_baudrate

                #self.port.open()
                time.sleep(0.02)

                self.putc(baudrate_list[i][1])
                time.sleep(0.02)

                #self.port.close()
                #time.sleep(0.02)

                self.port.baudrate = baudrate_list[i][0]
                #self.port.open()
                time.sleep(0.02)

                break

    def putc_bar(self, data, timeout=1):
        self.progress['value'] += 1
        self.mainwin.update()
        return self.port.write(data)

    def putc(self, data, timeout=1):
        return self.port.write(data)

    def getc(self, size, timeout=1):
        if size < 0:
            messagebox.showerror("flash failed", "read size error")
            self.delete_port()
            os._exit(0)
        try:
            return self.port.read(size) or None
        except IOError:
            messagebox.showerror("flash failed", "read failed")
            self.delete_port()
            os._exit(0)

    def empty_read(self):
        while True:
            c = self.getc(1)
            if c == None:
                break

    def reset_device(self):
        self.port.dtr = 0
        self.port.rts = 1
        time.sleep(0.01)
        self.port.dtr = 1
        self.port.rts = 0
        time.sleep(0.01)
        self.port.dtr = 0

    def try_reset_device(self):
        cnt = 0
        while True:
            #reset device by rts pin
            self.reset_device()
            self.send_esc(20)

            #reset device by atcmd
            self.putc(bytes("AT+RST\r\n", encoding='ascii'))
            self.putc("AT+RST\r\n".encode('utf-8'))
            self.send_esc(20)

            #reset device by cli
            #self.putc(bytes("reboot\r\n", encoding='ascii'))
            self.putc("reboot\r\n".encode('utf-8'))
            self.send_esc(50)

            cnt = cnt + 1
            if cnt >= 10:
                break

    def sync_device(self, mainwin, button):
        cnt = 0
        note = 1
        time_start = time.time()
        while True:
            c = self.getc(1)
            if c == None:
                self.send_esc(30)
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
                print("please reset device")
                button['text'] = "please reset device"
                mainwin.update()
                note = 0
            elif timeuse > 60:
                print("serial sync timeout")
                self.delete_port()
                print("flash device failed")
                messagebox.showerror("flash failed", "serial sync timeout")
                button["state"] = "normal"
                mainwin.update()
                os._exit(3)

    def erase_flash(self):
        print("erase device flash...")
        #erase 2M flash cmd
        self.putc(erase_cmd)
        self.empty_read()
        cnt = 0
        time_start = time.time()
        while True:
            c = self.getc(1)
            if c != None:
                if c == b'C':
                    cnt = cnt + 1
                else:
                    cnt = 0
                    print("erase device failed: " + str(c))
                    self.delete_port()
                    messagebox.showerror("flash failed", "failed reason: " + str(c))
                    self.button["state"] = "normal"
                    self.mainwin.update()
                    os._exit(5)
            if cnt >= sync_threshold:
                break
        time_end = time.time()
        timeuse = time_end - time_start
        if timeuse > 60:
            print("erase device timeout")
            self.delete_port()
            print("erase device failed")
            messagebox.showerror("flash failed", "erase device timeout")
            self.button["state"] = "normal"
            self.mainwin.update()
            os._exit(4)
        print("erase device complete")

    def flash(self, baudrate, image, progress, mainwin, check_button, button):
        self.button = button
        self.mainwin = mainwin
        self.port.timeout  = 0.001
        button["state"] = "disabled"
        mainwin.update()

        print("trying reset device...")
        self.try_reset_device()

        print("wait serial sync...")
        self.sync_device(mainwin, button)
        button['text'] = "flash..."
        mainwin.update()
        print("serial sync sucess")

        if check_button.get() == True:
            self.erase_flash()

        if len(baudrate) == 0:
            messagebox.showerror("flash failed", "The baudrate cannot null")
            self.delete_port()
            os._exit(0)

        if baudrate != "auto":
            if baudrate.isdigit():
                if int(baudrate) != default_baudrate:
                    self.use_baudrate(int(baudrate))
            else:
                messagebox.showerror("flash failed", "The baudrate format is incorrect")
                self.delete_port()
                os._exit(0)
        else:
            self.try_baudrate()

        print("start download image...")

        for i in range(len(sys.argv) - 1):
            image = sys.argv[1 + i]
            print("download %s..." % os.path.basename(image))
            try:
                statinfo_bin = os.stat(image)
                progress['maximum'] = statinfo_bin.st_size / 1024
                progress['value'] = 0
                self.progress = progress
                self.mainwin = mainwin

                stream = open(image, 'rb+')
            except IOError:
                messagebox.showerror("flash error", "Invalid firmware file")
                self.delete_port()
                os._exit(0)
            else:
                modem = XMODEM1k(self.getc, self.putc_bar)

                self.empty_read()

                self.port.timeout = 1

                result = modem.send(stream)
                stream.close()

                if not result:
                    break

                self.port.timeout = 0.001
                if i != (len(sys.argv) - 1 - 1):
                    self.sync_device(mainwin, button)

        self.putc(reboot_cmd)
        self.delete_port()

        if result:
            messagebox.showinfo("flash complete", "flash device complete")
        else:
            messagebox.showerror("flash failed", "flash device failed")

        button['text'] = default_button
        button["state"] = "normal"
        mainwin.update()
        os._exit(0)

class SerialUI:
    def __init__(self):
        self.port = None
        self.band = None
        self.myserial = None

        self.mainwin = Tk()
        self.mainwin.title("flash")
        self.mainwin.geometry("430x155")
        self.mainwin.resizable(width=False,height=False)
        globals()["_icon_img"] = PhotoImage(data="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", format="gif")
        self.mainwin.tk.call("wm", "iconphoto", self.mainwin._w, "-default", _icon_img)

        self.label1 = Label(self.mainwin,text = "port:", font = ("",15))
        self.label1.place(x = 5,y = 5)
        self.label2 = Label(self.mainwin, text= "baudrate:", font=("", 15))
        self.label2.place(x=5, y=45)

        self.com1value = StringVar()
        self.combobox_port = ttk.Combobox(self.mainwin, textvariable=self.com1value,
                                          width = 30,font = ("",13))
        self.combobox_port["value"] = [""]
        self.combobox_port.place(x = 105,y = 5)

        self.bandvalue = StringVar()
        self.combobox_band = ttk.Combobox(self.mainwin, textvariable=self.bandvalue, width=30, font=("", 13))
        self.combobox_band["value"] = ["auto", "115200","460800","921600","1000000","2000000"]
        self.combobox_band.current(0)
        self.combobox_band.place(x=105, y=45)

        self.check_button_value = IntVar()
        self.check_button = Checkbutton(self.mainwin, text= "erase", font=("", 12), variable = self.check_button_value)
        self.check_button.place(x = 5, y = 85)

        self.button_OK = Button(self.mainwin, text=default_button,
                                command=self.button_OK_click, font = ("",15),
                                width = 20,height = 1)
        self.button_OK.place(x = 105,y = 85)

        self.progress_dl=ttk.Progressbar(self.mainwin, orient=HORIZONTAL, length=430, mode='determinate')
        self.progress_dl.pack(side=BOTTOM)

        self.band = self.combobox_band.get()
        self.myserial = SerialDownload(default_baudrate)

        self.port_list = self.myserial.get_port()
        port_str_list = []
        for i in range(len(self.port_list)):
            lines = str(self.port_list[i])
            str_list = lines.split(" ")
            port_str_list.append(str_list[0])
        self.combobox_port["value"] = port_str_list
        self.combobox_port.current(i)

    def show(self):
        self.mainwin.mainloop()

    def flash(self, image, argv):
        self.combobox_band['state'] = 'disabled'
        self.combobox_port['state'] = 'disabled'
        self.check_button['state'] = 'disabled'
        self.button_OK['state'] = 'disabled'
        self.myserial.flash(self.combobox_band.get(), image, self.progress_dl, self.mainwin, self.check_button_value, self.button_OK)
        self.combobox_band['state'] = 'normal'
        self.combobox_port['state'] = 'normal'
        self.check_button['state'] = 'normal'
        self.button_OK['state'] = 'normal'

    def button_OK_click(self):
        if self.myserial.port == None or self.myserial.port.isOpen() == False:
            self.myserial.open_port(self.combobox_port.get())
            argv = sys.argv
            flash_thread = threading.Thread(target=self.flash, kwargs={"image": argv[1], "argv": self})
            #flash_thread.setDaemon(True)
            flash_thread.daemon = True
            flash_thread.start()
        else:
            pass

if __name__ == '__main__':
    if len(sys.argv) < 2:
        messagebox.showerror("flash error", "Invalid firmware file")
        sys.exit(1)
    sui = SerialUI()
    sui.show()