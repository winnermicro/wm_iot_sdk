.. _drv_crc:

CRC 
=============

Introduction
-------------

    CRC(Cyclic Redundancy Check) is the most commonly used error-checking code in the field of data communication. Its characteristic is that the lengths of the information field and the check field can be arbitrarily selected. Cyclic Redundancy Check is a data transmission error detection function that performs polynomial calculations on data and appends the result to the end of the frame. The receiving device also performs a similar algorithm to ensure the correctness and integrity of data transmission.    
    W80x supports four modes: CRC8, CRC16_MODBUS, CRC16_CCITT, and CRC32. It supports input/output inversion. SHA1/MD5/CRC supports continuous encryption for multiple packets and also supports alternate operations.W80x supports four modes: CRC8, CRC16_MODBUS, CRC16_CCITT, and CRC32. It supports input/output inversion. SHA1/MD5/CRC supports continuous encryption for multiple packets and also supports cross operations.


Function List
----------------

    1. :ref:`Initialization <crc_init>` —— Initialize the CRC driver.
    2. :ref:`Configure ctx <crc_cfg>` —— Configure the context (ctx) based on the CRC calculation mode and whether input/output is reversed.
    3. :ref:`Append data <crc_update>` —— Append data to ctx and calculate the CRC.
    4. :ref:`Read crc <crc_final>` —— Read the crc value in ctx.



.. _crc_init:

Initialize the CRC Driver
---------------------------

    Call ``wm_drv_crc_init()``, obtain a pointer to the CRC module; only with this pointer can you call other CRC module-related APIs. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_crc_init("crc");

    The function parameter specifies the device name, which is defined in the device table.
    
.. _crc_cfg:

Configure ctx
---------------

    Call ``wm_drv_crc_cfg()``, Configure the context (ctx) based on the CRC calculation mode and whether input/output is reversed. Example:

    .. code:: c

        uint32_t init_value = 0xFFFFFFFF;
        wm_drv_crc_cfg_t ctx;
        wm_device_t *dev;

        /*initialize crc*/
        dev = wm_drv_crc_init("crc");
        wm_drv_crc_cfg(dev, &ctx, init_value, WM_GPSEC_CRC8, WM_GPSEC_CRC_REVERSE_NONE);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: the initial value.

    The fourth parameter is: the CRC calculation mode, with 0 to 3 representing CRC8, CRC16_MODBUS, CRC16_CCITT, and CRC32, respectively.

    The fifth parameter is: whether input and output are inverted, with 0 indicating no inversion, 1 indicating output inversion, 2 indicating input inversion, and 3 indicating both input and output inversion.


.. _crc_update:

Append Data
------------

    Call ``wm_drv_crc_update()`` ,Calculate the CRC based on the context (ctx), save the result into ctx, and this interface can be called multiple times consecutively to append data. Supports alternating execution with different contexts ctx. Example:

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



    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the data.

    The fourth parameter is: the length of the data in bytes, with a range of 1-65532.     

.. _crc_final:

Read CRC
------------

    Call the ``wm_drv_crc_final()`` function to read the CRC value from the context (ctx) , example:

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

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to a context.

    The third parameter is: a pointer to the result.


Application Demo
--------------------
    To use CRC, please refer to:`examples/peripheral/crc<peripheral_example>`

API Reference
----------------
    For looking up CRC-related APIs, please refer to:

    :ref:`label_api_crc`
    

