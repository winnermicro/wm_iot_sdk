
.. _partition_table:

**Partition Table Mechanism**
================================

**Overview**
----------------

Flash memory can contain various types of data, including applications,user resources. To facilitate the efficient use of flash resources, we propose a partition table solution.

Each entry in the partition table includes the following components:  ``Name`` (partition name),  ``Offset``  (offset relative to the flash base address), ``Size`` (partition size), and ``Flag`` (currently unused).

.. only:: w800

    The default offset address of the W800 partition table is 0xE000, and the size of the partition table is 0x1000 (one sector, 4k).

.. attention:: 
   
    The offset and size of the partition table need to be 4K-aligned. Please ensure that when configuring the partition table, the offset and size of each partition are both integer multiples of 4K.

**Using the Partition Table**
--------------------------------

The simplest way to use the partition table is to open the project configuration menu ``menuconfig`` and enter the **Partition Table** menu. In the **Partition Table** ection, select a partition table. We provide three preset partition tables and one user-defined partition table. Below are the preset partition tables, with the last one being the user-defined partition table.

-  "Normal app, no OTA (components/partition_table/config/partition_table_normal.csv)"
-  "Large app, no OTA (components/partition_table/config/partition_table_large.csv)"
-  "Normal app, with OTA (components/partition_table/config/partition_table_with_ota.csv)"
-  "Custom partition table (<project directory>/partition_table_custom.csv)"

**Built-in Partition Table Styles**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Here is a summary of the partition table information for the "partition_table_normal.csv" option:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x100000,  0x0
  nvs,             0x1F0000,  0x8000,    0x0

Here is a summary of the partition table information for the "partition_table_large.csv" option:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x1E1000,  0x0
  nvs,             0x1F0000,  0x8000,    0x0

Here is a summary of the partition table information for the "partition_table_with_ota.csv" option:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x122A00,  0x0
  app_ota,         0x131A00,  0xAE600,   0x0
  user,            0x1E0000,  0x2000,    0x0
  nvs,             0x1F0000,  0x8000,    0x0

Among these three partition tables, the Normal partition does not include app_ota and user partitions. The SDK defaults to using the Large partition table, but other partition tables can be specified from the Kconfig configuration.

-  ft partition: Factory firmware for the chip, which will be loaded by default at startup.

-  bootloader: Stores the bootloader program, which completes the decompression and firmware update of the OTA upgrade file.

-  partition_table: Stores partition table information, including the name, address, size, and FLAG of each partition.

-  app: Stores the application program.

-  app_ota: Stores the OTA upgrade application package.

-  user: User-defined partition, currently not used by the SDK framework.

-  Non-volatile storage partition for storing key-value data of the NVS module. Since this partition requires a block for garbage collection, the size of nvs needs to be at least 2*0x1000.


.. attention:: ft is the factory firmware for the chip, and users cannot modify its content.

.. attention:: The configuration of the ft, bootloader, and partition_table partitions **cannot be modified**. The app, app_ota, and nvs partitions **cannot change names**, but their addresses and sizes can be modified.


**User-Defined Partition Table**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want a partition table with higher flexibility for your program, select the **Custom partition table** option in the ``menuconfig`` under the **Partition Table** menu. Then, add a ``partition_table_custom.csv`` file in the root directory of the project.The build system will automatically recognize and process it during compilation.


Here is a summary of the partition table information for the "Custom partition table" option:

::

  # name,          offset,    size,      flag
  ft,              0x0,       0x2000,    0x0
  bootloader,      0x2000,    0xC000,    0x0
  partition_table, 0xE000,    0x1000,    0x0
  app,             0xF000,    0x1D1000,  0x0
  user,            0x1E0000,  0x2000,    0x0
  nvs,             0x1F0000,  0x8000,    0x0

-  Spaces between fields will be ignored, and any line beginning with ``#`` (comments) will also be ignored.
-  Each non-comment line in the TXT file defines a partition.
-  The user partition is an example of a user-configured partition. When using it, you only need to set the offset and size fields appropriately.
-  The ft, bootloader, and partition_table partitions cannot be changed. The names of the app and nvs partitions cannot be changed, but their addresses and sizes can be modified.

**Flashing**
-----------------

After configuring the partition table, the build process will generate a partition_table.img flashing file in the build/partition_table directory. The wm.py flash command will automatically flash this partition file to the configured location.

**Erasing**
---------------------
The wm.py flash command with the  ``-e`` option can erase the entire flash before writing the image.


**Compiling Files to Custom Partitions**
---------------------------------------------

Please refer to :ref:`Build_system`
