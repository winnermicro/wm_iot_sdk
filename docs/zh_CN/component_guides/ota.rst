
.. _ota:

OTA
=============

简介
-------------

    OTA（Over The Air，空中升级）是一种通过无线通信技术实现远程更新设备固件或软件的方法。


功能列表
------------
    1. :ref:`全量升级 <ota_full_update>`：将完整的固件下载并安装到设备上。
    2. :ref:`压缩升级 <ota_compres_update>`：通过 XZ 压缩技术减少升级包的大小，节省存储空间，并提高下载成功率，减少升级时间和数据流量消耗。


OTA 功能概述
---------------
    OTA 升级机制允许设备在固件正常运行期间，通过接收数据（例如通过 Wi-Fi 或蓝牙）进行自我更新。使用 OTA 时，需配置设备的 :ref:`分区表 <partition_table>` , 并选择带有 OTA 的分区表或用户自定义的 OTA 分区表。

    OTA 功能启动后，下载的升级包将存储在 ``app_ota 分区`` 中，等待设备重启后由 ``bootloader`` 进行解压和固件升级。


OTA 升级配置
---------------

固件分区配置
^^^^^^^^^^^^^^^

    默认只有一个 OTA 分区，请参考 :ref:`分区表 <partition_table>` 进行设置。
    在 OTA 升级过程中，新的固件会存储在 ``app_ota 分区`` 中，待固件下载完成后再进行解压和升级。默认 OTA 分区表配置如下：

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

    XZ 算法二进制文件保守压缩率在 40% 左右，因此建议 ``app_ota 分区`` 配置的大小 ≤ ``app 分区`` 的 60%，过小的 ``app 分区`` 会导致无空间存放解压后的固件。


引导加载程序配置
^^^^^^^^^^^^^^^^^^^^

    引导加载程序（Bootloader），它在设备启动时运行，负责解压 OTA 升级文件并进行固件更新。在进行固件烧录时，Bootloader 会自动写入到 Flash 中。

固件包配置
^^^^^^^^^^^^^^^^^^^^

    使用开发者工具编译最新的固件文件（.img 文件），然后将编译好的固件文件上传到 OTA 服务器或通过网络可访问的存储位置（如 FTP 服务器、HTTP 服务器等。生成的 OTA img 文件有两个类型，下表是生成 OTA 文件说明：

    =================== =====================
    文件名               类型
    =================== =====================
    ``xxxxx.img``       全量升级包
    ``xxxxx_ota.img``   压缩升级包
    =================== =====================

    在下载过程中，设备会进行数据校验（如签名和原始校验和）以确保数据的完整性和安全性。下载完成后，设备会验证固件包的完整性和真实性，确认无误后再开始安装。

    **OTA header 格式简介**

    ====================================== =======================================
    结构体成员                              说明
    ====================================== =======================================
    ``uint32_t magic_no;``                 用于识别 OTA 固件的魔术字
    ``wm_img_attr_t img_attr``             固件属性的位字段
    ``uint32_t img_addr``                  执行固件的地址
    ``uint32_t img_len``                   二进制固件长度
    ``uint32_t img_header_addr``           图文件头部的地址
    ``uint32_t upgrade_img_addr``          升级图文件的地址
    ``uint32_t org_checksum``              固件的原始校验和
    ``uint32_t upd_no``                    更新版本跟踪号
    ``unsigned char ver[16]``              版本字符串，长度为 16 字节。
    ``uint32_t log_level : 3``             日志级别
    ``uint32_t skip_esc_check : 1``        跳过 ESC 检查
    ``uint32_t skip_upgrade_check : 1``    跳过升级检查
    ``uint32_t skip_integrity_check : 1``  跳过固件完整性检查
    ``uint32_t _reserved0 : 26``           保留用于将来使用
    ``uint32_t _reserved1``                保留用于将来使用
    ``struct wm_ota_header_t *next``       指向下一 OTA header 的指针
    ====================================== =======================================


