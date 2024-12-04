.. _drv_rsa:

RSA 
=============

Introduction
-------------

    The W80x supports hardware modular exponentiation. The purpose of modular exponentiation is to efficiently calculate (A^E mod N), which means computing A raised to the power of E and then taking the result modulo N. The W80x SDK adapts the modular exponentiation in the MbedTLS library using hardware. Users can encrypt data using mbedtls_rsa_pkcs1_encrypt and decrypt data using mbedtls_rsa_pkcs1_decrypt.


Function List
----------------

    1. :ref:`Initialization <rsa_init>` —— Initialize the RSA driver.
    2. :ref:`Modular Exponentiation Calculation <expt_mod>`. 


.. _rsa_init:

Initialize the RSA driver
---------------------------

    Call ``wm_drv_rsa_init()`` , To obtain a device pointer pointing to the RSA module, which is necessary for invoking the wm_drv_rsa_expt_mod function. Example:

    .. code:: c

        wm_device_t *dev;
        dev = wm_drv_rsa_init("rsa");

    The function parameter specifies the device name, which is defined in the device table.
    
.. _expt_mod:

Perform Modular Exponentiation
----------------------------------

    Call ``wm_drv_rsa_expt_mod()`` , Perform modular exponentiation, where X = A^E mod N. Here is an example of adapting modular exponentiation in the MbedTLS library:

    .. code:: c

        wm_device_t* wm_drv_rsa_dev = wm_dt_get_device_by_name("rsa");

        MBEDTLS_MPI_CHK(wm_drv_rsa_expt_mod(wm_drv_rsa_dev, &T, &T, &ctx->E, &ctx->N));

    The first parameter is: a pointer to the device.

    The second parameter is: a pointer to the result X.

    The third parameter is: a pointer to the base A.

    The fourth parameter is: a pointer to the exponent E.

    The fifth parameter is: a pointer to the modulus N.

Application Demo
-----------------
    To use RSA, please refer to:ref:`examples/peripheral/rsa<peripheral_example>`

API Reference
----------------
    For looking up RSA-related APIs, please refer to:

    :ref:`label_api_rsa`
    

