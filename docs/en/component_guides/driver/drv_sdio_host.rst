.. _drv_sdio_host:

SDIO Host
=============

Introduction
-------------------

The SDIO Host module includes SDH SDMMC and SDH SPI.

- **SDIO Host**: It is an interface used in embedded systems and computers, allowing devices to communicate with various types of Secure Digital (SD) cards through the Secure Digital Input Output (SDIO) interface.

- **SDH SDMMC**: Primarily used for reading and writing SD cards, including traditional SD cards and SDHC (Secure Digital High Capacity) cards.

- **SDH SPI**: The SPI mode of the SDIO interface allows devices to communicate with SPI devices through the SPI (Serial Peripheral Interface) protocol.It is mainly used to drive devices that only require one-way communication, such as TFT (Thin Film Transistor) displays.


Feature List
------------------

- SDH SDMMC Memory Card Reading and Writing
- SDH SPI Synchronous Data Transfer
- SDH SPI Asynchronous Data Transfer

Overview
-------------
SDH SDMMC Storage Card Read/Write:

- Supports traditional SD cards and SDHC cards, achieving efficient read/write operations for these storage media.
- Allows users to access and store large amounts of data, including text, images, audio, and video files.
- The SDK adapts the FATFS based on the SDH SDMMC driver, so you can directly use the FATFS API for SD card file read/write operations. Refer to Refer to:** :ref:`examples/storage/fatfs/sdmmc_disk<storage_example>`.

SDH SPI Synchronous Data Transfer:

- Synchronously transfers data through the SPI mode of the SDIO interface.
- Primarily used for transmitting small amounts of data, such as CMD transmission for driving TFT screens.

SDH SPI Asynchronous Data Transmission:

- Asynchronously transmits data through the SPI mode of the SDIO interface.
- Primarily used for transmitting small amounts of data, such as CMD transmission for driving TFT screens.

Hardware Configuration
---------------------------------

Configured through modifying the Device Tree (DT).

- The SDH SDMMC data width can be configured as 1bit (0) and 4bits (1).
- The SDH SDMMC/SPI clock frequencie supports 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 of the CPU clock. If the CPU clock is 240MHz, the supported clock frequencies are 120, 60, 40, 30, 24, 20, ≈17.14, 15 MHz. It is recommended that the clock does not exceed 60MHz.
- SDH SPI only supports mode 0 (CPOL=0, CPHA=0).
- The pin correspondence for SDH SPI is as follows.For detailed pinmux function multiplexing, please refer to the PINMUX section:

| SDH_SPI Pins | SDH_SDMMC Pins |
|-------------|----------------|
| SDH_SPI_CLK | MMC_CLK_x (x can be 0/1) |
| SDH_SPI_MOSI | MMC_CMD_x (x can be 0/1) |
| SDH_SPI_MISO | Half-duplex, cannot receive directly in SDMMC mode |
| SDH_SPI_CS | Any unused general GPIO can be used |

Main Functions
--------------------------

SDH SDMMC Device Data Read/Write
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Initial Conditions:**

- SDH SDMMC initialization.

**Related Timing APIs:**

- Call ``wm_drv_sdh_sdmmc_init`` to initialize the SDH SDMMC module.
- Call ``wm_drv_sdh_sdmmc_read_blocks`` to read data blocks from the SDH SDMMC device.
- Call ``wm_drv_sdh_sdmmc_write_blocks`` to write data blocks to the SDH SDMMC device.

**Results:**

- Returns ``return WM_ERR_SUCCESS`` on success, corresponding error code on failure.

SDH SPI Device Data Transmission / Reception
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Initial Conditions:**

- Initial SPI module configuration
- SDH SPI initialization.

**Related Timing APIs:**

- Call ``wm_drv_sdh_sdspi_init`` to initialize the SDH SPI module.
- Call ``wm_drv_sdh_sdspi_transceive_sync`` to synchronously transmit and receive data through the SDH SPI device.
- Call ``wm_drv_sdh_sdspi_transceive_async`` to asynchronously transmit and receive data through the SDH SPI device.

**Results:**

- Returns ``return WM_ERR_SUCCESS`` on success, or the corresponding error code on failure.

Notes
--------------------

- For SDH SDMMC memory card read/write, the single block data length is 512 bytes, and the maximum transmission data block is 127 blocks, buffer address needs to be 4-byte aligned.
- For SDH SPI, the maximum synchronous transmission data length is 65535 bytes, and the maximum asynchronous transmission data length is 65532 bytes. Asynchronous transmission data needs to be 4-byte aligned.
- SDH SDMMC and SDH SPI use related underlying hardware, so they cannot be used simultaneously.

Application Example
--------------------------

For a basic example of using the SDIO HOST,please refer to:ref:`examples/peripheral/sdio_host<peripheral_example>`

API Reference
--------------------------

:ref:`API Reference <sdio_host_api>`
