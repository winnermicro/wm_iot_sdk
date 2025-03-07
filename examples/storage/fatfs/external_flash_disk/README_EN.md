# FATFS (External Flash Disk)

## Overview

This application is a comprehensive example of the `FATFS` file system using the `WM IoT SDK`. It demonstrates how to use the `FATFS` API in the `WM IoT SDK` for SD card mounting and file read/write operations on an external Flash disk. The example includes:

1. Registering the external Flash disk: Registering the external Flash disk to the FATFS system;
2. Mounting the file system: Mount the external Flash disk to the file system.
   If the disk is not formatted, it needs to be formatted first. FATFS will automatically select the most suitable file system type based on disk capacity, as follows:
   - FAT12: Suitable for small capacity storage (≤ 2MB, maximum 4,085 clusters, MAX_FAT12 = 0xFF5 = 4,085 clusters = 2,042,560 Bytes ≈ 2MB)
   - FAT16: Suitable for medium capacity storage (≤ 32MB, maximum 65,493 clusters, MAX_FAT16 = 0xFFF5 = 65,493 clusters = 33,516,032 Bytes ≈ 32MB)
   - FAT32: Suitable for large capacity storage (≤ 128GB, maximum 268,435,445 clusters, MAX_FAT32 = 0x0FFFFFF5 = 268,435,445 clusters = 137,438,947,840 Bytes ≈ 128GB)
   - EXFAT: Suitable for extra large capacity storage (≤ 1TB, maximum 2,147,483,645 clusters, MAX_EXFAT = 0x7FFFFFFD = 2,147,483,645 clusters = 1,099,511,626,240 Bytes ≈ 1TB)
   - Note: The choice of file system type will affect the subsequent use of APIs.

   The following two types of volume IDs are supported:
   - Numeric volume ID: Uses the format `sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH)`, where `WM_DISKIO_DRIVER_NUM_EXTERNAL_FLASH` is the driver number for the external Flash disk
   - String volume ID: Requires configuring `CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y` and `CONFIG_FATFS_EXTERNAL_FLASH_DISK_FF_VOLUME_STRS` in menuconfig, allows using the format `sprintf(path, "%s:", CONFIG_FATFS_EXTERNAL_FLASH_DISK_FF_VOLUME_STRS)` to support Windows-like drive letters (C:/D:/E:)
3. Creating a file: Creating a file named test.txt and writing the string "Hello, FatFs!" to it;
4. Reading the file: Reading the contents of the test.txt file and comparing it with the written content to verify data consistency;
5. Unmounting the file system: Unmounting the file system.

This example uses external Flash as the storage medium, demonstrating how to use the FATFS file system on external Flash.

## Environment Requirements

1. The `wm_drv_flash` driver needs to be correctly configured;
2. A hardware environment that supports external Flash is required, and the external Flash device must be connected, with `CONFIG_FATFS_EXTERNAL_FLASH_DEVICE_NAME` correctly configured for the external Flash Device Name;
3. `CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS` must be correctly configured for the starting address of FATFS on the external Flash;
4. `CONFIG_FATFS_EXTERNAL_FLASH_SIZE` must be correctly configured for the size of FATFS on the external Flash, the partition size must be at least 0x18000 (96KB), otherwise formatting will fail;;

## Compilation and Flashing

Example location: `examples\storage\fatfs\external_flash_disk`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## Running Results

Upon successful execution, the following logs will be output.

```
[I] (146) main: ver: 2.0.5dev build at Sep 30 2024 13:41:31
[I] (250) example: FATFS demo start.
[I] (250) example: External flash driver registered successfully.
[I] (250) example: File system not found. Formatting...
[I] (1255) example: File system formatted successfully.
[I] (1328) example: write txt: Hello, FatFs!
[I] (1328) example: read txt: Hello, FatFs!
[I] (1328) example: This example succeeds in running.
```