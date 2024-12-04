#!/usr/bin/python
#
#                                partition table
#----------------------------------------------------------------------------
#|                                magic(4byte)                              |
#|--------------------------------------------------------------------------|
#|              version(2byte)          |             count(2byte)          |
#|--------------------------------------------------------------------------|
#|                                option(4byte)                             |
#|--------------------------------------------------------------------------|
#|   name(16byte)   |   offset(4byte)   |   size(4byte)   |   flag(4byte)   |
#|--------------------------------------------------------------------------|
#|                              md5-checksum(16byte)                        |
#|--------------------------------------------------------------------------|
#|                         ......(option: signature...)                     |
#|--------------------------------------------------------------------------|
#
import os
import sys
import struct
import hashlib

def check_partition_consistency(partition, expected):
    return (partition['offset'] == expected['offset'] and
            partition['size'] == expected['size'] and
            partition['flag'] == expected['flag'])

def is_4k_aligned(partition):
    return partition['offset'] % 4096 == 0 and partition['size'] > 0 and partition['size'] % 4096 == 0

def check_for_overlap(partitions):
    partitions.sort(key=lambda x: x['offset'])
    for i in range(1, len(partitions)):
        if partitions[i - 1]['offset'] + partitions[i - 1]['size'] - 1 > partitions[i]['offset']:
            return False
    return True

def validate_partition_table(def_file_path):
    # Expected partition information
    expected_partitions = {
        'ft':              { 'offset': 0x0,    'size': 0x2000, 'flag': 0x0 },
        'bootloader':      { 'offset': 0x2000, 'size': 0xC000, 'flag': 0x0 },
        'partition_table': { 'offset': 0xE000, 'size': 0x1000, 'flag': 0x0 },
    }

    # Read partition definition file and parse it
    partitions = []
    if not os.path.exists(def_file_path):
        print(f"Error: File '{def_file_path}' does not exist.")
        return False

    with open(def_file_path, 'r') as file:
        for line in file:
            parts = line.strip().split(',')
            if not parts[0].startswith('#'):
                partition = {
                    'name': parts[0],
                    'offset': int(parts[1], 16),
                    'size': int(parts[2], 16),
                    'flag': int(parts[3], 16)
                }
                partitions.append(partition)

    # Check the consistency of partitions
    for partition in partitions:
        if partition['name'] in expected_partitions:
            if not check_partition_consistency(partition, expected_partitions[partition['name']]):
                print(f"Error: Inconsistency found for partition {partition['name']}")
                expected_details = ', '.join([f"{key}: 0x{value:X}" for key, value in expected_partitions[partition['name']].items()])
                print(f"Error: Expected partition '{partition['name']}' to be: {{{expected_details}}}")
                return False

    # Check if the partition table contains 'ft', 'bootloader', 'partition_table' and 'app' partitions
    required_partitions = ['ft', 'bootloader', 'partition_table', 'app']
    missing_partitions = [p for p in required_partitions if p not in [part['name'] for part in partitions]]
    if missing_partitions:
        print(f"Error: Missing required partition '{', '.join(missing_partitions)}'")
        return False

    # Check if the partition is 4K aligned
    for partition in partitions:
        if not is_4k_aligned(partition):
            print(f"Error: Partition {partition['name']} is not 4K aligned.")
            return False

    # Check if partitions overlap
    if not check_for_overlap(partitions):
        print("Error: Partitions overlap detected.")
        return False

    # Check the length of partition names
    for partition in partitions:
        if len(partition['name']) >= 16:
            print(f"Error: Partition name '{partition['name']}' exceeds 15 characters.")
            return False

    return True

