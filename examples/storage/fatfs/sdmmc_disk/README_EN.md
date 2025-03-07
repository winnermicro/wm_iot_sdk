# FATFS (SDMMC Disk)

## Overview

This application is a comprehensive example of the `FATFS` file system using the `WM IoT SDK`. It demonstrates how to mount an SD card on an SDMMC disk and perform file read/write operations using the `FATFS` API in the `WM IoT SDK`. The example includes:

1. Registering the SDMMC disk: Registering the SDMMC disk with the FATFS system;
2. Mounting the file system: Mounting the SDMMC disk to the file system.
   If the disk is unformatted, it must to be formatted first. FATFS will automatically select the most suitable file system type based on disk capacity, as follows:
   - FAT12: Suitable for small capacity storage (≤ 2MB, maximum 4,085 clusters, MAX_FAT12 = 0xFF5 = 4,085 clusters = 2,042,560 Bytes ≈ 2MB)
   - FAT16: Suitable for medium capacity storage (≤ 32MB, maximum 65,493 clusters, MAX_FAT16 = 0xFFF5 = 65,493 clusters = 33,516,032 Bytes ≈ 32MB)
   - FAT32: Suitable for large capacity storage (≤ 128GB, maximum 268,435,445 clusters, MAX_FAT32 = 0x0FFFFFF5 = 268,435,445 clusters = 137,438,947,840 Bytes ≈ 128GB)
   - EXFAT: Suitable for extra large capacity storage (≤ 1TB, maximum 2,147,483,645 clusters, MAX_EXFAT = 0x7FFFFFFD = 2,147,483,645 clusters = 1,099,511,626,240 Bytes ≈ 1TB)
   - Note: The choice of file system type will affect the subsequent use of APIs.

   The following two types of volume IDs are supported:
   - Numeric volume ID: Uses the format `sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_SDMMC)`, where `WM_DISKIO_DRIVER_NUM_SDMMC` is the driver number for the SDMMC disk
   - String volume ID: Requires configuring `CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y` and `CONFIG_FATFS_SDMMC_DISK_FF_VOLUME_STRS` in menuconfig, allows using the format `sprintf(path, "%s:", CONFIG_FATFS_SDMMC_DISK_FF_VOLUME_STRS)` to support Windows-like drive letters (C:/D:/E:)
3. Creating a file: Creating a file named test.txt and writing the string "Hello, FatFs!" to it;
4. Reading the file: Reading the contents of test.txt and comparing it with the written content to verify data consistency;
5. Unmounting the file system: Unmounting the file system.

This example uses the SDMMC disk as the storage medium, demonstrating how to use the FATFS file system on the SDMMC disk.

## Environment Requirements

1. The `wm_drv_sdh_sdmmc` driver must be correctly configured.
2. A hardware environment that supports SD cards is required, and an SD card must be inserted.

## Compilation and Flashing

Example location: `examples\storage\fatfs\sdmmc_disk`

For compilation, flashing, and other operations, please refer to: [Quick Start](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Results

Upon successful execution, the following logs will be output.
```
[I] (146) main: ver: 2.0.5dev build at Sep 30 2024 13:36:01
[I] (249) example: FATFS demo start.
[I] (249) example: SDMMC driver registered successfully.
[I] (249) diskio_sdmmc: Initializing sdmmc device...
[I] (258) example: File system not found. Formatting...
[I] (438) example: File system formatted successfully.
[I] (453) example: write txt: Hello, FatFs!
[I] (453) example: read txt: Hello, FatFs!
[I] (453) example: This example succeeds in running.
```