import os
import re
import sys
import subprocess

def main(argv):
    # Check if the number of arguments is correct
    if len(argv) < 7 or (len(argv) - 4) // 3 <= 0 or (len(argv) - 4) % 3 != 0:
        print("usage: %s <partition_table_file_path> <partition_name> <output_path> <custom_file1_path> <partition_offset> <custom_file1_length> [<custom_file2_path> <partition_offset> <custom_file2_length> ...]" % os.path.basename(argv[0]))
        exit(-1)

    # Extract partition information from the partition table file
    partition_table_file_path = argv[1]
    partition_name = argv[2]
    output_path = argv[3]
    partition_offset = 0
    partition_size = 0
    if not os.path.exists(partition_table_file_path):
        print("Error: The specified partition table path '%s' does not exist." % partition_table_file_path)
        exit(-1)

    with open(partition_table_file_path, 'r', encoding='utf-8') as partition_table_file:
        found_flag = False
        for line in partition_table_file.readlines():
            line = line.strip()
            # Skip lines starting with '#'
            if line.startswith('#'):
                continue
            entry = line.split(',')
            name = entry[0].strip()
            # If the partition name matches, extract the offset and size
            if name == partition_name:
                partition_offset = int(entry[1].strip(), 16)
                partition_size = int(entry[2].strip(), 16)
                found_flag = True
                break

    if not found_flag:
        print("Error: Partition name '%s' not found in the partition table." % partition_name)
        exit(-1)

    # Check the file list and prepare the custom file information
    custom_file_list_table = []
    for i in range((len(argv) - 4) // 3):
        file_path = argv[4 + i * 3]
        file_offset = argv[4 + i * 3 + 1]
        file_length = argv[4 + i * 3 + 2]
        custom_file_list_table.append({
            'file_path': file_path,
            'offset': int(file_offset),
            'length': int(file_length)
        })
        if not os.path.exists(file_path):
            print("Error: The custom file path '%s' does not exist." % file_path)
            exit(-1)
        file_size = os.path.getsize(file_path)
        if int(file_length) > file_size:
            print("Error: The specified offset and length exceed the file size for '%s'." % file_path)
            exit(-1)

    # Generate the binary file from the custom files
    output_bin_file_path = os.path.join(output_path, f"{partition_name}.bin")
    with open(output_bin_file_path, 'wb') as output_file:
        for file_info in custom_file_list_table:
            with open(file_info['file_path'], 'rb') as f:
                if file_info['length'] >= 0:
                    chunk = f.read(file_info['length'])
                elif file_info['length'] == -1:
                    chunk = f.read()
                else:
                    print("Error: The specified length is invalid for custom file.")
                    exit(-1)

                output_file.seek(file_info['offset'])
                output_file.write(chunk)
                if (file_info['offset'] + file_info['length'] > partition_size) :
                    print("Error: The specified offset and length exceed the partition size for custom file.")
                    exit(-1)

    # Use a Python script to convert the binary file to an image file
    script_dir = os.path.dirname(__file__)
    pack_tool = os.path.join(script_dir, "bin2img.py")
    if not os.path.exists(pack_tool):
        print("Error: The package tool file path '%s' does not exist." % pack_tool)
        exit(-1)
    output_img_file_path = os.path.join(output_path, f"{partition_name}.img")
    flash_base_addr = 0x8000000
    run_address = flash_base_addr + partition_offset
    run_address_hex = "0x" + hex(run_address).upper()[2:]
    command = ["python"] + [pack_tool] + [
        "--input-binary", output_bin_file_path,
        "--output-name", output_img_file_path,
        "--compress-type", "0",
        "--image-type", "2",
        "--image-header", "0x20026000",
        "--run-address", run_address_hex,
        "--update-address", "0",
        "--next-image-header", "0",
        "--upd-no", "0"
    ]
    subprocess.run(command, check=True)

if __name__ == '__main__':
    main(sys.argv)