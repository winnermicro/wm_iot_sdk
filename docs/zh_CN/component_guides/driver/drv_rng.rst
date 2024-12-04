.. _drv_rng:

RNG
=============

简介
-------------

    W80x 支持产生真随机数。


功能列表
-------------

    1. :ref:`初始化 <rng_init>`——初始化驱动。

       :ref:`得到一个 32 位随机数 <rng_read>`。

       :ref:`得到多个字节的随机数<rng_read_bytes>`。

    2. :ref:`得到多个字节的随机数 <fill_random>`——用户不需要初始化驱动。


.. _rng_init:

初始化驱动
------------------

    调用 ``wm_drv_rng_init()`` ，得到指向 rng 模块的设备指针，只有得到了这个指针才能调用其他的相关的 API。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");

    函数参数指定设备名称，名称在设备表中定义。

.. _rng_read:

得到一个 32 位随机数
---------------------

    调用 ``wm_drv_rng_read()`` ，得到一个 32 位随机数。示例：

    .. code:: c


        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");
        uint32_t rng_value = wm_drv_rng_read(dev);

    函数参数是指向设备的指针

.. _rng_read_bytes:

得到多个字节的随机数
------------------------

    调用 ``wm_drv_rng_read()`` ，得到多个字节的随机数。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");
        uint8_t out[10];
        wm_drv_rng_read_bytes(dev, out, 10);

    第一个参数是：指向设备的指针

    第二个参数是：指向随机数的指针

    第三个参数是：产生的随机数的字节数

.. _fill_random:

得到多个字节的随机数
-----------------------

    调用 ``wm_drv_fill_random()`` 得到多个字节的随机数，函数内部已经含有 rng 驱动的初始化及上下文 ctx 的配置，用户只需要传两个参数，示例：

    .. code:: c


            uint8_t out[16];
            wm_drv_fill_random(out, 16);


    第一个参数是：指向随机数的指针

    第二个参数是：产生的随机数的字节数



应用实例
-------------
    使用 RNG 基本示例请参考 :ref:`examples/peripheral/rng<peripheral_example>`

API 参考
-------------
    查找 RNG 相关 API 请参考：

    :ref:`label_api_rng`


