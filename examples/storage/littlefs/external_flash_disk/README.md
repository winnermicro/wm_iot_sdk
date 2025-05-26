# LittleFS (外部 Flash 磁盘)

## 功能概述

此应用程序是使用 `WM IoT SDK` 进行 `LittleFS` 文件系统的基础示例。它演示了如何使用 `WM IoT SDK` 中的 `LittleFS` API 在外部 Flash 上进行基本的文件操作。示例包括：

1. 初始化 LittleFS：使用外部 Flash 初始化 LittleFS 配置；
2. 格式化文件系统：对文件系统进行格式化；
3. 挂载文件系统：将文件系统挂载到外部 Flash；
4. 文件操作演示：
   - 创建并写入文件：创建一个名为 hello.txt 的文件，并写入字符串 "Hello LittleFS!"；
   - 读取文件：读取 hello.txt 文件的内容并显示；
5. 卸载文件系统：卸载文件系统并清理资源。

本示例使用外部 Flash 作为存储介质，展示了如何在外部 Flash 上使用 LittleFS 文件系统进行基本的文件操作。

## 环境要求

1. 需要正确配置 `wm_drv_flash` 驱动；
2. 需要支持外部 Flash 的硬件环境，并连接上外部 Flash 设备，同时需要正确配置 `CONFIG_LITTLEFS_EXTERNAL_FLASH_DEVICE_NAME` 为外部 Flash Device Name；
3. 需要正确配置 `CONFIG_LITTLEFS_EXTERNAL_FLASH_START_ADDRESS` 为外部 Flash 用于 LittleFS 起始地址；
4. 需要正确配置 `CONFIG_LITTLEFS_EXTERNAL_FLASH_SIZE` 为外部 Flash 用于 LittleFS 大小；

## 编译和烧录

示例位置：`examples\storage\littlefs\external_flash_disk`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志
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
