.. _drv_crypto:

CRYPTO 
=============

Introduction
-------------

    The W80x supports four types of symmetric encryption and decryption: RC4, AES, DES, and DES3, which are collectively referred to as the CRYPTO module. AES supports three modes: ECB, CBC, and CTR. DES/3DES supports two modes: ECB and CBC.


Function List
---------------

    1. :ref:`Initialization <cryp_init>` —— Initialize the crypto driver.
    
    2. :ref:`Configure the ctx for the RC4 algorithm <rc4_init>` —— Configure the secret key and key length in a context ctx.
    
       :ref:`Perform RC4 calculation <rc4_enc_dec>` —— Perform an RC4 operation based on the context ctx.
       
    3. :ref:`Configure the ctx for the AES algorithm <aes_init>` —— Configure the initialization vector (not required for ECB mode), key, key length, and sub-mode in a context.
    
       :ref:`Perform AES encryption or decryption <aes_enc_dec>` —— Perform an AES encryption or decryption operation based on the context ctx.
       
    4. :ref:`Configure the ctx for the DES algorithm <des_init>` —— Configure the initialization vector (not required for ECB mode), key, key length, and sub-mode in a context.
    
       :ref:`Perform DES encryption or decryption <des_enc_dec>` —— Perform an DES encryption or decryption operation based on the context ctx.
       
    5. :ref:`Configure the ctx for the 3DES algorithm <3des_init>` —— Configure the initialization vector (not required for ECB mode), key, key length, and sub-mode in a context.
    
       :ref:`Perform 3DES encryption or decryption <3des_enc_dec>` —— Perform an 3DES encryption or decryption operation based on the context ctx.



.. _cryp_init:

Initialize the CRC Driver
----------------------------

    Call ``wm_drv_cryp_init()`` , Obtain a pointer to the encryption/decryption module; only with this pointer can you call other cryp-related APIs. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_cryp_init("crypto");

    The function parameter specifies the device name, which is defined in the device table.
    
.. _rc4_init:

Configure the ctx for RC4
---------------------------

    Call ``wm_drv_rc4_init()`` , Configure the key and key length in a context. Example:

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_rc4_init(dev, &ctx, key, RC4_128BIT_KEYLEN);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the key.

    The fourth parameter is: the length of the key in bytes, which must be either 16 or 32.


.. _rc4_enc_dec:

Perform RC4 calculation
---------------------------

    Call ``wm_drv_rc4_encrypt_decrypt()`` , Perform an RC4 operation based on the context ctx. Example:

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


    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the input data.

    The fourth parameter is: a pointer to the output data.

    The fifth parameter is: the length of the input data in bytes, ranging from 1 to 65532.


.. _aes_init:

Configure the ctx for AES
---------------------------

    Call ``wm_drv_aes_init()`` , Configure the initialization vector (not needed for ECB mode), key, key length, and sub-mode in a context. Example:

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_aes_init(dev, &ctx, iv, key, AES_128BIT_KEYLEN, WM_GPSEC_CBC);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the initialization vector (this pointer can be NULL when the sub_mode is ECB), with a byte count of 16.

    The fourth parameter is: a pointer to the key.

    The fifth parameter is: the length of the key in bytes, which must be 16.

    The sixth parameter is: the supported encryption/decryption sub-mode, with 0, 1, and 2 representing ECB, CBC, and CTR, respectively.



.. _aes_enc_dec:

Perform AES encryption or decryption
---------------------------------------

    Call ``wm_drv_aes_encrypt_decrypt()`` , Perform an AES encryption or decryption based on the context ctx. Example:

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
        
        
    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the input data.

    The fourth parameter is: a pointer to the output data.

    The fifth parameter is: the length of the input data in bytes, ranging from 16 to 65520, and must be a multiple of 16.

    The sixth parameter is: 0 for encryption, 1 for decryption.


.. _des_init:

Configure the ctx for DES
---------------------------

    Call ``wm_drv_des_init()`` , Configure the initialization vector (not needed for ECB mode), key, key length, and sub-mode in a context. Example:

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the initialization vector (this pointer can be NULL when the sub_mode is ECB), with a byte count of 8.

    The fourth parameter is: a pointer to the key.

    The fifth parameter is: the length of the key in bytes, which must be 8.

    The sixth parameter is: the supported encryption/decryption sub-mode, with 0 and 1 representing ECB and CBC, respectively.


.. _des_enc_dec:

Perform DES encryption or decryption
---------------------------------------

    Call ``wm_drv_des_encrypt_decrypt()`` , Perform an DES encryption or decryption based on the context ctx. Example:

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
        
        
    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the input data.

    The fourth parameter is: a pointer to the output data.

    The fifth parameter is: the length of the input data in bytes, ranging from 8 to 65528, and must be a multiple of 8 bytes.

    The sixth parameter is: 0 for encryption, 1 for decryption.


.. _3des_init:

Configure the ctx for 3DES
---------------------------

    Call ``wm_drv_3des_init()`` , Configure the initialization vector (not needed for ECB mode), key, key length, and sub-mode in a context. Example:

    .. code:: c

        wm_drv_cryp_cfg_t ctx;
        wm_device_t *dev;

        for (i = 0; i < 16; i++) {
            key[i] = rand();
            iv[i]  = rand();
        }
        dev = wm_drv_cryp_init("crypto");
        wm_drv_3des_init(dev, &ctx, iv, key, DES_KEY_LEN, WM_GPSEC_CBC);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the initialization vector (this pointer can be NULL when the sub_mode is ECB), with a byte count of 8.

    The fourth parameter is: a pointer to the secret key.

    The fifth parameter is: the length of the secret key in bytes, which must be 24.

    The sixth parameter is: the supported encryption/decryption sub-mode, with 0 and 1 representing ECB and CBC, respectively.


.. _3des_enc_dec:

Perform 3DES encryption or decryption
---------------------------------------

    Call ``wm_drv_3des_encrypt_decrypt()`` , Perform an 3DES encryption or decryption based on the context ctx. Example:

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
        
        
    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the input data.

    The fourth parameter is: a pointer to the output data.

    The fifth parameter is: the length of the input data in bytes, ranging from 8 to 65528, and must be a multiple of 8.

    The sixth parameter is: 0 for encryption, 1 for decryption.


Application Demo
-----------------
    To use crypto, please refer to:ref:`examples/peripheral/crypto<peripheral_example>`

API Reference
----------------
    For looking up crypto-related APIs, please refer to:

    :ref:`label_api_crypto`
    

