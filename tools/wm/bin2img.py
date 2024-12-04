#!/usr/bin/python
import os
import sys
import gzip
import lzma
import getopt
import struct
import binascii

my_version            = "1.0.0"

magic_no              = 0xA0FFFF9F

arg_image_type        = 0
arg_compress_type     = 0 # 1:gzip, 1<<4:xz
arg_run_address       = 0x80D0400
arg_image_header      = 0x80D0000
arg_update_address    = 0x8010000
arg_upd_no            = 0
arg_version_string    = "G03.00.00"
arg_next_image_header = 0
arg_input_binary      = None
arg_output_name       = None

help_usage = '''
Usage:

        bin2img.py [options]

options:
    -h,--help                        = print usage information and exit.
    -v,--version                     = print version number and exit.
    -i,--input-binary <file>         = original binary file.
    -o,--output-name <file>          = output firmware file.
    -c,--compress-type <type>        = compress type , <0 | 1(gzip) | 1<<4(xz)>.
    -I,--image-type <type>           = firmware image layout type, <0 | 1>.
    -H,--image-header <address>      = image header storage location (hexadecimal).
    -r,--run-address <address>       = runtime position (hexadecimal).
    -u,--update-address <address>    = upgrade storage location (hexadecimal).
    -n,--next-image-header <address> = next image header storage location (hexadecimal).
    -U,--upd-no <number>             = upd no version number (hexadecimal).
    -V,--version-string <version>    = firmware version string, cannot exceed 16 bytes.
'''

def prase_argv(argv):
    opts,args = getopt.getopt(argv[1:],'-h-v-i:-o:-c:-I:-H:-r:-u:-n:-U:-V:',['help','version','input-binary=','output-name=','compress-type=','image-type=','image-header=','run-address=','update-address=','next-image-header=','upd-no=','version-string='])

    global arg_input_binary
    global arg_output_name
    global arg_compress_type
    global arg_image_type
    global arg_image_header
    global arg_run_address
    global arg_update_address
    global arg_next_image_header
    global arg_upd_no
    global arg_version_string

    for opt_name,opt_value in opts:
        if opt_name in ('-h','--help'):
            print(help_usage)
            exit()
        if opt_name in ('-v','--version'):
            print("WinnerMicro firmware packaging tool, version is", my_version)
            exit()
        if opt_name in ('-i','--input-binary'):
            arg_input_binary = opt_value
        if opt_name in ('-o','--output-name'):
            arg_output_name = opt_value
        if opt_name in ('-c','--compress-type'):
            arg_compress_type = int(opt_value)
        if opt_name in ('-I','--image-type'):
            arg_image_type = int(opt_value)
        if opt_name in ('-H','--image-header'):
            arg_image_header = int(opt_value, 16)
        if opt_name in ('-r','--run-address'):
            arg_run_address = int(opt_value, 16)
        if opt_name in ('-u','--update-address'):
            arg_update_address = int(opt_value, 16)
        if opt_name in ('-n','--next-image-header'):
            arg_next_image_header = int(opt_value, 16)
        if opt_name in ('-U','--upd-no'):
            arg_upd_no = int(opt_value, 16)
        if opt_name in ('-V','--version-string'):
            arg_version_string = opt_value

def safety_crc32(indata):
    result = binascii.crc32(indata)
    #for python2 add check
    if result < 0:
        result = result + 2 ** 32
    return result

def main(argv):
    prase_argv(argv)

    if len(arg_version_string) < 16:
        newversion = arg_version_string + '\0' * (16 - len(arg_version_string))
    else:
        newversion = arg_version_string[:15] + '\0'

    bin_file_name = arg_input_binary

    if arg_compress_type == 1:
        file_prefix = bin_file_name + '.gz'
        with open(bin_file_name, 'rb') as f_in:
            with gzip.open(file_prefix, 'wb', compresslevel=9) as f_out:
                f_out.writelines(f_in)
                f_in.close()
                f_out.close()
        bin_file_name = file_prefix
    elif arg_compress_type == (1<<4):
        file_prefix = bin_file_name + '.xz'
        with open(bin_file_name, 'rb') as f_in:
            filters = [
                    {'id': lzma.FILTER_LZMA2,
                    'preset': 9 | lzma.PRESET_EXTREME,
                    'dict_size': (1<<15)}
                ]
            with lzma.open(file_prefix, 'wb', check=lzma.CHECK_CRC32, filters=filters) as f_out:
                f_out.write(f_in.read())
                f_in.close()
                f_out.close()
        bin_file_name = file_prefix

    try:
        f_bin = open(bin_file_name, mode='rb')
    except IOError:
        print("not find %s file" % bin_file_name)
        raise

    bin_data = f_bin.read()
    dummy = len(bin_data) % 4
    if dummy != 0:
        dummy = 4 - dummy
        bin_data += bytes([0xFF] * dummy)
    bin_len = len(bin_data)
    bin_crc = safety_crc32(bin_data) ^ (0xFFFFFFFF)

    magic            = struct.pack('<I', magic_no)
    img_type         = struct.pack('<H', arg_image_type)
    compress_type    = struct.pack('<H', arg_compress_type)
    run_img_addr     = struct.pack('<I', arg_run_address)
    run_img_len      = struct.pack('<I', bin_len)
    img_header_addr  = struct.pack('<I', arg_image_header)
    upd_img_addr     = struct.pack('<I', arg_update_address)
    run_org_checksum = struct.pack('<I', bin_crc)
    upd_no           = struct.pack('<I', arg_upd_no)
    ver_name         = newversion.encode('utf-8')
    reserved0        = struct.pack('<I', 0)
    reserved1        = struct.pack('<I', 0)
    next_boot        = struct.pack('<I', arg_next_image_header)

    image_header = magic + img_type + compress_type + run_img_addr + run_img_len \
                 + img_header_addr + upd_img_addr + run_org_checksum + upd_no \
                 + ver_name + reserved0 + reserved1 + next_boot

    header_crc = safety_crc32(image_header) ^ (0xFFFFFFFF)
    hd_checksum = struct.pack('<I', header_crc)

    try:
        f_img = open(arg_output_name, "wb+")
    except IOError:
        f_bin.close()
        print("create %s file fail" % arg_output_name)
        raise

    f_img.write(image_header)
    f_img.write(hd_checksum)
    f_img.write(bin_data)

    # write dummy data to pad 4byte-aligned
    # byteff = struct.pack('<B', 255)
    # dummy = len(bin_data) % 4 
    # if dummy != 0:
    #     dummy = 4 - dummy
    #     f_img.write(byteff * dummy)

    #len aligned 1K
    #dummy = len(bin_data) + dummy
    #if dummy < 1024 and (dummy % 1024) != 0:
    #    dummy = 1024 - (dummy % 1024)
    #    f_img.write(byteff * dummy)

    f_img.close()
    f_bin.close()
    exit()

if __name__ == '__main__':
    main(sys.argv)
