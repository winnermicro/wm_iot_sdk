.. _drv_crc:

CRC
=============

简介
-------------

    CRC 即循环冗余校验码（Cyclic Redundancy Check）：是数据通信领域中最常用的一种查错校验码，其特征是信息字段和校验字段的长度可以任意选定。循环冗余检查（CRC）是一种数据传输检错功能，对数据进行多项式计算，并将得到的结果附在帧的后面，接收设备也执行类似的算法，以保证数据传输的正确性和完整性。

    W80x支持 CRC8、 CRC16_MODBUS、 CRC16_CCITT 和 CRC32 四种模式。支持支持输入/输出反向。SHA1/MD5/CRC 支持连续多包加密，且支持交替运算。


功能列表
-------------

    1. :ref:`初始化 <crc_init>`——初始化驱动。
    2. :ref:`配置ctx <crc_cfg>`——根据 CRC 计算模式和输入输出是否反向配置上下文 ctx。
    3. :ref:`追加数据 <crc_update>`——追加数据到 ctx 并计算 CRC。
    4. :ref:`读取CRC <crc_final>`——读取 ctx 中的 CRC 值。



.. _crc_init:

初始化驱动
------------------

    调用 ``wm_drv_crc_init()`` ，得到指向 CRC 模块的设备指针，只有得到了这个指针才能调用其他的 CRC 模块相关的 API。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_crc_init("crc");

    函数参数指定设备名称，名称在设备表中定义。

.. _crc_cfg:

配置 ctx
------------

    调用 ``wm_drv_crc_cfg()`` ，根据 CRC 计算模式和输入输出是否反向配置上下文 ctx。示例：

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_crc_cfg_t ctx;
        wm_device_t *dev;

        /*initialize crc*/
        dev = wm_drv_crc_init("crc");
        wm_drv_crc_cfg(dev, &ctx, init_value, WM_GPSEC_CRC8, WM_GPSEC_CRC_REVERSE_NONE);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：初始值

    第四个参数是：CRC 计算模式，0 到 3 分别表示CRC8、 CRC16_MODBUS、 CRC16_CCITT 和 CRC32

    第五个参数是：输入和输出是否反向，0 表示不反向，1 表示输出反向，2 表示输入反向，3 表示输入输出均反向


.. _crc_update:

追加数据
------------

    调用 ``wm_drv_crc_update()`` ，根据上下文 ctx 计算 CRC，将结果保存到 ctx 中，该接口可连续调用多次追加数据。支持不同的上下文 ctx 交替执行。示例：

    .. code:: c

        uint32_t init_value = 0;
        uint8_t *in     = NULL;
        int i;
        wm_drv_crc_cfg_t ctx;
        wm_device_t *dev;

        /*initialize crc*/
        dev = wm_drv_crc_init("crc");

        in = malloc(1024);

        for (i = 0; i < 1024; i++) {
            in[i] = i;
        }
        wm_drv_crc_cfg(dev, &ctx, init_value, WM_GPSEC_CRC8, WM_GPSEC_CRC_REVERSE_NONE);
        wm_drv_crc_update(dev, &ctx, in, 512);
        wm_drv_crc_update(dev, &ctx, in + 512, 512);



    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：数据指针

    第四个参数是：数据长度，字节数，范围是 1-65532

.. _crc_final:

读取 CRC
------------

    调用 ``wm_drv_crc_final()`` 函数读取上下文 ctx 中的 CRC 值，示例：

    .. code:: c

        uint32_t init_value = 0;
        uint8_t *in     = NULL;
        int i;
        wm_drv_crc_cfg_t ctx;
        wm_device_t *dev;
        uint32_t crcvalue = 0;

        /*initialize crc*/
        dev = wm_drv_crc_init("crc");

        in = malloc(1024);

        for (i = 0; i < 1024; i++) {
            in[i] = i;
        }
        wm_drv_crc_cfg(dev, &ctx, init_value, WM_GPSEC_CRC8, WM_GPSEC_CRC_REVERSE_NONE);
        wm_drv_crc_update(dev, &ctx, in, 512);
        wm_drv_crc_update(dev, &ctx, in + 512, 512);
        wm_drv_crc_final(dev, &ctx, &crcvalue);


    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向结果的指针


应用实例
-------------
    使用 CRC 基本示例请参考 :ref:`examples/peripheral/crc<peripheral_example>`

API 参考
-------------
    查找 CRC 相关 API 请参考：

    :ref:`label_api_crc`


