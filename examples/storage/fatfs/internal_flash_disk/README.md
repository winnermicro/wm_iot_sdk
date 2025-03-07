# FATFS (内部 Flash 磁盘)

## 功能概述

此应用程序是使用 `WM IoT SDK` 进行 `FATFS` 文件系统的全面示例。它演示了如何使用 `WM IoT SDK` 中的 `FATFS` API 在内部 Flash 磁盘上进行 SD 卡挂载及文件读写操作，以及如何将 PC 上的文件打包成 FATFS 镜像并烧录到设备中 (可参考 [构建系统](https://doc.winnermicro.net/w800/zh_CN/latest/component_guides/build_system.html#add-fatfs-files-to-img))。示例包括：

1. 注册内部 Flash 磁盘：将内部 Flash 磁盘注册到 FATFS 系统；
2. 挂载文件系统：将内部 Flash 磁盘挂载到文件系统中。
   如果磁盘未格式化，需先进行格式化。FATFS 会根据磁盘容量自动选择最合适的文件系统类型，具体如下：
   - FAT12：适用于小容量存储（≤ 2MB，最大支持 4085 个簇，MAX_FAT12 = 0xFF5 = 4085 簇 = 2091520 Bytes ≈ 2MB）
   - FAT16：适用于中等容量存储（≤ 32MB，最大支持 65525 个簇，MAX_FAT16 = 0xFFF5 = 65525 簇 = 33548800 Bytes ≈ 32MB）
   - FAT32：适用于大容量存储（≤ 128GB，最大支持 268435445 个簇，MAX_FAT32 = 0x0FFFFFF5 = 268435445 簇 = 137438947840 Bytes ≈ 128GB）
   - EXFAT：适用于超大容量存储（≤ 1TB，最大支持 2147483645 个簇，MAX_EXFAT = 0x7FFFFFFD = 2147483645 簇 = 1099511626240 Bytes ≈ 1TB）
   - 注意：文件系统类型的选择会影响后续 API 的使用方式。

   支持以下两种盘符:
   - 数字盘符: 使用 `sprintf(path, "%d:", WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH)` 格式，其中 `WM_DISKIO_DRIVER_NUM_INTERNAL_FLASH` 为内部 Flash 磁盘的驱动号
   - 字符串盘符: 需在 menuconfig 中配置 `CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y` 和 `CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS`，可使用 `sprintf(path, "%s:", CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS)` 格式来支持类似 Windows 的盘符(C:/D:/E:)
3. 创建文件：创建一个名为 test.txt 的文件，并写入字符串 "Hello, FatFs!"；
4. 读取文件：读取 test.txt 文件的内容，并与写入的内容进行比较，验证数据一致性；
5. 文件列表展示：递归遍历目录，展示所有文件和文件夹的详细信息，包括文件大小、创建时间等；
6. 卸载文件系统：卸载文件系统。

本示例使用内部 Flash 作为存储介质，展示了如何在内部 Flash 上使用 FATFS 文件系统。

## 环境要求

1. 需要正确配置 `wm_drv_flash` 驱动；
2. 使用用户自定义分区 `partition_table_custom.csv`，并添加 `CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME`(默认名称为 `fatfs`) 分区, 分区大小至少为0x18000(96KB)，否则会导致格式化失败；

## 编译和烧录

示例位置：`examples\storage\fatfs\internal_flash_disk`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志
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