网络通信配置
^^^^^^^^^^^^^^^

    在进行 OTA 升级过程中，需要连接 WiFi 与 OTA 服务器进行通信。通过 HTTP 等协议从服务器下载固件文件，下载过程中设备会进行数据校验（如签名和校验和）以确保数据的完整性和安全性。如果没有连接至网络，在请求 OTA 时会直接返回失败。
    OTA 在网络超时时会进行续传直到升级完成或达到超时时间 ``CONFIG_OTA_RETRY_TIMEOUT``; 对于非网络超时错误将直接失败而不进行重试。

主要功能
---------------

.. _ota_full_update:

全量升级
^^^^^^^^^^^^^^^

    **1. 起始条件**

    - 配置 OTA 的分区表：设备的分区表需要配置为支持 OTA 的格式。
    - 设备连接到网络：设备必须通过 Wi-Fi 连接到互联网。
    - OTA 服务器可访问：设备能够访问配置好的 OTA 服务器（支持 HTTP、FTP 等协议）。
    - 固件版本验证：能正常验证 ``xxxx.img`` 作为全量升级文件， 以决定是否需要升级。
    
    **2. 相关时序 API**

    -  打开项目配置菜单 ``menuconfig`` 配置支持 OTA 的分区表
    -  调用 ``wm_wifi_init()`` 初始化 WiFi
    -  使用 ``wm_wifi_set_config()`` 配置 WiFi 参数
    -  使用 ``wm_wifi_connect()`` 连接至 WiFi
    -  调用 ``wm_ota_http_update()`` 进行 OTA 升级

    **3. 结果**

    - OTA 全量升级成功


.. _ota_compres_update:

压缩升级
^^^^^^^^^^^^^^^

    **1. 起始条件**

    - 配置 OTA 的分区表：设备的分区表需要配置为支持 OTA 的格式。
    - 设备连接到网络：设备必须通过 Wi-Fi 或以太网连接到互联网。
    - OTA 服务器可访问：设备能够访问配置好的 OTA 服务器（支持 HTTP、FTP 等协议）。
    - 固件版本验证：能正常验证 ``xxxx_ota.img`` 作为全量升级文件， 以决定是否需要升级。
    
    **2. 相关时序 API**

    -  打开项目配置菜单 ``menuconfig`` 配置支持 OTA 的分区表
    -  调用 ``wm_wifi_init()`` 初始化 WiFi
    -  使用 ``wm_wifi_set_config()`` 配置 WiFi 参数
    -  使用 ``wm_wifi_connect()`` 连接至 WiFi
    -  调用 ``wm_ota_http_update()`` 进行 OTA 升级

    **3. 结果**

    - OTA 压缩升级成功


.. hint::

    - 请确保 **固件文件的大小不超过 OTA 分区可用存储空间** 限制。
    - OTA 升级过程中需要 **保持网络连接稳定**，以避免升级失败。
    - OTA 升级过程中请 **不要断开电源**，以避免升级失败。


应用实例
---------

    使用 OTA 基本示例请参照 :ref:`examples/ota <ota_example>`


API 参考
---------

    查找 OTA 相关 API 请参考:

    :ref:`label-api-ota_ops` 和 :ref:`label-api-ota_http`

OTA 的 meunconfig 配置
------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_COMPONENT_OTA_ENABLED
     - 是否启用 OTA 组件
     - N

   * - CONFIG_COMPONENT_OTA_HTTP_ENABLED
     - 是否启用 OTA HTTP
     - N

   * - CONFIG_OTA_BLOCK_SIZE
     - 配置 OTA 块大小
     - 4096

   * - CONFIG_OTA_RETRY_TIMEOUT
     - 配置重试超时时间
     - 120000

   * - CONFIG_OTA_SOCKET_RECONNECT_TIMES 
     - 配置 ota socket 重连次数
     - 5