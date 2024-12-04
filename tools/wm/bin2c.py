#!/usr/bin/python
import os
import sys

def main(argv):
    if(len(argv) != 4):
        print("bin2c <in_file> <out_file> <is_text>")
        sys.exit(-1)

    bin_file = argv[1]
    c_file = argv[2]
    is_text = int(argv[3])

    try:
        in_file = open(bin_file, 'rb') 
    except Exception as e:
        return -1

    out_file    = open(c_file, 'w')
    in_size     = os.path.getsize(bin_file)
    if is_text:
        in_size = in_size + 1
    array_name  = os.path.basename(c_file)
    array_name  = array_name.split('.')[0]
    out_file.write('const unsigned int %s = %u;\n'%(array_name + "_length", in_size))
    out_file.write('const unsigned char %s[%d] = {\n    '%(array_name, in_size))

    while True:
        block = in_file.read(1024)
        if block:
            for i in range(0, len(block)):
                out_file.write('0x%02X'%block[i]+', ')
                if not (i + 1) % 16:
                    out_file.write('\n    ')
        else:
            break

    in_file.close()
    if is_text:
        out_file.write('0x%02X'%0)
    out_file.write('\n};')
    out_file.close()
    #print('bin to c file complete')
    return 0

if __name__ == '__main__':
    main(sys.argv)