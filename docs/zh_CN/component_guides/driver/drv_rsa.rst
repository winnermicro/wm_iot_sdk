.. _drv_rsa:

RSA
=============

简介
-------------

    W80x 支持硬件模幂运算。模幂运算的目的是通过一种有效的方法来计算 ( A^E mod N )，即计算 ( A ) 的 ( E ) 次幂后再对 ( N ) 取模的结果。W80x SDK 用硬件适配了 mbedtls 库中的模幂运算。用户可以使用 mbedtls_rsa_pkcs1_encrypt 对数据进行加密，使用 mbedtls_rsa_pkcs1_decrypt 对数据进行解密。


功能列表
-------------

    1. :ref:`初始化 <rsa_init>`——初始化驱动。
    2. :ref:`模幂运算 <expt_mod>`。


.. _rsa_init:

初始化驱动
------------------

    调用 ``wm_drv_rsa_init()`` ，得到指向 rsa 模块的设备指针，只有得到了这个指针才能调用 wm_drv_rsa_expt_mod。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rsa_init("rsa");

    函数参数指定设备名称，名称在设备表中定义。

.. _expt_mod:

执行模幂运算
--------------

    调用 ``wm_drv_rsa_expt_mod()`` ，执行模幂运算，X = A^E mod N。在 mbedtls 库中适配模幂运算的示例：

    .. code:: c

        wm_device_t* wm_drv_rsa_dev = wm_dt_get_device_by_name("rsa");

        MBEDTLS_MPI_CHK(wm_drv_rsa_expt_mod(wm_drv_rsa_dev, &T, &T, &ctx->E, &ctx->N));

    第一个参数是：指向设备的指针

    第二个参数是：指向结果 X 的指针。

    第三个参数是：指向底数 A 的指针。

    第二个参数是：指向指数 E 的指针。

    第二个参数是：指向模数 N 的指针。

应用实例
-------------
    使用 RSA 基本示例请参考 :ref:`examples/peripheral/rsa<peripheral_example>`

API 参考
-------------
    查找 RSA 相关 API 请参考：

    :ref:`label_api_rsa`


