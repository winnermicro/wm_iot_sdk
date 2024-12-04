.. _drv_hash:

Hash
=============

Introduction
-------------

    The W80x supports two hash algorithms: SHA1 and MD5. The SHA1/MD5/CRC of W80x supports continuous encryption for multiple packets and also supports alternate operations.


Function List
--------------

    1. :ref:`Initialization <hash_init>` —— Initialize the CRC driver.
    
    2. :ref:`Configure ctx <md5_start>` —— Configure a ctx of the MD5 type.
    
       :ref:`Append data <md5_update>` —— Calculate the MD5 based on the ctx and save the result into ctx.
       
       :ref:`Read md5 value <md5_final>` —— Read MD5 value in the ctx.
       
    3. :ref:`Calculate the MD5 value for a single packet of data. <md5>`.
    
    4. :ref:`Configure ctx <sha1_start>` —— Configure a ctx of the SHA1 type.
    
       :ref:`Append data <sha1_update>` —— Calculate the SHA1 based on the ctx and save the result into ctx.
       
       :ref:`Read sha1 value <sha1_final>` —— Read SHA1 value in the ctx.


.. _hash_init:

Initialize the Hash driver
-----------------------------

    Call ``wm_drv_hash_init()`` , Obtain a pointer to the hash module; only with this pointer can you call other hash module-related APIs. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_hash_init("hash");

    The function parameter specifies the device name, which is defined in the device table.
    
.. _md5_start:

Configure ctx
----------------

    Call ``wm_drv_md5_start()`` , Configure a ctx of the MD5 type. Example:

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_crc_init("hash");
        wm_drv_md5_start(dev, &ctx);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.



.. _md5_update:

Append Data
------------

    Call ``wm_drv_md5_update()`` , Calculate the MD5 based on the context (ctx), save the result into ctx, and this interface can be called multiple times consecutively to append data. Supports alternating execution with different contexts (ctx). Example:

    .. code:: c

        uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
        uint8_t *md5in2     = (uint8_t *)"1234567890123456abcd";
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_hash_init("hash");

        wm_drv_md5_start(dev, &ctx);
        wm_drv_md5_update(dev, &ctx, md5in1, 20);
        wm_drv_md5_update(dev, &ctx, md5in2, 20);


    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the data.

    The fourth parameter is: the length of the data in bytes.
    

.. _md5_final:

Read MD5 Value
------------------

    Call ``wm_drv_md5_final()``, Read MD5 value in the ctx, Example:

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


    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the result, 16 bytes in size.

.. _md5:

Calculate and read the MD5 value of a single packet of data.
----------------------------------------------------------------

    Call ``wm_drv_md5()``, Read the MD5 value of a single packet of data; the function already includes the initialization of the hash driver and the configuration of the context (ctx). Users only need to pass three parameters, example:

    .. code:: c
            
            uint8_t md5hash[16];
            uint8_t *md5in1     = (uint8_t *)"abacdefghgjklmno1234";
            wm_drv_md5(md5in1, 20, md5hash);

    The first parameter is: a pointer to the data.

    The second parameter is: the length of the data in bytes.

    The third parameter is: a pointer to the result, 16 bytes in size.
    
    
.. _sha1_start:

Configure ctx
---------------

    Call ``wm_drv_sha1_start()`` , Configure a ctx of the SHA1 type. Example:

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_crc_init("hash");
        wm_drv_sha1_start(dev, &ctx);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.


.. _sha1_update:

Append Data
------------

    Call ``wm_drv_sha1_update()`` , Calculate SHA1 based on the context ctx, save the result into ctx, and this interface can be called multiple times consecutively to append data. Supports alternating execution with different contexts ctx. Example:

    .. code:: c

        uint8_t *sha1in1     = (uint8_t *)"abacdefghgjklmno1234";
        uint8_t *sha1in2     = (uint8_t *)"1234567890123456abcd";
        wm_drv_hash_cfg_t ctx;
        wm_device_t *dev;

        dev = wm_drv_hash_init("hash");

        wm_drv_sha1_start(dev, &ctx);
        wm_drv_sha1_update(dev, &ctx, sha1in1, 20);
        wm_drv_sha1_update(dev, &ctx, sha1in2, 20);


    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the data.

    The fourth parameter is: the length of the data in bytes.

.. _sha1_final:

Read SHA1 value
-----------------

    Call ``wm_drv_sha1_final()`` The function reads the SHA1 value from the context ctx. Example:

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


    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the result, 20 bytes in size.


Application Demo
------------------
    To use Hash, please refer to:ref:`examples/peripheral/hash<peripheral_example>`

API Reference
-----------------
    For looking up Hash-related APIs, please refer to:

    :ref:`label_api_hash`
    

