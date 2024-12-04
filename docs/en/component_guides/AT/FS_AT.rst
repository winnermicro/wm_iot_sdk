
.. _FS-AT:
.. |Equipment-Name| replace:: w800

*********************
FS AT Command Set
*********************

-  :ref:`AT+FSMOUNT <cmd-FSMOUNT>`: Mount/Unmount File System
-  :ref:`AT+FS <cmd-FS>`: File system operations

.. _cmd-FSMOUNT:

:ref:`AT+FSMOUNT <FS-AT>`: Mount/Unmount File System
------------------------------------------------------------

This command supports the mounting and unmounting of three types of storage disks: SDMMC, internal Flash, and external Flash. The type of disk to be mounted can be selected through the menuconfig configuration, with SDMMC being the default option.

Environmental Requirements:

1. SDMMC Disk
   - Requires proper configuration of the wm_drv_sdh_sdmmc driver.
   - Requires a hardware environment that supports SD cards, with an SD card inserted.

2. Internal Flash Disk
   - Requires proper configuration of the wm_drv_flash driver.
   - Use user-defined partition partition_table_custom.csv and add CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME (default name is fatfs) partition , partition size must be at least 0x18000 (96KB), otherwise formatting will fail

3. External Flash Disk
   - Requires proper configuration of the wm_drv_flash driver.
   - Requires a hardware environment that supports external Flash devices, with an external Flash device connected.
   - Requires proper configuration of the following parameters:

      - CONFIG_FATFS_EXTERNAL_FLASH_DEVICE_NAME: The name of the external External Flash Device Name
      - CONFIG_FATFS_EXTERNAL_FLASH_START_ADDRESS: The starting address of the external Flash for FATFS use.
      - CONFIG_FATFS_EXTERNAL_FLASH_SIZE: Size for FATFS on external Flash, partition size must be at least 0x18000 (96KB), otherwise formatting will fail


Set Command
^^^^^^^^^^^^^^

**Command:**

::

    AT+FSMOUNT=<mount>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^^^^

-  **<mount>**:

   -  0: Unmount FS file system
   -  1: Mount FS file system

Description
^^^^^^^^^^^^^^^^

-  After completing the :ref:`AT+FS <cmd-FS>` file system operation, it is recommended to use :ref:`AT+FSMOUNT=0 <cmd-FSMOUNT>` command to unmount the file system to free up RAM space.

Example
^^^^^^^^^^^^

::

    // Manually unmount file system
    AT+FSMOUNT=0

    // Manually mount file system
    AT+FSMOUNT=1


.. _cmd-FS:

:ref:`AT+FS <FS-AT>`: File System Operations
------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+FS=<type>,<operation>,<filename>[,<offset>,<length>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^^^^^^^^

-  **<type>**: Currently only supports FATFS

   -  0: FATFS

-  **<operation>**:

   -  0: Delete file
   -  1: Write file
   -  2: Read file
   -  3: Query file size
   -  4: List files in the path, currently only supports "/" root directory

-  **<filename>**: File name (limited to 8 characters, can be configured through menuconfig ``File name length configuration`` to support long file names); when operation is 4, it indicates a directory, currently only supports "/" root directory

-  **<offset>**: Offset address, set only for read/write operations

-  **<length>**: Length, set only for read/write operations

Description
^^^^^^^^^^^^^^^^^

-  This command will automatically mount the file system. After completing the :ref:`AT+FS <cmd-FS>` file system operation, it is recommended to use :ref:`AT+FSMOUNT=0 <cmd-FSMOUNT>` command to unmount the file system to free up RAM space.
-  If the length of data read is greater than the actual file size, only the actual length of data will be returned.
-  When ``<operation>`` is ``write``, the system will return ``>`` after receiving this command, and you need to input the data to be written within 6 seconds, with the data length matching ``<length>``.
-  When mounting a disk, if it is not formatted, it will be formatted automatically. FATFS will automatically select the most suitable file system type based on disk capacity, as follows:
   - FAT12: Suitable for small capacity storage (≤ 2MB, maximum 4,085 clusters, MAX_FAT12 = 0xFF5 = 4,085 clusters = 2,042,560 Bytes ≈ 2MB)
   - FAT16: Suitable for medium capacity storage (≤ 32MB, maximum 65,493 clusters, MAX_FAT16 = 0xFFF5 = 65,493 clusters = 33,516,032 Bytes ≈ 32MB)
   - FAT32: Suitable for large capacity storage (≤ 128GB, maximum 268,435,445 clusters, MAX_FAT32 = 0x0FFFFFF5 = 268,435,445 clusters = 137,438,947,840 Bytes ≈ 128GB)
   - EXFAT: Suitable for extra large capacity storage (≤ 1TB, maximum 2,147,483,645 clusters, MAX_EXFAT = 0x7FFFFFFD = 2,147,483,645 clusters = 1,099,511,626,240 Bytes ≈ 1TB)
   - Note: The choice of file system type will affect the maximum value of ``<offset>`` and the subsequent use of APIs.

Example
^^^^^^^^^^^^^^^^

.. code-block:: text

    // Delete a file
    AT+FS=0,0,"filename"

    // Write 10 bytes at offset 0 of a file
    AT+FS=0,1,"filename",0,10

    // Read 10 bytes from offset 0 of a file
    AT+FS=0,2,"filename",0,10

    // Query the size of a file
    AT+FS=0,3,"filename"

    // List all files in the root directory
    AT+FS=0,4,"."