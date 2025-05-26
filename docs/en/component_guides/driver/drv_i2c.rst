
.. _i2c_drv:

I2C
=============

Introduction
---------------

    I2C (Inter-Integrated Circuit) is a simple, bidirectional, two-wire synchronous serial bus. It requires only two lines to transmit information between devices connected to the bus.

    I2C is a serial communication bus that uses a multi-master-slave architecture. It was developed by Philips in the 1980s for connecting low-speed peripherals to motherboards, embedded systems, or mobile phones.

    The master device initiates data transmission on the bus and generates the clock for data transmission. Any addressed device is considered a slave. The roles of master and slave, transmitter and receiver, are not fixed but depend on the direction of data transmission. If the master sends data to a slave, the master first addresses the slave, then sends data to it, and finally terminates the data transmission. If the master receives data from a slave, the master first addresses the slave, then receives data from it, and finally terminates the reception process. In this case, the master is responsible for generating the clock and terminating data transmission.

I2C controllers
-----------------

    Two I2C master controllers are implemented in the driver: One is a hardware-based I2C controller, usually with the device name "i2c". Only a few pins support this hardware controller (refer to the Pinmux for details). The second one is a software-based I2C controller, which is implemented by simulating with GPIO. Usually, the device name is "i2c_soft", and this software controller is applicable to all GPIOs.

    Both controllers use the same interface. When used by the upper layer, the only difference lies in the name of the I2C controller passed as a parameter during initialization.


Function List
---------------

    1. :ref:`Configuration <i2c_set>` — Pin and speed configuration.
    2. :ref:`Initialization <i2c_init>` — Initialize the driver.
    3. :ref:`Data Write <i2c_write>` — Write data to the I2C device.
    4. :ref:`Data Write, Read <i2c_read>` — Read data from I2C device.

Function Overview
---------------------

    **Master Device** Can only be used as a master controller.
    **Configurable Speed** 100KHz to 400KHz.
    **Combined Read and Write** Supports I2C read, write offset address or command followed by read, and write offset address or command followed by write data.
    **Multi-Slave Speed Adaptation** Supports operation with slaves of different speeds.

.. _i2c_set:

Configuration
---------------

   I2C requires configuration in the device table, with the clock and data lines configured according to the actual pins used on the Board. Other settings can use the default configurations provided in the SDK.

    .. note::
       The SCL and SDA pins for I2C are typically configured with pull-up resistors in the device table by default. Generally speaking, the pull-up resistance for I2C needs to be within a certain range, neither too large nor too small, as determined by the slave devices. This value can be calculated based on the parameters specified in the I2C slave device's manual.

I2C Address
--------------

    The I2C slave device address used by the driver interface is a 7-bit address that excludes the read/write bit. When calling the read/write interfaces, the underlying driver will left-shift the address by one bit and then add the read/write bit.

Main Functions
----------------

.. _i2c_init:

Initialize I2C
^^^^^^^^^^^^^^^^^

    Before using I2C,  you need to call the ``wm_drv_i2c_init()`` function to allocate resources for I2C. Use ``wm_device_t`` to specify the device, as shown in the example:

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");
        wm_drv_i2c_init("i2c0");

    The first parameter specifies the device name, which is defined in the device table. Currently, there is only one I2C master device, so only "i2c0" can be filled in.

    .. warning:: After initializing I2C, if ``wm_drv_i2c_deinit`` is not called, calling ``wm_drv_i2c_init`` again will return ``NULL``.

.. _i2c_write:

Write
^^^^^^^^^^^^^

    After preparing the data to be sent, call the function ``wm_drv_i2c_write`` to send the data to the I2C slave device.

    This interface has two ways of writing data for two scenarios:

    - Scenario 1
        Only the register address or command needs to be written. In this case, only the first write data buffer and buffer size are passed, and the data parameter is set to NULL, with data_size set to 0.

    - Scenario 2
        The internal address or command of the slave device needs to be passed. In this case, the first data buffer and buffer size are passed as the internal address or command, and the second parameter data and data_size are passed as the data to be written.

    Here is an example of writing to an EEPROM:

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");

        wm_drv_i2c_config_t config = {
            .addr     = 0x50,
            .speed_hz = 400000,
        };

        uint8_t sub_addr = 5;

        /* Write 5 bytes to the EEPROM at offset 5 */
        wm_drv_i2c_write(i2c_dev, &config, &sub_addr, 1, "hello", 5);


.. _i2c_read:

Reading Data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    Use the ``wm_drv_i2c_read`` function to read data from the I2C slave device. This interface requires writing the internal address of the slave device, then reading the specified length of data.

    Here is an example of reading from an EEPROM using the write-then-read method:

    .. code:: c

        wm_device_t *i2c_dev = wm_dt_get_device_by_name("i2c0");

        wm_drv_i2c_config_t config = {
            .addr     = 0x50,
            .speed_hz = 400000,
        };

        uint8_t sub_addr = 5;
        uint8_t buf[5];

        /* Read 5 bytes from the EEPROM at offset 5 */
        wm_drv_i2c_read(i2c_dev, &config, &sub_addr, 1, buf, 5);

Precautions
-------------

.. warning:: The address used by I2C is a 7-bit address, excluding the read/write bit. When calling the interfaces, the underlying driver will left-shift the address by one bit and then OR it with the read/write bit.

Application Example
--------------------

    For a basic example of using I2C, please refer to :ref:`examples/peripheral/i2c<peripheral_example>`

Other Related Documents
-------------------------
    :ref:`eeprom`
    :ref:`eeprom_dev`

API Reference
-----------------
    To find I2C-related APIs, please refer to:

    :ref:`label_api_i2c`
