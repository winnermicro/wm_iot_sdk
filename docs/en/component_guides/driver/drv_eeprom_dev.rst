
.. _eeprom_dev:

EEPROM driver development
=========================

Write a new driver
-------------------

   The SDK already includes drivers for the NV24C0X model, and actual testing also supports other models such as ST24C0X, AT24C0X, etc.  When a new EEPROM model needs to be supported, you can first verify if the existing EEPROM driver supports it.  If not, a new EEPROM driver can be written.


Interface
-------------

    The EEPROM driver has a unified interface, which is defined in the file components/driver/include/eeprom/wm_drv_eeprom.h. In this file, the function pointers defined in wm_drv_eeprom_ops_t represent the interfaces that need to be implemented for the EEPROM driver.

    The implementation of the EEPROM driver is located in the components/driver/src/eeprom/ directory. In this directory, the wm_drv_eeprom.c file implements the invocation from the unified interface to the specific driver instance. The association between the specific driver instance and the device is defined in the device table.


    .. figure:: ../../../_static/component-guides/driver/eeprom_call.svg
        :align: center
        :alt: EEPROM call relationship

    In the diagram, wm_drv_eeprom_xxx.c is the driver that needs to be implemented.

Driver instance
----------------

    The wm_drv_eeprom_nv24c0x.c file is the driver for the NV24C0X series models.  Of course, the ST24C0X and AT24C0X series can also use this driver.  The file internally declares a global variable wm_drv_eeprom_nv24c0x_ops, which will be associated with the device in the device table.  When performing operations on the device, the interfaces in ops will be called.

    .. code:: c

        const wm_drv_eeprom_ops_t wm_drv_eeprom_nv24c0x_ops = {
            .init     = eeprom_nv24c0x_init,
            .deinit   = eeprom_nv24c0x_deinit,
            .read     = eeprom_nv24c0x_read,
            .write    = eeprom_nv24c0x_write,
            .get_size = eeprom_nv24c0x_get_size,
            .set_wp   = eeprom_nv24c0x_set_wp,
        };

Add a new driver instance.
----------------------------

    Refer to the implementation of wm_drv_eeprom_nv24c0x.c, copy the file wm_drv_eeprom_nv24c0x.c to wm_drv_eeprom_XXX.c, 
    then change wm_drv_eeprom_nv24c0x_ops to wm_drv_eeprom_XXX_ops. 
    After that, modify the implementation of each interface according to the requirements of the new EEPROM model.

    For example:

    .. code:: c

        const wm_drv_eeprom_ops_t wm_drv_eeprom_xxx_ops = {
            .init     = eeprom_xxx_init,
            .deinit   = eeprom_xxx_deinit,
            .read     = eeprom_xxx_read,
            .write    = eeprom_xxx_write,
            .get_size = eeprom_xxx_get_size,
            .set_wp   = eeprom_xxx_set_wp,
        };

Notes
-------------

.. note:: The new driver needs to be associated with a specific device in the device table so that the device can use the new driver.


Other related documents
-------------------------

    :ref:`i2c`

    :ref:`eeprom`

