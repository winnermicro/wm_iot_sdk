
.. _FS-AT:
.. |Equipment-Name| replace:: w800

*********************
FS AT 命令集
*********************

-  :ref:`AT+FSMOUNT <cmd-FSMOUNT>`：文件系统挂载与卸载
-  :ref:`AT+FS <cmd-FS>`：文件系统操作

.. _cmd-FSMOUNT:

:ref:`AT+FSMOUNT <FS-AT>`：文件系统挂载与卸载
------------------------------------------------------------

支持挂载 SDMMC、内部 Flash 和外部 Flash 三种类型的磁盘，可以通过 menuconfig 配置选择挂载的磁盘类型，默认挂载 SDMMC 磁盘。

环境要求：

1. SDMMC 磁盘
   - 需要正确配置 wm_drv_sdh_sdmmc 驱动
   - 需要支持 SD 卡的硬件环境，并插上 SD 卡

2. 内部 Flash 磁盘
   - 需要正确配置 wm_drv_flash 驱动
   - 使用用户自定义分区 partition_table_custom.csv，并添加 CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME (默认名称为 fatfs) 分区，分区大小需要 ≥ 0x18000(96KB)，否则会导致格式化失败
   - 如需将文件或文件夹添加到 FATFS 文件系统镜像中，请参考 :ref:`构建系统 <ADD_FATFS_FILES_TO_IMG>` 章节

3. 外部 Flash 磁盘
   - 需要正确配置 wm_drv_flash 驱动
   - 需要支持外部 Flash 的硬件环境，并连接上外部 Flash 设备
   - 提供有 tools/wm/mkfs2img.py 工具，用于将 PC 上的文件或文件夹打包成 FATFS 镜像
   - 需要正确配置以下参数:

     - CONFIG_FATFS_EXTERNAL_FLASH_DEVICE_NAME: 外部 Flash Device Name
     - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS: 外部 Flash 用于 FATFS 起始地址
     - CONFIG_FATFS_EXTERNAL_FLASH_SIZE: 外部 Flash 用于 FATFS 大小，分区大小至少为0x18000(96KB)，否则会导致格式化失败

设置命令
^^^^^^^^

**命令：**

::

    AT+FSMOUNT=<mount>

**响应：**

::

    OK

参数
^^^^

-  **<mount>**：

   -  0：卸载 FS 文件系统
   -  1：挂载 FS 文件系统

说明
^^^^

-  :ref:`AT+FS <cmd-FS>` 文件系统操作完成后，建议使用 :ref:`AT+FSMOUNT=0 <cmd-FSMOUNT>` 命令来卸载文件系统，以释放大量 RAM 空间。

示例
^^^^

::

    // 手动卸载文件系统
    AT+FSMOUNT=0

    // 手动挂载文件系统
    AT+FSMOUNT=1


.. _cmd-FS:

:ref:`AT+FS <FS-AT>`：文件系统操作
------------------------------------------------------------

设置命令
^^^^^^^^

**命令：**

::

    AT+FS=<type>,<operation>,<filename>[,<offset>,<length>]

**响应：**

::

    OK

参数
^^^^

-  **<type>**：目前仅支持 FATFS

   -  0：FATFS

-  **<operation>**:

   -  0：删除文件
   -  1：写文件
   -  2：读文件
   -  3：查询文件大小
   -  4：查询路径下文件，目前仅支持 "/" 根目录

-  **<filename>**：文件名（限制为8个字符，可通过 menuconfig 配置 ``File name length configuratio`` 以支持长文件名）; operation 为 4 时表示目录，目前仅支持 "/" 根目录

-  **<offset>**：偏移地址，仅针对读写操作设置

-  **<length>**：长度，仅针对读写操作设置

说明
^^^^

-  本命令会自动挂载文件系统。:ref:`AT+FS <cmd-FS>` 文件系统操作完成后，建议使用 :ref:`AT+FSMOUNT=0 <cmd-FSMOUNT>` 命令来卸载文件系统，以释放大量 RAM 空间。
-  若读取数据的长度大于实际文件大小，仅返回实际长度的数据。
-  当 ``<operator>`` 为 ``write`` 时，系统收到此命令后先换行返回 ``>``，此时您需要在 6 秒内输入要写的数据，数据长度应与 ``<length>`` 一致。
-  挂载磁盘时，如果没有格式化，会自动进行格式化。FATFS 会根据磁盘容量自动选择最合适的文件系统类型，具体如下：

   - FAT12：适用于小容量存储（≤ 2MB，最大支持 4085 个簇，MAX_FAT12 = 0xFF5 = 4085 簇 = 2091520 Bytes ≈ 2MB）
   - FAT16：适用于中等容量存储（≤ 32MB，最大支持 65525 个簇，MAX_FAT16 = 0xFFF5 = 65525 簇 = 33548800 Bytes ≈ 32MB）
   - FAT32：适用于大容量存储（≤ 128GB，最大支持 268435445 个簇，MAX_FAT32 = 0x0FFFFFF5 = 268435445 簇 = 137438947840 Bytes ≈ 128GB）
   - EXFAT：适用于超大容量存储（≤ 1TB，最大支持 2147483645 个簇，MAX_EXFAT = 0x7FFFFFFD = 2147483645 簇 = 1099511626240 Bytes ≈ 1TB）
   - 注意：文件系统类型的选择会影响到 ``<offset>`` 的最大值和后续 API 的使用方式。

示例
^^^^

.. code-block:: text

    // 删除某个文件
    AT+FS=0,0,"filename"

    // 在某个文件偏移地址 0 处写入 10 字节
    AT+FS=0,1,"filename",0,10

    // 从某个文件偏移地址 0 处读取 10 字节
    AT+FS=0,2,"filename",0,10

    // 查询某个文件大小
    AT+FS=0,3,"filename"

    // 列出根目录下所有文件
    AT+FS=0,4,"."
