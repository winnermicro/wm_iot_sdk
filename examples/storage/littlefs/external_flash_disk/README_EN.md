# LittleFS (External Flash Disk)

## Overview

This application is a basic example of using the `LittleFS` file system with the `WM IoT SDK`. It demonstrates how to use the `LittleFS` API in the `WM IoT SDK` to perform basic file operations on external Flash. The example includes:

1. Initializing LittleFS: Initialize LittleFS configuration using external Flash;
2. Formatting the file system: Format the file system;
3. Mounting the file system: Mount the file system to external Flash;
4. File operation demonstration:
   - Create and write file: Create a file named hello.txt and write the string "Hello LittleFS!" to it;
   - Read file: Read and display the contents of hello.txt;
5. Unmounting the file system: Unmount the file system and clean up resources.

This example uses external Flash as the storage medium, demonstrating how to use the LittleFS file system on external Flash for basic file operations.

## Environment Requirements

1. The `wm_drv_flash` driver must be configured correctly;
2. Hardware environment that supports external Flash is required, with external Flash device connected, and `CONFIG_LITTLEFS_EXTERNAL_FLASH_DEVICE_NAME` correctly configured as the external Flash Device Name;
3. `CONFIG_LITTLEFS_EXTERNAL_FLASH_START_ADDRESS` must be correctly configured as the starting address for LittleFS on the external Flash;
4. `CONFIG_LITTLEFS_EXTERNAL_FLASH_SIZE` must be correctly configured as the size allocated for LittleFS on the external Flash;

## Compilation and Flashing

Example location: `examples\storage\littlefs\external_flash_disk`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Results

Upon successful execution, the following logs will be output.

```
[I] (3) main: ver: 2.3-beta.2 build at Apr 14 2025 11:49:06
[I] (4) main: boot reason 0
[I] (4) main: heap size 272560(266.17KB)
[I] (5) main: flash size 0x200000(2MB)
[I] (5) lfs: LittleFS demo start.     
[I] (16) lfs: Writing to file: hello.txt
[I] (22) lfs: Reading from file: hello.txt
[I] (23) lfs: Read content: Hello LittleFS!
[I] (23) lfs: File operations completed successfully.
[I] (24) lfs: This example succeeds in running.
```