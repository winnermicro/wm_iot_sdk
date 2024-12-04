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
- Zero-crossing detection
- Supports FIFO functionality
- Supports sample width of 8/16/24/32 bits
- Supports stereo audio transmission
- Supports multiple data formats including I2S, MSB Justified, PCM A/B
- Supports interrupts and DMA
- Mute functionality

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
        enum wm_i2s_fmt fmt;
        enum wm_i2s_chan_type ctype;
        enum wm_i2s_xfer_type xtype;
        uint32_t sample_rate_hz;
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

**rx_ready_th**

| When in input mode, the driver-layer buffer threshold. Only when the buffered data exceeds this threshold will the buffered packets be chained and then delivered to the user through the user-registered rxready callback.

**tx_ready_th**

| When in output mode, the driver-layer buffer threshold. Only when the buffered data exceeds this threshold will the transmission actually begin.

.. figure:: ../../../_static/component-guides/driver/I2Streshold.drawio_en.svg
    :align: center
    
.. note::
   The principle for setting rx/tx threshold: The larger the memory and the lower the delay requirement, the larger the value that can be set, thereby accommodate link jitter (occurrence of data blockage or burst) situations.

**Parameters that can be dynamically modified using IOCTL API**

.. code:: C

    typedef struct {
        enum wm_drv_i2s_ioctl_cmd cmd;
        bool tx; /* tx/rx select for this setting */
        union {
            bool mclk_en; /**< true if enable mclk */
            bool mute;    /**< true if enable mute */
            bool inverse; /**< true if enable clock inverse */
            bool lzc;     /**< true if enable left zero cross check */
            bool rzc;     /**< true if enable right zero cross check */
            bool left;    /**< if set to left channel when work in mono mode */
        } u;
    } wm_drv_i2s_ioctl_args_t;

**cmd**

| Specifies the IOCTL command to execute, including:

- ``WM_DRV_I2S_CMD_SET_MUTE``: Enable or disable the mute function for TX or RX. When enabled, the respective channel will not transmit or receive audio data.
- ``WM_DRV_I2S_CMD_SET_INVERSE``: Set the clock phase to inverse mode. This is typically used for compatibility with certain hardware to ensure the correct clock signal phase.
- ``WM_DRV_I2S_CMD_CLS_FIFO``: Clear the FIFO buffer for TX or RX. This helps reset or clear the data in the buffer when necessary.
- ``WM_DRV_I2S_CMD_SET_MONO_CH``: Set the channel used in mono mode, allowing selection of whether to use the left or right channel data.
- ``WM_DRV_I2S_CMD_SET_LZC``: Enables or disables left channel zero-crossing detection. Zero-crossing detection is a technique to ensure phase continuity of the audio signal at zero crossings.
- ``WM_DRV_I2S_CMD_SET_RZC``: Enables or disables right channel zero-crossing detection, similar to the left channel.

**tx**

| Indicates whether this setting is applied to TX (Transmit) or RX (Receive).

**u**

| Depending on the cmd value, it can store different boolean parameters:

- mclk_en: Used when the cmd requires enabling or disabling the master clock.
- mute: Used when the cmd is to set mute function.
- inverse: Used when the cmd is to set clock phase inversion.
- lzc: Used when the cmd is to enable or disable left channel zero-cross check.
- rzc: Used when the cmd is to enable or disable right channel zero-cross check.
- left: Used when the cmd is to set the channel in mono mode, selecting whether to use the left channel.

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

.. code:: C

    int main(void)
    {
        int ret;
        uint8_t *buf;
        wm_drv_i2s_cfg_t cfg;

        memset(&cfg, 0x0, sizeof(cfg));

        cfg.mode           = WM_I2S_MODE_MASTER;
        cfg.dir            = WM_I2S_DIR_OUT;
        cfg.std            = WM_I2S_STD_I2S;
        cfg.fmt            = WM_I2S_FMT_16BIT;
        cfg.ctype          = WM_I2S_CHAN_TYPE_STEREO;
        cfg.xtype          = WM_I2S_XFER_DMA;
        cfg.sample_rate_hz = 8000;

        wm_device_t *i2s_device = wm_drv_i2s_init("i2s", &cfg);
        if (i2s_device == NULL) {
            wm_log_error("I2S driver init Failed!\n");
            return WM_ERR_FAILED;
        }

        wm_drv_i2s_register_write_cb(i2s_device, txdone_cb);
        wm_drv_i2s_register_read_cb(i2s_device, rxready_cb);

        buf = calloc(1, block_size);

        ret = wm_drv_i2s_write_async(i2s_device, (void *)buf, block_size);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver init Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        while (send_pkts == 0) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        wm_log_info("I2S send 1 pkt done!\n");

        ret = wm_drv_i2s_write_stop(i2s_device);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver stop Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        ret = wm_drv_i2s_deinit(i2s_device);
        if (ret != WM_ERR_SUCCESS) {
            wm_log_error("I2S driver deinit Failed!\n");
            free_buf(buf);
            return WM_ERR_FAILED;
        }

        return WM_ERR_SUCCESS;
    }

API Reference
----------------

    To find TIMER-related APIs, please refer to:

    :ref:`label_api_i2s`
