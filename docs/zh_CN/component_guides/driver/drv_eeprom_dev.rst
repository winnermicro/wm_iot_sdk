
.. _eeprom_dev:

EEPROM 驱动开发
=================

编写新的驱动
-------------

    SDK 中已经包含 NV24C0X 型号的驱动，实际测试也可以支持其他如 ST24C0X，AT24C0X 等型号，当有新的 EEPROM 型号需要支持时，可以先验证既有的 EEPROM 驱动是否支持，如果不支持，可以写新 EEPROM 驱动。


接口
-------------

    EEPROM 驱动有统一的接口，定义在 components/driver/include/eeprom/wm_drv_eeprom.h 中， 文件中 wm_drv_eeprom_ops_t 定义的函数指针，就是实现 EEPROM 驱动需要实现的接口。

    EEPROM 的驱动实现都放到 components/driver/src/eeprom/ 下，其中 wm_drv_eeprom.c 文件实现从统一接口到具体驱动实例的调用，具体驱动实例和设备的关联是在设备表中定义的。


    .. figure:: ../../../_static/component-guides/driver/eeprom_call.svg
        :align: center
        :alt: EEPROM 调用关系

    图中 wm_drv_eeprom_xxx.c 就是需要实现的驱动。

驱动实例
-------------

    wm_drv_eeprom_nv24c0x.c 文件是 NV24C0X 系列型号的驱动，当然 ST24C0X，AT24C0X 系列也可以使用该驱动。该文件内部声明了全局变量 wm_drv_eeprom_nv24c0x_ops ，在设备表中会关联到设备上， 对设备操作时就会调用到 ops 中的接口。

    .. code:: c

        const wm_drv_eeprom_ops_t wm_drv_eeprom_nv24c0x_ops = {
            .init     = eeprom_nv24c0x_init,
            .deinit   = eeprom_nv24c0x_deinit,
            .read     = eeprom_nv24c0x_read,
            .write    = eeprom_nv24c0x_write,
            .get_size = eeprom_nv24c0x_get_size,
            .set_wp   = eeprom_nv24c0x_set_wp,
        };

新增驱动实例
-------------

    参照 wm_drv_eeprom_nv24c0x.c 的实现，复制文件 wm_drv_eeprom_nv24c0x.c 为 wm_drv_eeprom_XXX.c ，
    修改其中的 wm_drv_eeprom_nv24c0x_ops 为 wm_drv_eeprom_XXX_ops , 然后根据新的 EEPROM型号要求修改
    实现其中的各个接口实现。

    例如：

    .. code:: c

        const wm_drv_eeprom_ops_t wm_drv_eeprom_xxx_ops = {
            .init     = eeprom_xxx_init,
            .deinit   = eeprom_xxx_deinit,
            .read     = eeprom_xxx_read,
            .write    = eeprom_xxx_write,
            .get_size = eeprom_xxx_get_size,
            .set_wp   = eeprom_xxx_set_wp,
        };

注意事项
-------------

.. note:: 新的驱动需要在设备表中关联在某个设备上，这样该设备才能使用新的驱动。


其他相关文档
-------------

    :ref:`i2c`

    :ref:`eeprom`

