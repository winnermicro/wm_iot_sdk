:orphan:

.. _i2s:

I2S
=============

Introduction
---------------

I2S (Inter-IC Sound) is a serial, synchronous communication protocol commonly used for transmitting audio data between two digital audio devices. This module supports multiple data formats and communication standards, making it suitable for a wide range of audio transmission scenarios.

Function List
----------------

- Supports multiple interfaces: I2S interface, PCM interface
- Supports master/slave mode
- Supports Zero-crossing detection
- Supports sample width of 8/16/24/32 bits
- Supports mono and stereo audio transmission
- Supports multiple data formats including I2S, MSB Justified, PCM A/B
- Supports DMA

Function Overview
--------------------

.. figure:: ../../../_static/component-guides/driver/I2S.drawio.svg
    :align: center

.. note::
   The diagrams above illustrate the sending and receiving processes, respectively. In cases where the clock signals of the transmitting and receiving parties are incompatible, an external CONTROLLER is used to provide a synchronous clock for both parties.

.. figure:: ../../../_static/component-guides/driver/I2Sinterface.drawio_en.svg
    :align: center

** Module supports multiple functional options**

**Mode Selection**

- Master, Slave

**Transmission Direction**

- Input, Output, Duplex

**Protocol Standard**

- I2S, MSB, PCMA, PCMB

**Frame Format**

Each channel width is 8 bits, supported frame formats:

- 8/16/24/32 bits

**Channel Types**

- Stereo (dual-channel), Mono

**Sampling Rates**

- 8KHz, 11.025KHz, 16KHz, 22.05KHz, 24KHz, 32KHz, 44.1KHz, 47.25KHz, 48KHz, 50KHz, 50.04KHz, 96KHz, 192KHz

Configuration Usage
----------------------

Parameter Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^

Use the structure ``wm_drv_i2s_cfg_t`` for configuration:

.. code:: C

    typedef struct {
        enum wm_i2s_mode mode;
        enum wm_i2s_dir dir;
        enum wm_i2s_std std;
        enum wm_i2s_xfer_type xtype;
        uint32_t rx_pkt_size;
        uint32_t rx_ready_th;
        uint32_t tx_ready_th;
    } wm_drv_i2s_cfg_t;

**mode**

| Operating Mode: Master, Slave

**dir**

| Transmission Direction: Input, Output, Duplex

**std**

| Protocol Standard: I2S, MSB, PCMA, PCMB

**fmt**

| Frame Format Standard: 8/16/24/32 bits

.. note::
    Each channel has a bit width of 8 bits.

**ctype**

| Channel Type: Stereo (dual-channel), Mono

**xtype**

| Data Transfer Type: Currently only supports DMA mode

**sample_rate_hz**

| Sampling Rate: 8KHz, 11.025KHz, 16KHz, 22.05KHz, 24KHz, 32KHz, 44.1KHz, 47.25KHz, 48KHz, 50KHz, 50.04KHz, 96KHz, 192KHz

**rx_pkt_size**

| When in input mode, the maximum length of each data packet

**rx_pkt_num**

| max number pkt buffer to receive

**tx_pkt_num**

| max number packet data to send.

.. figure:: ../../../_static/component-guides/driver/I2Streshold.drawio_en.svg
    :align: center

.. note::
   The principle for setting rx/tx threshold: The larger the memory and the lower the delay requirement, the larger the value that can be set, thereby accommodate link jitter (occurrence of data blockage or burst) situations.

**Parameters that can be dynamically modified using IOCTL API**

| Specifies the IOCTL command to execute, including:

- ``WM_DRV_I2S_CMD_SET_TX_BITSHIFT``: Set the TX clock phase to inverse mode. This is typically used for compatibility with certain hardware to ensure the correct clock signal phase.
- ``WM_DRV_I2S_CMD_SET_RX_BITSHIFT``: Set the RX clock phase to inverse mode. This is typically used for compatibility with certain hardware to ensure the correct clock signal phase.
- ``WM_DRV_I2S_CMD_SET_LZC``: Enables or disables left channel zero-crossing detection. Zero-crossing detection is a technique to ensure phase continuity of the audio signal at zero crossings.
- ``WM_DRV_I2S_CMD_SET_RZC``: Enables or disables right channel zero-crossing detection, similar to the left channel.

DMA Configuration
^^^^^^^^^^^^^^^^^^^^^^^^

This module uses DMA for efficient transmission and reception. The I2S module depends on DMA hardware, so sufficient DMA channels must be available (I2S occupies one DMA channel per direction).

Clock Configuration
^^^^^^^^^^^^^^^^^^^^^^^^

I2S includes multiple clocks:

- BCLK (serial clock, also known as Bit Clock): Corresponds to each bit of the digital audio data.
- WS/LRCLK (Word Select/Left-Right Clock): Indicating "0" for the left channel data and "1" for the right channel data transmission.
- SD (serial data): Represents audio data in binary two's complement format (data transmission from high bit to low bit).

| The upper clock of I2S is CLK_PERI, which is derived from the 480MHz output of DPLL.
| Normally, it should be fixed at 3 division, i.e., the CLK_PERI clock is 160MHz.
| This clock is further divided to obtain 80MHz and 40MHz, respectively provided to the encryption module and interface module.

Main Functions
----------------

Data Transmission Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Starting Condition:**

- I2S module initialization.

**Related Timing APIs:**

- Call ``wm_drv_i2s_write_async`` to asynchronously send I2S data. A successful return indicates the data has been successfully processed by the I2S driver, otherwise the user needs to release it.
- Call ``wm_drv_i2s_write_pause`` as needed to pause the I2S device's write operation. This interface does not reset the hardware, allowing a quick resume.
- Call ``wm_drv_i2s_write_resume`` as needed to resume the I2S device's write operation, restarting asynchronous data transmission.
- Call ``wm_drv_i2s_write_stop`` as needed to stop the I2S device's write operation.
- Call ``wm_drv_i2s_register_write_cb`` to register a callback function for asynchronous write operation.

**Result:**

- Returns ``WM_ERR_SUCCESS`` on success, or the corresponding error code on failure.

Data Reception Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Starting Condition:**

- I2S module initialization.

**Related Timing APIs:**

- Call ``wm_drv_i2s_read_async`` to asynchronously read data from the I2S device. A successful return indicates the data has been successfully processed by the I2S driver, otherwise the user needs to release it.
- Call ``wm_drv_i2s_read_pause`` as needed to pause the I2S device's read operation. This interface does not reset the hardware, allowing a quick resume.
- Call ``wm_drv_i2s_read_resume`` as needed to resume the I2S device's read operations, restarting asynchronous data reception.
- Call ``wm_drv_i2s_read_stop`` as needed to stop the I2S device's read operation.
- Call ``wm_drv_i2s_register_read_cb`` to register a callback function, to be called when asynchronous read operations are successfully completed.

**Result:**

- Returns ``WM_ERR_SUCCESS`` on success, or the corresponding error code on failure.

I2S Device Query Function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Starting Condition:**

- I2S module initialization.

**Related APIs:**

- Call ``wm_drv_i2s_dump_cfgs`` to query and print the configuration information of the I2S device.

**Result:**

- Returns device information.

Precautions
--------------

| I2S is a streaming device and has high requirements for data stream stability and latency. In practical applications, adjustable data granularity is required to accommodate different link needs.

Application Example
---------------------

    I2S example :ref:`examples/peripheral/i2s<peripheral_example>`

API Reference
----------------

    To find TIMER-related APIs, please refer to:

    :ref:`label_api_i2s`
