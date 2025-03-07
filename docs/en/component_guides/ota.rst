
.. _ota:

OTA
=============

Introduction
---------------------

OTA (Over The Air) is a method for remotely updating device firmware or software through wireless communication technology.


Feature List
----------------
1. :ref:`Full Update <ota_full_update>`: Download and install the complete firmware on the device.
2. :ref:`Compressed Update <ota_compres_update>`: Reduce the size of the update package through  XZ compression technology, saving storage space, improving download success rate, and decreasing upgrade time and data usage.


OTA Feature Overview 
-----------------------------
The OTA upgrade mechanism allows the device to self-update by receiving data (e.g., via Wi-Fi or Bluetooth) during normal firmware operation. When using OTA, you need to configure the device's :ref:`partition table <partition_table>` and choose a partition table with OTA support or a custom OTA partition table.

Once the OTA feature is activated, the downloaded update package will be stored in the ``app_ota partition``, awaiting decompression and firmware upgrade by the  ``bootloader``  after the device restarts.


OTA Upgrade Configuration
------------------------------

Firmware Partition Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By default, there is only one OTA partition. Please refer to the :ref:`partition table <partition_table>` for settings.
During the OTA upgrade process, the new firmware will be stored in the ``app_ota partition``. After the firmware download is complete, it will be decompressed and upgraded. The default OTA partition table configuration is as follows:

    ::

        # name,          offset,    size,      flag
        ft,              0x0,       0x2000,    0x0
        bootloader,      0x2000,    0xC000,    0x0
        partition_table, 0xE000,    0x1000,    0x0
        app,             0xF000,    0x122000,  0x0
        app_ota,         0x131000,  0xAF000,   0x0
        user,            0x1E0000,  0x2000,    0x0
        nvs,             0x1F0000,  0x8000,    0x0

.. hint::

   The conservative compression rate of XZ algorithm binary files is about 40%, so it is recommended that the size of the ``app_ota partition`` be configured to be ≤ 60% of the ``app partition``.  An overly small ``app partition`` will result in  insufficient space to store the decompressed firmware.


Bootloader Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Bootloader runs when the device starts up and is responsible for decompressing OTA upgrade files and performing firmware updates. When burning the firmware, the Bootloader will be automatically written to the Flash.


Firmware Package Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Use the developer tool to compile the latest firmware file (.img file), then upload the compiled firmware file to the OTA server or a network-accessible storage location (such as FTP server, HTTP server, etc.). There are two types of generated OTA img files, as shown in the table below:

=================== ===========================
Filename            Type
=================== ===========================
``xxxxx.img``        Full update package
``xxxxx_ota.img``    Compressed update package
=================== ===========================

During the download process, the device will perform data verification (such as signature and checksum) to ensure data integrity and security. After the download is complete, the device will verify the integrity and authenticity of the firmware package before starting the installation.

**Introduction to OTA Header Format**

====================================== =============================================
Struct Member                          Description
====================================== =============================================
``uint32_t magic_no``                  Magic number for identifying OTA firmware
``wm_img_attr_t img_attr``             Bit field of firmware attributes
``uint32_t img_addr``                  Address of the executable firmware
``uint32_t img_len``                   Length of the binary firmware
``uint32_t img_header_addr``           Address of the image header
``uint32_t upgrade_img_addr``          Address of the upgrade image
``uint32_t org_checksum``              Original checksum of the firmware
``uint32_t upd_no``                    Update version tracking number
``unsigned char ver[16]``              Version string, 16 bytes long
``uint32_t log_level : 3``             Log level
``uint32_t skip_esc_check : 1``        Skip escape check
``uint32_t skip_upgrade_check : 1``    Skip upgrade check
``uint32_t skip_integrity_check : 1``  Skip firmware integrity check
``uint32_t _reserved0 : 26``           Reserved for future use
``uint32_t _reserved1``                Reserved for future use
``struct wm_ota_header_t *next``       Pointer to the next OTA header
====================================== =============================================

