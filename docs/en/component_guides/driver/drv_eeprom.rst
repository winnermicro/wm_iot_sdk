
.. _eeprom:

EEPROM
=============

Introduction
--------------

EEPROM (Electrically Erasable Programmable Read-Only Memory) refers to memory that can be electrically erased and programmed. It is a type of memory chip that retains data even when power is turned off.

Function List
----------------

  1. :ref:`Initialization <eeprom_init>` — Initializes the driver.

  2. :ref:`Get Size <eeprom_get_size>` — Gets the size of the EEPROM.

  3. :ref:`Read Data <eeprom_read>` — Reads data from the EEPROM device.

  4. :ref:`Write Data <eeprom_write>` — Writes data to the EEPROM.

  5. :ref:`Set Write Protection <set_wp>` — Sets write protection.

Function Overview
--------------------

  **EEPROM Device Configuration** Configure the EEPROM models, total size, page size, write, write protection control pins, timing, and other parameters in the device table.

  **Random Address Read/Write** Supports reading and writing of any length at any offset within the storage area.

  **Write Protection** Supports write protection functionality.

.. _eeprom_set:

Configuration
----------------

Before using the EEPROM driver, you need to configure the specific EEPROM model, address, size, page size, write page timing, and write protection functionality in the device table. Write protection is not mandatory.

.. note::
    The EEPROM drivers pre-implemented in the SDK have been tested with the following models:

    NV24C02

    NV24C04

    NV24C16

    ST24C02WP

    AT24C16

    AT24C32

    AT24C256

    Other EEPROM models may not be supported. If a particular EEPROM model is not supported by the existing drivers, you may need to develop a custom EEPROM driver based on the model or directly use the I2C driver interface for reading and writing.

Support for New EEPROM
--------------------------

The SDK already includes drivers for NV24C0X models and has been tested to support other models such as ST24C0X and AT24C0X. When a new EEPROM model needs to be supported, you can first verify if the existing EEPROM drivers are compatible. If not, you will need to write a new EEPROM driver.

How to add support for a new model? Based on the manual of the new EEPROM chip, you can modify the existing  ``eeprom0`` device entry in the device table or add a new  ``eeprom0?``  device entry. Mainly, you need to fill in the following options according to the manual:

.. code:: c

    .speed_hz          = 400000,
    .size              = 256,
    .i2c_addr          = 0x52,
    .page_size         = 16,
    .addr_width        = 8,
    .read_only         = 0,
    .max_write_time_ms = 5,

The read_only switch controls whether the driver operates in read-only mode. The other parameters need to be filled in according to the EEPROM manual.

Additionally, configuring the device to use the driver API is also necessary. For detailed information, please refer to the device table configuration.

EEPROM Address
----------------

The EEPROM address is determined by the specific EEPROM model and the hardware board wiring method. For details, refer to the EEPROM documentation and the board circuit schematic.

Typically, in a 7-bit address, the high 4 bits are ``1 0 1 0``, and the next 3 bits are usually referred to as ``A2, A1, A0``. These 3 bits can be connected to the board's GND or VDD high level, usually left floating. 
Floating and grounded both represent 0, while connecting to VCC represents 1. If all are left floating, the EEPROM address is 0b1010000, and the I2C address is 0x50. If A1 is connected to VDD and A2, A0 are left floating, the address is 0b1010010, and the I2C address is 0x52.


Main Functions
----------------

.. _eeprom_init:

Initialize EEPROM
^^^^^^^^^^^^^^^^^^^

Before using the EEPROM, you need to call the ``wm_drv_eeprom_init()`` function to allocate resources for the EEPROM using ``wm_device_t``. Example:

.. code:: c

      wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");
      wm_drv_eeprom_init("eeprom0");

The first parameter specifies the device name, which is defined in the device table and must match the name in the device table.

.. warning:: After initializing the EEPROM, if ``wm_drv_eeprom_deinit`` is not called, subsequent calls to ``wm_drv_eeprom_init`` will return ``NULL``.

.. _eeprom_get_size:

Get the Size
^^^^^^^^^^^^^^^^^^

To get the size of the EEPROM, call the ``wm_drv_eeprom_get_size()`` function, which requires the device handle of the EEPROM to be passed in.

Here is an example of how to get the size:

 .. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");

        wm_drv_eeprom_init("eeprom0");

        /* Get the size of eeprom0 */
        wm_drv_eeprom_get_size(eeprom_dev);

.. _eeprom_write:

Write
^^^^^^

Once the data to be sent is ready, call the function ``wm_drv_eeprom_write`` to send the data to the EEPROM device.

Here is an example of writing to the EEPROM:

.. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");

        wm_drv_eeprom_init("eeprom0");

        /* Write 5 bytes to the EEPROM at offset 2 */
        wm_drv_eeprom_write(eeprom_dev, 2, "hello", 5);

.. _eeprom_read:

Read
^^^^^^

To read data, use the interface function ``wm_drv_eeprom_read``, which retrieves data from the EEPROM device.

Here is an example of reading from the EEPROM:

.. code:: c

        wm_device_t *eeprom_dev = wm_dt_get_device_by_name("eeprom0");
        uint8_t buf[5];

        wm_drv_eeprom_init("eeprom0");

        /* Read 5 bytes from the EEPROM device at offset 2 */
        wm_drv_eeprom_read(eeprom_dev, 2, buf, 5);

.. _set_wp:

Set Write Protection
^^^^^^^^^^^^^^^^^^^^^^^^^

To set write protection, you need to configure the GPIO in the device table. Additionally, the EEPROM itself must support write protection functionality. Typically, pulling the EEPROM's WP pin high places the device in a write-protected state, preventing I2C from writing data to the EEPROM. Pulling the WP pin low allows the device to be written to.


Notes
----------

.. note:: When configuring the EEPROM, the total size, page size, and write time may vary by model. You need to fill in these parameters according to the exact manual; otherwise, read/write operations may not function correctly.

.. warning:: Even if write protection is set, the ``wm_drv_eeprom_write`` interface will automatically disable write protection during the write operation. However, the I2C interface cannot directly write to the EEPROM when it is in a write-protected state.


Application Example
------------------------
For a basic example of using the EEPROM, please refer to :ref:`examples/peripheral/eeprom<peripheral_example>`


Other Related Documents
--------------------------
:ref:`i2c`
:ref:`eeprom_dev`



API Reference
----------------------
To find EEPROM-related APIs, please refer to:

:ref:`label_api_eeprom`