def main(argv):
    if len(argv) != 6 and len(argv) != 3 and len(argv) != 2:
        print("usage: %s <in_def_file> [<out_bin_file> <cmake_config_file> <c_header_file> <header_size>]" % os.path.basename(argv[0]))
        exit(-1)

    if len(argv) == 2:
        with open(argv[1], 'r', encoding='utf-8') as def_file:
            line = def_file.readlines()
            print("            name,            offset,    size,      flag")
            for i in line:
                i = i.strip()
                if (i.startswith('#')):
                    continue
                print(f"            {i}")
        exit(0)

    ret = validate_partition_table(argv[1])
    if ret == False:
        exit(-1)

    def_cnt = 0
    with open(argv[1], 'r', encoding='utf-8') as def_file:
        line = def_file.readlines()
        for i in line:
            i = i.strip()
            if (i.startswith('#')):
                continue
            def_cnt = def_cnt + 1

        magic           = struct.pack('<I', 0xA1F2F39F)
        version         = struct.pack('<H', 0x0)
        count           = struct.pack('<H', def_cnt)
        option          = struct.pack('<I', 0x0)
        partition_table = magic + version + count + option

        if len(argv) == 6:
            cmake_config    = "\n"
            c_head_config   = "\n"

        for i in line:
            i = i.strip()
            if (i.startswith('#')):
                continue
            entry = i.split(',')
            
            if len(entry[0].strip()) < 16:
                name = entry[0].strip() + '\0' * (16 - len(entry[0].strip()))
            else:
                name = entry[0].strip()[:15] + '\0'
            offset = int(entry[1].strip(), 16)
            size = int(entry[2].strip(), 16)
            flag = int(entry[3].strip(), 16)
            
            partition_name   = name.encode('utf-8')
            partition_offset = struct.pack('<I', offset)
            partition_size   = struct.pack('<I', size)
            partition_flag   = struct.pack('<I', flag)
            partition_table  = partition_table + partition_name + partition_offset + partition_size + partition_flag

            if len(argv) == 6:
                flash_base_addr = 0x8000000
                if entry[0].strip() == "bootloader":
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_BOOTLOADER_HEADER_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr + int(argv[5], 16))
                    cmake_config = cmake_config + 'set(CONFIG_BIN_BOOTLOADER_RUN_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_BOOTLOADER_RUN_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(size - int(argv[5], 16))
                    cmake_config = cmake_config + 'set(CONFIG_BIN_BOOTLOADER_LENGTH "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_BOOTLOADER_LENGTH ' + hex_value + '\n'
                elif entry[0].strip() == "app":
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_APP_HEADER_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_APP_HEADER_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr + int(argv[5], 16))
                    cmake_config = cmake_config + 'set(CONFIG_BIN_APP_RUN_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_APP_RUN_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(size - int(argv[5], 16))
                    cmake_config = cmake_config + 'set(CONFIG_BIN_APP_LENGTH "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_APP_LENGTH ' + hex_value + '\n'
                elif entry[0].strip() == "bootloader_ota":
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_BOOTLOADER_OTA_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_BOOTLOADER_OTA_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(size)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_BOOTLOADER_OTA_LENGTH "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_BOOTLOADER_OTA_LENGTH ' + hex_value + '\n'
                elif entry[0].strip() == "app_ota":
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_APP_OTA_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_APP_OTA_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(size)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_APP_OTA_LENGTH "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_APP_OTA_LENGTH ' + hex_value + '\n'
                else:
                    entry_name = entry[0].strip()
                    entry_name2 = entry_name.upper()
                    hex_value = '0x{:02X}'.format(offset + flash_base_addr)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_' + entry_name2 + '_ADDRESS "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_' + entry_name2 + '_ADDRESS ' + hex_value + '\n'
                    hex_value = '0x{:02X}'.format(size)
                    cmake_config = cmake_config + 'set(CONFIG_BIN_' + entry_name2 + '_LENGTH "' + hex_value + '")\n'
                    c_head_config = c_head_config + '#define CONFIG_BIN_' + entry_name2 + '_LENGTH ' + hex_value + '\n'

    def_file.close()

    partition_md5_chksum = hashlib.md5(partition_table).digest()
    partition_table = partition_table + partition_md5_chksum

    with open(argv[2], 'wb+') as bin_file:
        bin_file.write(partition_table)
        #len aligned 1K
        dummy = 1024 - len(partition_table)
        if dummy != 0:
            byteff = struct.pack('<B', 255)
            bin_file.write(byteff * dummy)
        #print("generate partition table done")
    bin_file.close()

    if len(argv) == 6:
        with open(argv[3], 'a') as cmake_config_file:
            #print(cmake_config)
            cmake_config_file.write(cmake_config)
        cmake_config_file.close()

        with open(argv[4], 'a') as c_head_file:
            #print(c_head_config)
            c_head_file.write(c_head_config)
        c_head_file.close()

    return 0

if __name__ == '__main__':
    main(sys.argv)