Network Communication Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

During the OTA upgrade process, you need to connect to  a WiFi network to communicate with the OTA server.  Firmware files are downloaded from the server using protocols such as HTTP. During the download process, the device will perform data verification (such as signature and checksum) to ensure data integrity and security. If not connected to the network, the OTA request will directly return a failure.
OTA will resume transmission upon network timeout until the upgrade is completed or the timeout period ``CONFIG_OTA_RETRY_TIMEOUT`` is reached; for non-network timeout errors, it will fail directly without retrying.

Main Features
---------------

.. _ota_full_update:

Full Update
^^^^^^^^^^^^^^^^^^^^

   **1. Starting Conditions**

    - Configure the OTA partition table: The device's partition table needs to be configured to support the OTA format.
    - The device connected to the network: The device must be connected to the internet via Wi-Fi.
    - The OTA server accessible: The device can access the configured OTA server (supporting HTTP, FTP, etc.).
    - Firmware version verification: The ``xxxx.img file`` can  be normally verified as a full update file to decide whether an upgrade is needed.
    
   **2. Related Sequence API**

    -  Open the project configuration menu ``menuconfig`` to configure the partition table that supports OTA
    -  Call ``wm_wifi_init()`` to initialize WiFi
    -  Use ``wm_wifi_set_config()`` to configure WiFi parameters
    -  Use ``wm_wifi_connect()`` to connect to WiFi
    -  Call ``wm_ota_http_update()`` to perform OTA upgrades.

   **3. Result**

    - OTA full update successful


.. _ota_compres_update:

Compressed Update
^^^^^^^^^^^^^^^^^^^^

    **1. Starting Conditions**

    - Configure the OTA partition table: The device's partition table needs to be configured to support the OTA format.
    - The device is connected to the network: The device must be connected to the internet via Wi-Fi or Ethernet.
    - The OTA server is accessible: The device can access the configured OTA server (supporting HTTP, FTP, etc.).
    - Firmware version verification: The ``xxxx_ota.img`` can be normally verified as a  compressed upgrade file to determine if an upgrade is needed.
    
    **2. Related Sequence API**

    -  Open the project configuration menu ``menuconfig`` to configure the OTA-supported partition table.
    -  Call ``wm_wifi_init()`` to initialize WiFi
    -  Use ``wm_wifi_set_config()`` to configure WiFi parameters
    -  Use ``wm_wifi_connect()`` to connect to WiFi
    -  Call ``wm_ota_http_update()`` for OTA upgrade

    **3. Result**

    - OTA compressed update successful


.. hint::

    - Ensure that **the size of the firmware file does not exceed the available storage space limit of the OTA partition**.
    - During the OTA upgrade, **maintain a stable network connection** to avoid upgrade failure.
    - During the OTA upgrade, **do not disconnect the power** to avoid upgrade failure.


Application Example
-------------------------

    For basic OTA examples, please refer to :ref:`examples/ota <ota_example>`


API Reference
---------------------

    For OTA-related APIs, please refer to:

    :ref:`label-api-ota_ops` and :ref:`label-api-ota_http`

OTA's meunconfig configuration
--------------------------------

The main configurations are as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration Name
    - Config Discriptors
    - Default Values

  * - CONFIG_COMPONENT_OTA_ENABLED
    - Whether to enable OTA components
    - N

  * - CONFIG_COMPONENT_OTA_HTTP_ENABLED
    - Whether to enable OTA HTTP
    - N

  * - CONFIG_OTA_BLOCK_SIZE
    - Configure the OTA block size
    - 4096

  * - CONFIG_OTA_RETRY_TIMEOUT
    - The retry timeout period is specified
    - 120000

  * - CONFIG_OTA_SOCKET_RECONNECT_TIMES 
    - The number of ota socket reconnections was set
    - 5