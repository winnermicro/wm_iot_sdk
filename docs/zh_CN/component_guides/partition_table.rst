
.. _partition_table:

**分区表机制**
================

**概述**
-----------

Flash 可以包含应用程序，用户资源等多种不同类型的数据。为了更方便的使用 flash 资源，我们于是提出了分区表的解决方案。

分区表中的每个条目都包括以下几个部分： ``Name`` （分区名）、 ``Offset``  （分区相对 flash 基址的偏移量）、 ``Size`` （分区大小） 以及 ``Flag`` （暂时未使用）。

.. only:: w800

    W800 分区表的默认偏移地址为 0xE000，分区表的大小为 0x1000（一个sector，4k）。

.. attention:: 分区表的 Offset 和 size 需要 4K 对齐。请确保在配置分区表时，每个分区的 Offset 和 size 都是 4K 的整数倍。


**使用分区表**
----------------

在使用分区表时，可打开项目配置菜单 ``menuconfig`` ，并进入 **Partition Table** 菜单，在 **Partition Table** 栏下选择一种分区表方案。我们提供了三种预置的分区表和一种用户自定义的分区表，下面是预置的分区表，最后一种是用户自定义的分区表。

-  "Normal app, no OTA (components/partition_table/config/partition_table_normal.csv)"
-  "Large app, no OTA (components/partition_table/config/partition_table_large.csv)"
-  "Normal app, with OTA (components/partition_table/config/partition_table_with_ota.csv)"
-  "Custom partition table (<project directory>/partition_table_custom.csv)"

**内置分区表样式**
~~~~~~~~~~~~~~~~~~~~~~~

以下是 "partition_table_normal.csv" 选项的分区表信息摘要:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x100000,  0x0
  nvs,             0x1F0000,  0x8000,    0x0

以下是 "partition_table_large.csv" 选项的分区表信息摘要:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x1E1000,  0x0
  nvs,             0x1F0000,  0x8000,    0x0

以下是 "partition_table_with_ota.csv" 选项的分区表信息摘要:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x122000,  0x0
  app_ota,         0x131000,  0xAF000,   0x0
  user,            0x1E0000,  0x2000,    0x0
  nvs,             0x1F0000,  0x8000,    0x0

这三种分区表中，Normal 的分区不带 app_ota 和 user 分区，SDK 默认使用 Large 这个分区表，可以从 Kconfig 配置中指定其他分区表。


-  ft分区：芯片的出厂固件，启动时将默认加载这个文件。

-  bootloader：存放 bootloader 的启动程序，完成OTA升级文件的解压和固件更新。

-  partition_table：存放分区表信息，包括各个分区的名称，地址，大小和FLAG。

-  app：存放应用程序。

-  app_ota：存放 OTA 升级的应用程序压缩包。

-  user：用户自定义分区，目前 SDK 框架没有使用。

-  nvs：非易失存储分区，用于 NVS 模块的 key-value 数据存放，该分区需要一块做垃圾回收，所以 NVS 的大小需要大于等于2*0x1000。


.. attention:: ft，是芯片出厂固件，用户不能修改其中的内容。

.. attention:: ft，bootloader，partition_table这3个分区的配置 **不能修改** 。app，app_ota， NVS 这3个分区 **不能修改名称** ，可以修改地址和大小。



**用户自定义分区表**
~~~~~~~~~~~~~~~~~~~~~~

如果想要根据自己的程序使用灵活性更高的分区表，那么请在 ``menuconfig`` 的 **Partition Table** 菜单中选择 **Custom partition table** 选项，启用自定义的分区表功能。之后在工程根目录下添加一个 ``partition_table_custom.csv`` 文件即可，在编译时构建系统会自动识别并进行处理。

以下是 "Custom partition table" 选项的分区表信息摘要:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x1D1000,  0x0
  user,            0x1E0000,  0x2000,    0x0
  nvs,             0x1F0000,  0x8000,    0x0

-  字段之间的空格会被忽略，任何以 ``#`` 开头的行（注释）也会被忽略。
-  TXT 文件中的每个非注释行均为一个分区定义。
-  user 是一个用户自己配置的分区的示例，我们在使用时只需要合理的设置 offset 和 size 字段的内容即可。
-  ft, bootloader, partition_table 这3个分区不能改，app，nvs 这2个分区名称不能改，地址和大小可以修改。

在自定义分区表中添加自定义文件可参考 :ref:`在固件中添加自定义文件 <add_file_to_img>`

**烧写**
-------------------------

当分区表配置好之后，build 后会在 build/partition_table 下生成 partition_table.img 烧写文件， wm.py flash 命令会自动把该分区文件烧写到配置位置。

**擦除**
-------------------------

wm.py flash 命令加上 ``-e`` 选项可以在写入镜像前擦除整个 flash


**编译文件到自定义分区**
-------------------------

    请参考 :ref:`Build_system`
