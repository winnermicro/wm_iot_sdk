#!/usr/bin/python
import sys
from elftools.elf.elffile import ELFFile

def main(argv):
    if(len(argv) != 4):
        print("elf2bin <elf_file> <bin_file> <bin_max_size>")
        sys.exit(-1)

    count = 0
    length = 0
    section_pos = []
    section_len = []
    elf_path = argv[1]
    bin_path = argv[2]
    bin_max_size = int(argv[3], 16)
    file = open(elf_path, 'rb')
    elf_file = ELFFile(file)
    total_length = 0
    for section in elf_file.iter_sections():
        if section.header['sh_type'] == 'SHT_PROGBITS' and section.header['sh_addr'] != 0:
            if section.name == '.text':
                section_pos.insert(0, section.header['sh_offset'])
                section_len.insert(0, section.header['sh_size'])
            elif section.name == '.rodata':
                section_pos.insert(1, section.header['sh_offset'])
                section_len.insert(1, section.header['sh_size'])
            elif section.name == '.data':
                section_pos.insert(2, section.header['sh_offset'])
                section_len.insert(2, section.header['sh_size'])
            else:
                continue
            total_length += int(section.header['sh_size'])

    if total_length > int(bin_max_size):
        print("Generating binary from elf file failed, exceeded by {} bytes".format(total_length - int(bin_max_size)))
        print("Memory region         Used Size  Region Size  %age Used")
        print("           FLASH:      %d B      %d KB     %.2f%%" % (total_length, int(bin_max_size)/1024, total_length/int(bin_max_size)*100))
        sys.exit(-1)

    file.close()
    file = open(elf_path, 'rb')
    with open(bin_path, 'wb') as bin_file:
        for i, j in zip(section_pos, section_len):
            file.seek(i, 0)
            write_to_bin = file.read(j)
            bin_file.write(write_to_bin)
            count = count + 1
            length = length + j
        bin_file.close()
        #print("Merged %d sections (%d bytes)" % (count, length))

    return 0

if __name__ == '__main__':
    main(sys.argv)