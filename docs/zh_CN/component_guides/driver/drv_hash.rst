.. _drv_hash:

Hash
=============

简介
-------------

    W80x 支持两种 Hash 算法 SHA1、MD5，W80x的 SHA1/MD5/CRC 支持连续多包加密，且支持交替运算。


功能列表
-------------

    1. :ref:`初始化 <hash_init>`——初始化驱动。

    2. :ref:`配置ctx <md5_start>`——配置一个 MD5 类型的上下文 ctx。

       :ref:`追加数据 <md5_update>`——根据上下文 ctx 计算 MD5，将结果保存到 ctx 中。

       :ref:`读取md5值 <md5_final>`——读取上下文 ctx 中的 MD5 值。

    3. :ref:`计算单包数据的md5值 <md5>`。

    4. :ref:`配置ctx <sha1_start>`——配置一个 SHA1 类型的上下文 ctx。

       :ref:`追加数据 <sha1_update>`——根据上下文 ctx 计算 SHA1，将结果保存到 ctx 中。

       :ref:`读取sha1值 <sha1_final>`——读取上下文 ctx 中的 SHA1 值。


.. _hash_init:

初始化驱动
------------------

    调用 ``wm_drv_hash_init()`` ，得到指向 Hash 模块的设备指针，只有得到了这个指针才能调用其他的 Hash 模块相关的 API。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_hash_init("hash");

    函数参数指定设备名称，名称在设备表中定义。

.. _md5_start:

配置 ctx
------------

    调用 ``wm_drv_md5_start()`` ，配置一个 MD5 类型的上下文 ctx。示例：

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_crc_init("hash");
        wm_drv_md5_start(dev, &ctx);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针


.. _md5_update:

追加数据
------------

    调用 ``wm_drv_md5_update()`` ，根据上下文 ctx 计算 MD5，将结果保存到 ctx 中，该接口可连续调用多次追加数据。支持不同的上下文 ctx 交替执行。示例：

    .. code:: c

        uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
        uint8_t *md5in2     = (uint8_t *)"1234567890123456abcd";
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_hash_init("hash");

        wm_drv_md5_start(dev, &ctx);
        wm_drv_md5_update(dev, &ctx, md5in1, 20);
        wm_drv_md5_update(dev, &ctx, md5in2, 20);



    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：数据指针

    第四个参数是：数据长度，字节数

.. _md5_final:

读取 MD5 值
------------

    调用 ``wm_drv_md5_final()`` 函数读取上下文 ctx 中的 MD5 值，示例：

    .. code:: c

            uint8_t md5hash[16];
            uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
            uint8_t *md5in2     = (uint8_t *)"1234567890123456abcd";

            wm_device_t *dev;
            wm_drv_hash_cfg_t ctx;

            /*initialize hash*/
            dev = wm_drv_hash_init("hash");
            if (dev == NULL) {
                wm_log_error("init fail.");
                return 0;
            }

            wm_drv_md5_start(dev, &ctx);
            wm_drv_md5_update(dev, &ctx, md5in1, 20);
            wm_drv_md5_update(dev, &ctx, md5in2, 20);
            wm_drv_md5_final(dev, &ctx, md5hash);


    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向结果的指针，16 个字节


.. _md5:

计算并读取单包数据的 MD5 值
--------------------------------

    调用 ``wm_drv_md5()`` 读取单包数据的 MD5 值，函数内部已经含有 Hash 驱动的初始化及上下文 ctx 的配置，用户只需要传三个参数，示例：

    .. code:: c


            uint8_t md5hash[16];
            uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
            wm_drv_md5(md5in1, 20, md5hash);


    第一个参数是：数据指针

    第二个参数是：数据长度，字节数

    第三个参数是：指向结果的指针，16 个字节


.. _sha1_start:

配置 ctx
------------

    调用 ``wm_drv_sha1_start()`` ，配置一个 SHA1 类型的上下文 ctx。示例：

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_crc_init("hash");
        wm_drv_sha1_start(dev, &ctx);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针


.. _sha1_update:

追加数据
------------

    调用 ``wm_drv_sha1_update()`` ，根据上下文 ctx 计算 SHA1，将结果保存到 ctx 中，该接口可连续调用多次追加数据。支持不同的上下文 ctx 交替执行。示例：

    .. code:: c

        uint8_t *sha1in1     = (uint8_t *)"abacdefghgjklmno1234";
        uint8_t *sha1in2     = (uint8_t *)"1234567890123456abcd";
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_hash_init("hash");

        wm_drv_sha1_start(dev, &ctx);
        wm_drv_sha1_update(dev, &ctx, sha1in1, 20);
        wm_drv_sha1_update(dev, &ctx, sha1in2, 20);



    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：数据指针

    第四个参数是：数据长度，字节数

.. _sha1_final:

读取 SHA1 值
------------

    调用 ``wm_drv_sha1_final()`` 函数读取上下文 ctx 中的 SHA1 值，示例：

    .. code:: c

            uint8_t sha1hash[20];
            uint8_t *sha1in1     = (uint8_t *)"abacdefghgjklmno1234";
            uint8_t *sha1in2     = (uint8_t *)"1234567890123456abcd";

            wm_device_t *dev;
            wm_drv_hash_cfg_t ctx;

            /*initialize hash*/
            dev = wm_drv_hash_init("hash");
            if (dev == NULL) {
                wm_log_error("init fail.");
                return 0;
            }

            wm_drv_sha1_start(dev, &ctx);
            wm_drv_sha1_update(dev, &ctx, sha1in1, 20);
            wm_drv_sha1_update(dev, &ctx, sha1in2, 20);
            wm_drv_sha1_final(dev, &ctx, sha1hash);


    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向结果的指针，20 个字节

应用实例
-------------
    使用 Hash 基本示例请参考 :ref:`examples/peripheral/hash<peripheral_example>`

API 参考
-------------
    查找 Hash 相关 API 请参考：

    :ref:`label_api_hash`


