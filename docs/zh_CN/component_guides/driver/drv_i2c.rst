
.. _i2c_drv:

I2C
=============

简介
-------------

    I2C 总线是一种简单、双向二线制同步串行总线。它只需要两根线即可在连接于总线上的器件之间传送信息。

    Inter-Integrated Circuit。所以中文应该叫集成电路总线，它是一种串行通信总线，使用多主从架构，由飞利浦公司在1980年代为了让主板、嵌入式系统或手机用以连接低速周边设备而发展起来的。

    主器件用于启动总线传送数据，并产生时钟以开放传送的器件，此时任何被寻址的器件均被认为是从器件。在总线上主和从、发和收的关系不是恒定的，而取决于此时数据传送方向。如果主机要发送数据给从器件，则主机首先寻址从器件，然后主动发送数据至从器件，最后由主机终止数据传送；如果主机要接收从器件的数据，首先由主器件寻址从器件，然后主机接收从器件发送的数据，最后由主机终止接收过程。在这种情况下,主机负责产生定时时钟和终止数据传送。


功能列表
-------------

    1. :ref:`配置 <i2c_set>`——pin 脚和速率配置。

    2. :ref:`初始化 <i2c_init>`——初始化驱动。

    3. :ref:`数据 Write <i2c_write>`——写数据到 I2C 设备上。

    4. :ref:`数据 Read <i2c_read>`——从 I2C 设备读数据。


功能概述
-------------

    **主设备** 只可作为主设备控制器使用。

    **速率可配** 100KHz~400KHz。

    **组合读写** 支持 I2C 读，写偏移地址或者命令后再读，写偏移地址或者命令后再写数据。

    **多从设备速率自适应** 不同速率从设备工作支持。


.. _i2c_set:

配置
-------------

    I2C 需要在设备表中配置，根据实际使用 Board 的 pin 脚，配置时钟和数据线。其他可以使用 SDK 中的默认配置。

    .. note::
        I2C 的 SCl 和 SDA 这两条 pin 脚，设备表中默认配置了上拉，一般来说 I2C 的上拉电阻需要在某个区间内，不能太大，也不能太小，
        这是由从设备决定的，可以从 I2C 从设备手册参数中计算出来。


I2C 地址
-------------

    I2C 从设备地址，驱动接口使用 7 位地址，不包括读写位，在调用读写接口时，底层驱动会左移一位再加上读写位。


主要功能
-------------

.. _i2c_init:

初始化 I2C
^^^^^^^^^^^^^

    使用 I2C 之前需调用 ``wm_drv_i2c_init()`` 函数为I2C分配资源，利用 ``wm_device_t`` ，示例：

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");;
        wm_drv_i2c_init("i2c0");


    第一个参数指定设备名称，名称在设备表中定义，目前只有一个 I2C 主设备，只能填 "i2c0"

    .. warning:: I2C 初始化后，如果没有调用 ``wm_drv_i2c_deinit`` , 再次调用 ``wm_drv_i2c_init`` 会返回 ``NULL`` 。

.. _i2c_write:

Write
^^^^^^^^^^^^^

    发送数据准备好后，调用函数 ``wm_drv_i2c_write`` ，调用该接口把要发送的数据发给 I2C 从设备

    该接口有2个写数据，分别用于2种场景:

    - 场景1

        只需要写寄存器地址或者命令，这时，只传第一个写数据 buf 和 buf_size，data 参数传 NULL,data_size 传0即可。

    - 场景2

        需要传从设备内部地址或者命令，这时，第一个数据 buf 和 buf_size 传内部地址或者命令，第二个参数 data 和 data_size 传要写的数据。

    下面是读 EEPROM 的实例：

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");;

        wm_drv_i2c_config_t config = {
            .addr     = 0x50,
            .speed_hz = 400000,
        };

        uint8_t sub_addr = 5;

        /*写5个字节到EEPROM偏移为5的地方*/
        wm_drv_i2c_write(i2c_dev,&config,&sub_addr,1,"hello",5);

.. _i2c_read:

read 读数据
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    write read 方式使用接口函数 ``wm_drv_i2c_read`` 从 I2C 从设备中读数据，该接口需要写从设备的内部地址，再读指定长度的数据。


    下面是使用 write,read 方式读 EEPROM 的实例：

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");;

        wm_drv_i2c_config_t config = {
            .addr     = 0x50,
            .speed_hz = 400000,
        };

        uint8_t sub_addr = 5;
        Uint8_t buf[5];

       /*从EEPROM从设备偏移为5的地方读取5个字节*/
        wm_drv_i2c_read(i2c_dev,&config,&sub_addr,1,buf,5);


注意事项
-------------

.. warning:: I2C 使用的地址是7位地址，不包括读写位，在调用接口时，底层会左移一位再加上读写位。


应用实例
-------------

    使用 I2C 基本示例请参照 :ref:`examples/peripheral/i2c<peripheral_example>`


其他相关文档
-------------
    :ref:`eeprom`
    :ref:`eeprom_dev`

API 参考
-------------
    查找 I2C 相关 API 请参考：

    :ref:`label_api_i2c`
