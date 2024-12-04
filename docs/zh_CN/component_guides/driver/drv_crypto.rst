.. _drv_crypto:

CRYPTO
=============

简介
-------------

    W80x支持四种对称加解密 RC4、AES、DES、DES3，这四种统称为 CRYPTO 模块。AES 支持 ECB、 CBC 和 CTR 三种模式。DES/3DES 支持 ECB 和 CBC 两种模式。


功能列表
-------------

    1. :ref:`初始化 <cryp_init>`——初始化驱动。

    2. :ref:`配置 RC4 算法的ctx <rc4_init>`——配置一个上下文 ctx 中的秘钥、秘钥长度。

       :ref:`进行 RC4 计算<rc4_enc_dec>`——根据上下文 ctx，执行一次 RC4 运算。

    3. :ref:`配置 AES 算法的ctx <aes_init>`——配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。

       :ref:`进行 AES 加密或解密<aes_enc_dec>`——根据上下文 ctx，执行一次 AES 加密或解密。

    4. :ref:`配置 DES 算法的ctx <des_init>`——配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。

       :ref:`进行 DES 加密或解密<des_enc_dec>`——根据上下文 ctx，执行一次 DES 加密或解密。

    5. :ref:`配置 3DES 算法的ctx <3des_init>`——配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。

       :ref:`进行 3DES 加密或解密<3des_enc_dec>`——根据上下文 ctx，执行一次 3DES 加密或解密。



.. _cryp_init:

初始化驱动
------------------

    调用 ``wm_drv_cryp_init()`` ，得到指向加解密模块的设备指针，只有得到了这个指针才能调用其他的 crypto 相关的 API。示例：

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_cryp_init("crypto");

    函数参数指定设备名称，名称在设备表中定义。

.. _rc4_init:

配置 RC4 算法的 ctx
---------------------

    调用 ``wm_drv_rc4_init()`` ，配置一个上下文中的秘钥、秘钥长度。示例：

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_rc4_init(dev, &ctx, key, RC4_128BIT_KEYLEN);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向秘钥的指针

    第四个参数是：秘钥的长度，字节数，要求为 16 或 32


.. _rc4_enc_dec:

进行 RC4 计算
----------------

    调用 ``wm_drv_rc4_encrypt_decrypt()`` ，根据上下文 ctx，执行一次 RC4 运算。示例：

    .. code:: c

            uint8_t *in   = NULL;
            uint8_t *out1 = NULL;
            uint8_t *out2 = NULL;
            uint8_t *key  = NULL;
            int i;
            wm_drv_cryp_cfg_t ctx;
            wm_device_t *dev;

            /*initialize rng*/
            dev = wm_drv_cryp_init("crypto");
            if (dev == NULL) {
                wm_log_error("init fail.");
            }

            in   = malloc(OP_DATA_BYTES);
            out1 = malloc(OP_DATA_BYTES);
            out2 = malloc(OP_DATA_BYTES);
            key  = malloc(16);

            if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL)) {
                wm_log_info("malloc err");
                goto OUT;
            }

            for (i = 0; i < OP_DATA_BYTES; i++) {
                in[i] = rand();
            }

            for (i = 0; i < 16; i++) {
                key[i] = rand();
            }

            memset(out1, 0, OP_DATA_BYTES);
            memset(out2, 0, OP_DATA_BYTES);

            if (wm_drv_rc4_init(dev, &ctx, key, RC4_128BIT_KEYLEN) != WM_ERR_SUCCESS) {
                wm_log_info("rc4 init err");
                goto OUT;
            }
            wm_drv_rc4_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES);
            wm_drv_rc4_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES);

            if (memcmp(in, out2, OP_DATA_BYTES)) {
                wm_log_info("rc4 test fail");
            } else {
                wm_log_info("rc4 test success");
            }



    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：输入数据指针

    第四个参数是：输出数据指针

    第五个参数是：输入数据长度，字节数，范围 1 到 65532

.. _aes_init:

配置 AES 算法的 ctx
---------------------

    调用 ``wm_drv_aes_init()`` ，配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。示例：

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_CBC);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向初始化向量的指针（当 sub_mode 为 ECB 的时候，该指针可以为空），字节数为 16

    第四个参数是：指向秘钥的指针

    第五个参数是：秘钥的长度，字节数，要求为 16

    第六个参数是：支持的加解密子模式，0 到 2 分别表示 ECB、CBC 和 CTR


.. _aes_enc_dec:

进行 AES 加密或解密
------------------------

    调用 ``wm_drv_aes_encrypt_decrypt()`` ，根据上下文 ctx，执行一次 AES 加密或解密。示例：

    .. code:: c

        uint8_t *in   = NULL;
        uint8_t *out1 = NULL;
        uint8_t *out2 = NULL;
        uint8_t *key  = NULL;
        uint8_t *iv   = NULL;
        int i;
        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        /*initialize cryp */
        dev = wm_drv_cryp_init("crypto");
        if (dev == NULL) {
            wm_log_error("init fail.");
        }

        in   = malloc(OP_DATA_BYTES);
        out1 = malloc(OP_DATA_BYTES);
        out2 = malloc(OP_DATA_BYTES);
        key  = malloc(16);
        iv   = malloc(16);

        if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
            wm_log_info("malloc err");
            goto OUT;
        }

        for (i = 0; i < OP_DATA_BYTES; i++) {
            in[i] = i;
        }

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }

        memset(out1, 0, OP_DATA_BYTES);
        memset(out2, 0, OP_DATA_BYTES);
        if (wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
            wm_log_info("aes init err");
            goto OUT;
        }
        wm_drv_aes_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
        wm_drv_aes_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
        if (memcmp(in, out2, OP_DATA_BYTES)) {
            wm_log_info("aes cbc test fail");
        } else {
            wm_log_info("aes cbc test success");
        }


    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：输入数据指针

    第四个参数是：输出数据指针

    第五个参数是：输入数据长度，字节数，范围 16 到 65520，要求为 16 的整数倍

    第六个参数是：0 加密，1 解密


