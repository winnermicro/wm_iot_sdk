# FATFS (Internal Flash Disk)

## Overview

This application is a comprehensive example of the `FATFS` file system using the `WM IoT SDK`. It demonstrates how to use the `FATFS` API in the `WM IoT SDK` for mounting and file read/write operations on an internal Flash disk, as well as how to package files from a PC into a FATFS image and flash it to the device (refer to [Build System](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/build_system.html#add-fatfs-files-to-img)). The example includes:

1. Registering the internal Flash disk: Registering the internal Flash disk with the FATFS system;
2. Mounting the file system: Mount the internal Flash disk to the file system.
   If the disk is not formatted, it needs to be formatted first. FATFS will automatically select the most suitable file system type based on disk capacity, as follows:
   - FAT12: Suitable for small capacity storage (≤ 2MB, maximum 4,085 clusters, MAX_FAT12 = 0xFF5 = 4,085 clusters = 2,042,560 Bytes ≈ 2MB)
   - FAT16: Suitable for medium capacity storage (≤ 32MB, maximum 65,493 clusters, MAX_FAT16 = 0xFFF5 = 65,493 clusters = 33,516,032 Bytes ≈ 32MB)
   - FAT32: Suitable for large capacity storage (≤ 128GB, maximum 268,435,445 clusters, MAX_FAT32 = 0x0FFFFFF5 = 268,435,445 clusters = 137,438,947,840 Bytes ≈ 128GB)
   - EXFAT: Suitable for extra large capacity storage (≤ 1TB, maximum 2,147,483,645 clusters, MAX_EXFAT = 0x7FFFFFFD = 2,147,483,645 clusters = 1,099,511,626,240 Bytes ≈ 1TB)
   - Note: The choice of file system type will affect the subsequent use of APIs.

   The following two types of volume IDs are supported:
   - Numeric volume ID: Uses the format `sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH)`, where `WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH` is the driver number for the internal Flash disk
   - String volume ID: Requires configuring `CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y` and `CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS` in menuconfig, allows using the format `sprintf(path, "%s:", CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS)` to support Windows-like drive letters (C:/D:/E:)
3. Creating a file: Creating a file named test.txt and writing the string "Hello, FatFs!" to it;
4. Reading a file: Reading the contents of test.txt and comparing it with the written content to verify data consistency;
5. File listing display: Recursively traverse directories, displaying detailed information for all files and folders, including file size, creation time, etc.;
6. Unmounting the file system: Unmounting the file system.

This example uses internal Flash as the storage medium, demonstrating how to use the FATFS file system on internal Flash.

## Environment Requirements

1. The `wm_drv_flash` driver must be configured correctly;
2. Use a user-defined partition named `partition_table_custom.csv`, and ensure that the partition named `CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME` (with a default name of `fatfs`) is included, the partition size must be at least 0x18000 (96KB), otherwise formatting will fail;

## Compilation and Flashing

Example location: `examples\storage\fatfs\internal_flash_disk`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Results

Upon successful execution, the following logs will be output.

```
[I] (10) main: ver: 2.3-alpha.1 build at Dec 30 2024 11:21:03
[I] (10) main: boot reason 0
[I] (10) main: heap size 271840(265.47KB)
[I] (11) main: flash size 0x200000(2MB)
[I] (12) example: FATFS demo start.
[I] (12) example: File system mount success, fs_type: 1
[I] (107) example: write txt: Hello, FatFs!
[I] (107) example: read txt: Hello, FatFs!
[I] (108) example: File system mount success, fs_type: 1
.
+-- 111.txt (3B, 2024-12-30 11:20:58)
+-- abc/
|   +-- 222.txt (3B, 2024-12-30 11:20:58)
+-- def/
|   +-- 333.txt (3B, 2024-12-30 11:20:58)
|   +-- ghi/
|   |   +-- 444.txt (3B, 2024-12-30 11:20:58)
+-- fatfs_file.txt (14B, 2024-12-30 11:20:58)
+-- test.txt (13B, 2000-01-01 00:00:00)
[I] (111) example: File listing completed successfully.
[I] (111) example: This example succeeds in running.
```