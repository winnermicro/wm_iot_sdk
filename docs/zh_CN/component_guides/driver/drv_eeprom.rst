
.. _eeprom:

EEPROM
=============

简介
-------------

    EEPROM (Electrically Erasable Programmable read only memory) 是指带电可擦可编程只读存储器。是一种掉电后数据不丢失的存储芯片。


功能列表
-------------

    1. :ref:`初始化 <eeprom_init>`——初始化驱动。

    2. :ref:`获取大小 <eeprom_get_size>`——获取 EEPROM 大小。

    3. :ref:`数据 Read <eeprom_read>`——从 EEPROM 设备读数据。

    4. :ref:`数据 write <eeprom_write>`——写数据到 EEPROM。

    5. :ref:`设置写保护 <set_wp>`——设置写保护。


功能概述
-------------

    **EEPROM 设备配置** 在设备表中配置 EEPROM 型号，总大小，页大小，写入，写保护控制 pin 脚，时间等参数。

    **随机地址读写** 支持存储区域内部的任意偏移，任意长度的读写。

    **写保护** 支持写保护功能。


.. _eeprom_set:

配置
-------------

    EEPROM 驱动使用前，需要在设备表中配置具体使用的 EEPROM 型号，地址，大小，每页大小，写入页面的时间，写保护功能，其中写保护不是必须。

    .. note::
        SDK 中写好的 EEPROM 驱动，下面这些型号有测试过：

        NV24C02

        NV24C04

        NV24C16

        ST24C02WP

        AT24C16

        AT24C32

        AT24C256

        其他型号的 EEPROM，不一定能支持。如果既有驱动无法支持的 EEPROM 型号，需要根据型号自己开发 EEPROM 的驱动或者直接调用 I2C 驱动接口读写。

新EEPROM支持
-------------

    SDK 中已经包含 NV24C0X 型号的驱动，实际测试也可以支持其他如 ST24C0X，AT24C0X 等型号，当有新的 EEPROM 型号需要支持时，可以先验证既有的 EEPROM 驱动是否支持，如果不支持，可以写新 EEPROM 驱动。

    怎么添加新型号支持呢，根据新型号的 EEPROM 芯片手册，在设备表中修改原来的 ``eeprom0`` 设备或者增加一个 ``eeprom?`` 设备，主要需要根据手册填写下面这些选项：

    .. code:: c

        .speed_hz          = 400000,
        .size              = 256,
        .i2c_addr          = 0x52,
        .page_size         = 16,
        .addr_width        = 8,
        .read_only         = 0,
        .max_write_time_ms = 5,

    read_only 是驱动控制只读的开关，其他参数需要根据 EEPROM 手册来填写。

    设备使用驱动 API 也需要配置，详情可参照设备表配置。

EEPROM 地址
-------------

    EEPROM 地址需要根据具体 EEPROM 型号和硬件 Borad 连线方式共同决定，详情需要参照 EEPROM 的文档和 Board 电路原理图。

    通常7位地址中，高4位为 ``1 0 1 0`` ,接下来的3位通常叫 ``A2, A1, A0``, 这3位可以连接到 Borad 上的 GND 或者 VDD 高电平上，通常悬空。悬空和接地都是0， 接到VCC上就是1 。如果都悬空，
    则 EEPROM 地址为 0b1010000,I2C 地址为 0x50, 如果 A1 接 VDD,A2,A0 悬空，则地址为 0b1010010,I2C 地址为 0x52


主要功能
-------------

.. _eeprom_init:

初始化 EEPROM
^^^^^^^^^^^^^

    使用 EEPROM 时前需调用 ``wm_drv_eeprom_init()`` 函数为 EEPROM 分配资源，利用 ``wm_device_t`` ，示例：

    .. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");
        wm_drv_eeprom_init("eeprom0");


    第一个参数指定设备名称，名称在设备表中定义，和设备表中的名称要保持一致。

    .. warning:: EEPROM 初始化后，如果没有调用 ``wm_drv_eeprom_deinit`` , 再次调用 ``wm_drv_eeprom_init`` 会返回 ``NULL`` 。


.. _eeprom_get_size:

获取大小
^^^^^^^^^^^^^

    获取 EEPROM 的大小可调用 ``wm_drv_eeprom_get_size()`` 函数，需传入 EEPROM 的设备句柄。

    下面是获取大小的实例：

    .. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");

        wm_drv_eeprom_init("eeprom0");

        /*获取eeprom0的大小*/
        wm_drv_eeprom_get_size(eeprom_dev);


.. _eeprom_write:

Write
^^^^^^^^^^^^^

    发送数据准备好后，调用函数 ``wm_drv_eeprom_write`` ，调用该接口把要发送的数据发给 EEPROM 设备

    下面是写 EEPROM 的实例：

    .. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");

        wm_drv_eeprom_init("eeprom0");

        /*写5个字节到eeprom偏移为2的地方*/
        wm_drv_eeprom_write(eeprom_dev,2,"hello",5);


.. _eeprom_read:

Read
^^^^^^^^^^^^^^^^^^^^

    Read 使用接口函数 ``wm_drv_eeprom_read`` 读取数据，该接口会从 EEPROM 设备读数据。

    下面是读 EEPROM 的实例：

    .. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");;
        uint8_t buf[5];

        wm_drv_eeprom_init("eeprom0");

        /*从EEPROM从设备偏移为2的地方读取5个字节*/
        wm_drv_eeprom_read(eeprom_dev,2,buf,5);


.. _set_wp:

Set Write Protection
^^^^^^^^^^^^^^^^^^^^^^^^^

    设置写保护，需要在设备表中配置 GPIO ，同时需要 EEPROM 本身支持写保护功能，通常把 EEPROM 的 WP 脚拉成高电平，设备处于写保护状态，I2C 无法把数据写入到 EEPROM ,如果 WP 脚拉成低电平，则设备处于可写入状态。


注意事项
-------------

.. note:: EEPROM 配置时，总大小，页大小，写入时间都会因型号而定，需要根据确切的手册去填写,否则可能读写不正常。

.. warning:: 即使设置了写保护，wm_drv_eeprom_write 接口中会自动关闭写保护再写,但 I2C 接口无法直接写入。


应用实例
-------------

    使用 EEPROM 基本示例请参照 :ref:`examples/peripheral/eeprom<peripheral_example>`


其他相关文档
-------------
    :ref:`i2c`
    :ref:`eeprom_dev`

API 参考
-------------
    查找 EEPROM 相关 API 请参考：

    :ref:`label_api_eeprom`