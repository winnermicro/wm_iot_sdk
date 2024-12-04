.. _label_component_driver_spim:

.. _spimaster:

SPI Master
=================

Introduction
-----------------

SPI is a high-speed, full-duplex synchronous serial communication protocol. 
In SPI communication, devices are categorized as Master and Slaves. The SPI-Master acts as the main controller, initiates the communication process, controls the timing and rate of data transfer, and manages the entire communication flow. 
It plays a dominant role in SPI communication, capable of actively sending commands or data to SPI Slave devices and receiving responses from them.

Function List
-----------------

1. :ref:`Initialization <spi_init>` — Initialize the SPI driver.
2. :ref:`Synchronous Data Transfer <spi_trans_sync>` — Ends transmission upon completion or timeout of data transfer.
3. :ref:`Asynchronous Data Transfer <spi_trans_async>` — Ends transmission after data transfer is complete and enters a callback function.
4. :ref:`Send Command and Get Result <TX_CMD>`—— Sends instructions to the peer user and retrieves the execution result.
5. :ref:`Endianness <big_endian>`—— When hardware sends data in word units, it determines whether to send the contents of the high address or low address of the word first.


Function Overview
-------------------
**Full-Duplex Communication**: SPI supports simultaneous data sending and receiving, allowing data transmission and reception to occur within each clock cycle.

**Clock Control**: SPI Master generates and controls the serial clock signal (SCLK) to synchronize data transmission.

**Chip Select Management** :By controlling the chip select signal (CS), SPI-M can select different Slave devices for communication.

**Data Transmission Control**:The SPI Master controls the direction of data transfer and decides when to send and receive data.

**High Transmission Rate** Compared to interfaces like I2C, SPI offers higher transfer rates, suitable for applications requiring high-speed data transmission.

**SPI has four transmission modes as shown in the table below**

================ ======== ======== ==================================================================
SPI Mode          CPOL     CPHA     Description
================ ======== ======== ==================================================================
Mode 0            0        0        SCLK is low when idle, data sampled on the rising edge
Mode 1            0        1        SCLK is low when idle, data sampled on the falling edge
Mode 2            1        0        SCLK is high when idle, data sampled on the falling edge
Mode 3            1        1        SCLK is high when idle, data sampled on the rising edge
================ ======== ======== ==================================================================

SPI Master Hardware Wiring
-----------------------------

.. figure:: ../../../_static/component-guides/driver/spi_master_en.svg
    :align: center
    :scale: 30%
    :alt: SPI Wiring Diagram


.. _spi_init:

Initialization of SPI
--------------------------------
Before using SPI, you need to call the ``wm_drv_spim_init()`` function to allocate resources for SPI and  use the  ``wm_device_t`` structure  to receive the SPI device identifier. An example is as follows:

.. code:: c

     wm_device_t *spi_dev;
     spi_dev = wm_drv_spim_init("spi");

During initialization, only the ``"spi"`` string needs to be passed in.

.. note::

     1. If SPI is already initialized, you can also directly call ``wm_dt_get_device_by_name`` to obtain the SPI device pointer.
     2. SPI-related configurations can be changed in the device tree.

.. _spi_trans_sync:

SPI Synchronous Transmission
---------------------------------

Setting Transmission Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  - The API will return after the  the transmission and reception are complete. The user can set the maximum waiting time. If transmission and reception are not completed within this time, it will return immediately.
  - An SPI device can communicate with the slave using SPI. The default GPIO pins for SPI are CLK: GPIO17, MISO: GPIO16, and MOSI: GPIO7. If modifications are needed , these can be modified in the device tree.
  - Different slaves need to select their own CS pin. W800 provides CS GPIO as GPIO0, GPIO20, GPIO30, GPIO39.Any unused GPIO can also be used as a CS PIN. GPIO-related configurations can refer to the SPI master demo project.
  - The CS pin and other parameters are placed in the ``wm_dt_hw_spim_dev_cfg_t`` structure.

