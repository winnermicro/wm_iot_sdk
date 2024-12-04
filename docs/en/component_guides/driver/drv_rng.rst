.. _drv_rng:

RNG 
=============

Introduction
-------------

    W80x Supports the generation of true random numbers.


Function List
---------------

    1. :ref:`Initialization <rng_init>` —— Initialize the RNG driver.
    
       :ref:`Generate a 32-bit random number. <rng_read>`.
    
       :ref:`Generate multiple bytes of random numbers. <rng_read_bytes>`.
       
    2. :ref:`Generate multiple bytes of random numbers. <fill_random>` —— Users do not need to initialize the driver.


.. _rng_init:

Initialize the RNG Driver
--------------------------

    Call ``wm_drv_rng_init()`` , Obtain a pointer to the RNG (Random Number Generator) module; only with this pointer can you call other related APIs. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");

    The function parameter specifies the device name, which is defined in the device table.
    
.. _rng_read:

Obtain a 32-bit Random Number
------------------------------

    Call ``wm_drv_rng_read()`` , Obtain a 32-bit random number. Example:

    .. code:: c


        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");
        uint32_t rng_value = wm_drv_rng_read(dev);

    The function parameter specifies the device name, which is defined in the device table.
    
.. _rng_read_bytes:

Generate Multiple bytes of Random Numbers
----------------------------------------------

    Call ``wm_drv_rng_read()`` , Generate multiple bytes of random numbers. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rng_init("rng");
        uint8_t out[10];
        wm_drv_rng_read_bytes(dev, out, 10);

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to the random numbers.

    The third parameter is: the number of bytes for the generated random numbers.

.. _fill_random:

Generate Multiple bytes of Random Numbers
-------------------------------------------

    Call ``wm_drv_fill_random()`` Generate multiple bytes of random numbers, The function already includes the initialization of the RNG driver and the configuration of the context (ctx). Users only need to pass two parameters. For example:

    .. code:: c

            
            uint8_t out[16];
            wm_drv_fill_random(out, 16);

    The first parameter is: a pointer to the random numbers.

    The second parameter is: the number of bytes for the generated random numbers.
    
    

Application Demo
-------------------
    To use RNG, please refer to:ref:`examples/peripheral/rng<peripheral_example>`

API Reference
----------------
    For looking up RNG-related APIs, please refer to:

    :ref:`label_api_rng`
    