.. _des_init:

配置 DES 算法的 ctx
----------------------

    调用 ``wm_drv_des_init()`` ，配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。示例：

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向初始化向量的指针（当sub_mode为ECB的时候，该指针可以为空），字节数为 8

    第四个参数是：指向秘钥的指针

    第五个参数是：秘钥的长度，字节数，要求为 8

    第六个参数是：支持的加解密子模式，0 到 1 分别表示 ECB、CBC


.. _des_enc_dec:

进行 DES 加密或解密
------------------------

    调用 ``wm_drv_des_encrypt_decrypt()`` ，根据上下文 ctx，执行一次 des 加密或解密。示例：

    .. code:: c

        uint8_t *in   = NULL;
        uint8_t *out1 = NULL;
        uint8_t *out2 = NULL;
        uint8_t *key  = NULL;
        uint8_t *iv   = NULL;
        int i;
        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        /*initialize cryp */
        dev = wm_drv_cryp_init("crypto");
        if (dev == NULL) {
            wm_log_error("init fail.");
        }

        in   = malloc(OP_DATA_BYTES);
        out1 = malloc(OP_DATA_BYTES);
        out2 = malloc(OP_DATA_BYTES);
        key  = malloc(16);
        iv   = malloc(16);

        if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
            wm_log_info("malloc err");
            goto OUT;
        }

        for (i = 0; i < OP_DATA_BYTES; i++) {
            in[i] = i;
        }

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }

        memset(out1, 0, OP_DATA_BYTES);
        memset(out2, 0, OP_DATA_BYTES);
        if (wm_drv_des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
            wm_log_info("des init err");
            goto OUT;
        }
        wm_drv_des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
        wm_drv_des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
        if (memcmp(in, out2, OP_DATA_BYTES)) {
            wm_log_info("des cbc test fail");
        } else {
            wm_log_info("des cbc test success");
        }


    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：输入数据指针

    第四个参数是：输出数据指针

    第五个参数是：输入数据长度，字节数，范围 8 到 65528，要求为 8 字节的整数倍

    第六个参数是：0 加密，1 解密

.. _3des_init:

配置 3DES 算法的 ctx
------------------------

    调用 ``wm_drv_3des_init()`` ，配置一个上下文中的初始化向量（ECB 模式不需要初始化向量）、秘钥、秘钥长度、子模式。示例：

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_3des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC);

    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：指向初始化向量的指针（当sub_mode为 ECB 的时候，该指针可以为空），字节数为 8

    第四个参数是：指向秘钥的指针

    第五个参数是：秘钥的长度，字节数，要求为 24

    第六个参数是：支持的加解密子模式，0 到 1 分别表示 ECB、CBC


.. _3des_enc_dec:

进行 3DES 加密或解密
-------------------------

    调用 ``wm_drv_3des_encrypt_decrypt()`` ，根据上下文 ctx，执行一次 3DES 加密或解密。示例：

    .. code:: c

        uint8_t *in   = NULL;
        uint8_t *out1 = NULL;
        uint8_t *out2 = NULL;
        uint8_t *key  = NULL;
        uint8_t *iv   = NULL;
        int i;
        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        /*initialize cryp */
        dev = wm_drv_cryp_init("crypto");
        if (dev == NULL) {
            wm_log_error("init fail.");
        }

        in   = malloc(OP_DATA_BYTES);
        out1 = malloc(OP_DATA_BYTES);
        out2 = malloc(OP_DATA_BYTES);
        key  = malloc(16);
        iv   = malloc(16);

        if ((in == NULL) || (out1 == NULL) || (out2 == NULL) || (key == NULL) || (iv == NULL)) {
            wm_log_info("malloc err");
            goto OUT;
        }

        for (i = 0; i < OP_DATA_BYTES; i++) {
            in[i] = i;
        }

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }

        memset(out1, 0, OP_DATA_BYTES);
        memset(out2, 0, OP_DATA_BYTES);
        if (wm_drv_3des_init(dev, &ctx, iv, key, DES3_KEY_LEN, WM_GPSEC_CBC) != WM_ERR_SUCCESS) {
            wm_log_info("3des init err");
            goto OUT;
        }
        wm_drv_3des_encrypt_decrypt(dev, &ctx, in, out1, OP_DATA_BYTES, WM_GPSEC_ENCRYPT);
        wm_drv_3des_encrypt_decrypt(dev, &ctx, out1, out2, OP_DATA_BYTES, WM_GPSEC_DECRYPT);
        if (memcmp(in, out2, OP_DATA_BYTES)) {
            wm_log_info("3des cbc test fail");
        } else {
            wm_log_info("3des cbc test success");
        }



    第一个参数是：指向设备的指针

    第二个参数是：指向一个上下文的指针

    第三个参数是：输入数据指针

    第四个参数是：输出数据指针

    第五个参数是：输入数据长度，字节数，范围 8 到 65528，要求为 8 字节的整数倍

    第六个参数是：0 加密，1 解密


应用实例
-------------
    使用 CRYPTO 基本示例请参考 :ref:`examples/peripheral/crypto<peripheral_example>`

API 参考
-------------
    查找 CRYPTO 相关 API 请参考：

    :ref:`label_api_crypto`