Use synchronous transmission by calling ``wm_drv_spim_transceive_sync()`` and passing the ``wm_dt_hw_spim_dev_cfg_t`` and ``transceive_t`` structures.

  **1. SPI Device Parameter Configuration**

    The transmission frequency, mode, and pins of SPI can be configured in the ``wm_dt_hw_spim_dev_cfg_t`` structure. Example:

    .. code:: c

        wm_device_t *spi_dev;
        spi_dev = wm_drv_spim_init("spi");

        wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 2, // 2M clock
        .mode = 0,  // SPI mode
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_20,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            },
        };

  **2. SPI Chip Select Pin Configuration**

     SPI transmission pins need to be configured separately, set to floating output mode, as the chip select pin, active low. An example configuration is as follows:

     .. code:: c

        // SPI GPIO config
        wm_drv_gpio_iomux_func_sel(config.pin_cs.pin_num, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_pullmode(config.pin_cs.pin_num, WM_GPIO_FLOAT);
        wm_drv_gpio_set_dir(config.pin_cs.pin_num, WM_GPIO_DIR_OUTPUT);
        wm_drv_gpio_data_set(config.pin_cs.pin_num);

   **3. SPI Transmission Parameter Configuration**

      - The data sent and transmitted by SPI need to set tx_buffer and rx_buffer. The data to be sent is stored in tx_buffer, and the received data is stored in rx_buffer.
      - It supports both tx_buffer and rx_buffer being non-empty for simultaneous transmission and reception.
      - It supports tx_buffer being empty and rx_buffer being non-empty for receiving data only.
      - It supports rx_buffer being empty and tx_buffer being non-empty for sending data only.
   
      An example configuration is as follows:

      .. code:: c

        uint32_t tx_len   = 8;
        uint8_t *tx_buf   = NULL;
        uint32_t rx_len   = 8;
        uint8_t *rx_buf   = NULL;

        tx_buf = malloc(tx_len);
        rx_buf = malloc(rx_len);

        transceive_t desc = {
        .tx_buf = tx_buf,
        .tx_len = tx_len,
        .rx_buf = rx_buf,
        .rx_len = rx_len,
        };
	 
Starting Synchronous Data Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After completing the synchronous transmission parameter configuration,  you can call ``wm_drv_spim_transceive_sync()`` for synchronous transmission. You need to pass in the device descriptor, SPI device parameters, transmission parameters, and timeout time (in milliseconds).

.. code:: c

            // Set TX data
            for (int i = 0; i < tx_len; i++) {
                tx_buf[i] = 0xAA;
            }

            wm_drv_spim_transceive_sync(spi_dev, &config, &desc, 1000);


Deinitializing SPI
^^^^^^^^^^^^^^^^^^^^

If SPI is no longer needed for data transmission, you can call ``wm_drv_spim_deinit()`` to remove the driver and release the allocated resources.

.. _spi_trans_async:

SPI Asynchronous Transmission
-------------------------------

Setting Transmission Parameters
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 - After calling this interface and giving the relevant parameters to the driver, the API will return immediately. The underlying layer will invoke the user-registered callback function after the transmission and reception are complete.
 - An SPI device can communicate with slaves using SPI. The default GPIO pins for SPI are CLK: GPIO17, MISO: GPIO16, and MOSI: GPIO21. If modifications are needed, they can be made in the device tree.
 - Different slaves require their own CS pin lines. The CS GPIOs provided by W800 are: GPIO0, GPIO20, GPIO30, GPIO39.
 - The CS pin and other parameters are placed in the ``wm_dt_hw_spim_dev_cfg_t`` structure.
 - To use asynchronous transmission, you can call``wm_drv_spim_transceive_sync()`` and pass the ``wm_dt_hw_spim_dev_cfg_t`` and ``transceive_t`` structure to it.

  **1. SPI Device Parameter Configuration**

    The transmission frequency, mode, and pins of SPI can be configured in the ``wm_dt_hw_spim_dev_cfg_t`` structure. Example:

    .. code:: c

        wm_device_t *spi_dev;
        spi_dev = wm_drv_spim_init("spi");

        wm_dt_hw_spim_dev_cfg_t config = {
        .freq = 2, // 2M clock
        .mode = 0,
        .pin_cs = {
            .pin_num = WM_GPIO_NUM_20,
            .pin_mux = WM_GPIO_IOMUX_FUN5,
            },
        };

   **2. SPI Chip Select Pin Configuration**

     The SPI transmission pins need to be configured separately, set to floating output mode as chip select pins, active low. The configuration example is as follows:

     .. code:: c

        // SPI GPIO config
        wm_drv_gpio_iomux_func_sel(config.pin_cs.pin_num, WM_GPIO_IOMUX_FUN5);
        wm_drv_gpio_set_pullmode(config.pin_cs.pin_num, WM_GPIO_FLOAT);
        wm_drv_gpio_set_dir(config.pin_cs.pin_num, WM_GPIO_DIR_OUTPUT);
        wm_drv_gpio_data_set(config.pin_cs.pin_num);

   **3. SPI Transmission Parameter Configuration**

    - For SPI transmission and reception, tx_buffer and rx_buffer need to be set. The data to be sent is stored in tx_buffer, and the received data is stored in rx_buffer.
    - It supports scenarios where both tx_buffer and rx_buffer are not empty, allowing simultaneous transmission and reception.
    - It supports scenarios where tx_buffer is empty but rx_buffer is not, allowing only data reception without transmission.
    - It supports scenarios where rx_buffer is empty but tx_buffer is not, allowing only data transmission without reception..

    Example configuration:

    .. code:: c

        uint32_t tx_len   = 8;
        uint8_t *tx_buf   = NULL;
        uint32_t rx_len   = 8;
        uint8_t *rx_buf   = NULL;

        tx_buf = malloc(tx_len);
        rx_buf = malloc(rx_len);

        transceive_t desc = {
        .tx_buf = tx_buf,
        .tx_len = tx_len,
        .rx_buf = rx_buf,
        .rx_len = rx_len,
        };

    **4. Configure SPI Callback Function**

    After the asynchronous transmission is completed, the callback function will be invoked. This function will pass an integer ``result`` and user data ``data``. The returned data type is ``wm_spim_callback_t``. Example:

    .. code:: c

        void example_spim_async_callback(int result, void *data) {
            printf("SPI callback: transmit result: %d\n", result);
        }

Starting Asynchronous Data Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After completing the asynchronous transmission parameter configuration, you can call ``wm_drv_spim_transceive_async()`` to perform asynchronous transmission. It requires passing in the device descriptor, SPI device parameters, transmission parameters, callback function, and user dataPass the device descriptor, SPI device parameters, transmission parameters, callback function, and user data .

.. code:: c

            // Set TX data
            for (int i = 0; i < tx_len; i++) {
                tx_buf[i] = 0xAA;
            }

            wm_drv_spim_transceive_async(spi_dev, &config, &desc, example_spim_async_callback, NULL);

Deinitialize SPI
^^^^^^^^^^^^^^^^^^^^

If SPI is no longer needed for data transmission, you can call ``wm_drv_spim_deinit()`` to uninstall the driver and release the allocated resources.



.. _TX_CMD:


SPI Send Command and Retrieve Result
-------------------------------------

Function Overview
^^^^^^^^^^^^^^^^^^

This feature supports sending commands and operating addresses to a slave device and retrieving results, such as manipulating SPI flash memory. The corresponding timing diagram is provided below :
     
    .. figure:: ../../../_static/component-guides/driver/spim_cmd_addr.svg
        :align: center
        :scale: 100%
        :alt: Timing Diagram for Sending Commands
		
Phase T1: Send user-defined commands and operating addresses.
    
Phase T2: Support user-configurable dummy bits.
    
Phase T3: Retrieve data returned by the slave.


Data Structure
^^^^^^^^^^^^^^^^^^

To send commands, you need to use the SPI master extended parameter structure as shown below.
	
.. code:: c

        //***Only support half duplex mode
        typedef struct {
            spim_transceive_t basic; /**< it's must be as the first member */
            uint16_t cmd;            /**< [input] tx command data, if you want to use this member must set "cmd_len" at the same time,
                                          if transmit low byte first, must set SPI_TRANS_CMD_SMALL_ENDIAN */
            uint32_t addr;           /**< [input] tx address data, if you want to use this member must set "addr_len" at the same time,
                                          if transmit low byte first, must set SPI_TRANS_ADDR_SMALL_ENDIAN */
            uint8_t cmd_len;         /**< [input] command length, unit is byte [0~2]*/
            uint8_t addr_len;        /**< [input] address length, unit is byte [0~4] */
            uint8_t dummy_bits; /**< [input] dummy length, unit is bit, must set SPI_TRANS_DUMMY_BITS flag if want to use this member */
        } spim_transceive_ex_t;



Example Code
^^^^^^^^^^^^^^^^^^

.. code:: c

        spim_transceive_ex_t desc_ex = { 0 };

        desc_ex.cmd     = 0x8;
        desc_ex.cmd_len = 1;
        desc_ex.basic.flags |= SPI_TRANS_VARIABLE_CMD;

        desc_ex.addr = 0x123456;
        desc_ex.addr_len = 3;
        desc_ex.basic.flags |= SPI_TRANS_VARIABLE_ADDR;

        desc_ex.dummy_bits = 8;
        desc_ex.basic.flags |= SPI_TRANS_DUMMY_BITS;

        desc_ex.basic.tx_buf = NULL;
        desc_ex.basic.tx_len = 0;
        desc_ex.basic.rx_buf = rx_buf;
        desc_ex.basic.rx_len = 32;

        ret = wm_drv_spim_transceive_sync(dev, config, &desc_ex.basic, 1000)

.. note::

    1: This feature only supports the simplex and half-duplex modes mentioned in the function overview and is not supported in full-duplex mode.

    2: The cmd and addr parameters will only be valid if the SPI_TRANS_VARIABLE_CMD or SPI_TRANS_VARIABLE_ADDR flag in desc_ex.basic.flags is set.


.. _big_endian:


Endianness
-----------------

Function Introduction
^^^^^^^^^^^^^^^^^^^^^^^
  
The W800 SPI hardware supports little-endian and big-endian transmission functions.
  
Little-Endian Mode:
     - Transmission Direction: When sending data in word units, the SPI controller hardware will transmit the content of the low address of the word first.
     - Reception Direction: The SPI controller hardware will store the received byte stream into the FIFO first, with the storage unit being a word. The storage order is to store into the low address space of the word first.
  
Big-Endian Mode:
     - Transmission Direction: When sending data in word units, the SPI controller hardware will transmit the content of the high address of the word first.
     - Reception Direction:  The SPI controller hardware will store the received byte stream into the FIFO first, with the storage unit being a word. The storage order is to store into the high address space of the word first.

The driver layer provides a flag bit: SPI_TRANS_BIG_ENDIAN, allowing users to choose between big-endian and little-endian modes when calling the SPI transmission and reception API each time.

TX Direction Example:
^^^^^^^^^^^^^^^^^^^^^^

Assume the user wants to send: 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB.

The way data is stored in the TX FIFO is shown in the figure below:

.. figure:: ../../../_static/component-guides/driver/spim_endian_tx.svg
    :align: center
    :scale: 100%
    :alt: Diagram of SPI connection lines

In Little-Endian Mode: The HW will start sending from the low address of a word, so 0x11 will be sent first, followed by: 0x11(first), 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB.

In Big-Endian Mode: The HW will start sending from the high address of a word, so 0x44 will be sent first, followed by: 0x44(first), 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55, 0xBB, 0xAA, 0x99.


RX Direction Example:
^^^^^^^^^^^^^^^^^^^^^
Assume the data sent by the opposite end is: 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB.

In Little-Endian Mode: The order of data delivered to the user is: 0x11(first), 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB.

In Big-Endian Mode: The order of data delivered to the user is: 0x44(first), 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55, 0x99, 0xAA, 0xBB.

In Big-Endian Mode: The HW will start sending from the high address of a word, so 0x44 will be sent first, followed by: 0x44(first), 0x33, 0x22, 0x11, 0x88, 0x77, 0x66, 0x55, 0xBB, 0xAA, 0x99.



Notes
------------------

.. note::

   1.If the transmission or reception start address is not 4-byte aligned, the underlying layer will not use DMA for transmission and reception.
   2.The transmission length is preferably not less than the reception length to avoid the opposite end receiving invalid data.
   3.Only transmission or only reception is supported.
   4.There are no restrictions on transmission and reception lengths.
   5.The CLOCK, MISO, and MOSI pins are configured in the SPI driver. If the user wants to modify the default pins, they can do so in the DT (Device Table).
   6.The CS pin is passed by the user each time the transmission and reception API is called. The driver will manage the CS pin. If the user passes an invalid CS pin (WM_GPIO_NUM_MAX), the CS pin will be maintained by the user themselves, unless for special purposes. We do not recommend that the user application layer control the CS Pin, especially in scenarios where the SPI Master controls multiple Slave devices. If some devices are configured to use the Driver to control the CS pin, while others are configured to be controlled by the user application, it can easily lead to confusion.



Application Example
----------------------

For basic SPI usage examples, refer to :ref:`examples/peripheral/spi_master <spi_master_example>`

API Reference
------------------
For related SPI APIs, refer to:

:ref:`label_api_spi_master`