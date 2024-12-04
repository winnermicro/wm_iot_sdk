.. _drv_sdio_host:

SDIO Host
=============

简介
-------------

SDIO Host 模块包括 SDH SDMMC 和 SDH SPI

- **SDIO Host:** 是一种用于嵌入式系统和计算机的接口，它允许设备通过 Secure Digital Input Output (SDIO) 接口与各种类型的 Secure Digital (SD) 卡进行通信

- **SDH SDMMC:** 主要用于读写 SD 卡，包括传统的 SD 卡、SDHC (Secure Digital High Capacity) 卡

- **SDH SPI:** SDIO 接口的 SPI 模式，允许设备通过 SPI (Serial Peripheral Interface) 协议与 SPI 设备进行通信，主要用于驱动那些只需要单向通信的设备，例如 TFT (Thin Film Transistor) 显示屏

功能列表
-------------

- SDH SDMMC 存储卡读取写入。
- SDH SPI 同步传输数据。
- SDH SPI 异步传输数据。


功能概述
-------------

SDH SDMMC 存储卡读写：

- 支持传统的 SD 卡和 SDHC 卡，实现了对这些存储介质的高效读写操作
- 允许用户访问和存储大量数据，包括文本、图像、音频和视频文件
- SDK 基于 SDH SDMMC 驱动适配了 FATFS ，因此可直接使用 FATFS 的 API 进行 SD 卡文件的读写，可参考 :ref:`examples/storage/fatfs/sdmmc_disk<storage_example>`

SDH SPI 同步传输数据：

- 通过 SDIO 接口的 SPI 模式同步传输数据
- 主要用于传输少量数，比如驱动 TFT 屏的 CMD 传输

SDH SPI 异步传输数据：

- 通过 SDIO 接口的 SPI 模式异步传输数据
- 主要用于传输大量数，比如驱动 TFT 屏的大量显示数据传输


硬件配置
-------------

| 通过修改 DT 进行配置。

- SDH SDMMC 数据宽度可配置成 1bit(0) 和  4bits(1)
- SDH SDMMC/SPI 时钟频率支持 1/2, 1/4, 1/6, 1/8, 1/10, 1/12, 1/14, 1/16 CPU clock, 若 CPU clock 为 240MHz, 则支持的时钟频率为 120, 60, 40, 30, 24, 20, ≈17.14, 15 MHz，建议时钟不得超过60MHz
- SDH SPI 仅支持模式 0 (CPOL=0, CPHA=0)
- SDH SPI 引脚对应关系如下，详细的 pinmux 功能复用请参考 PINMUX 章节：

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - SDH_SPI 引脚
     - SDH_SDMMC 引脚

   * - SDH_SPI_CLK
     - MMC_CLK_x (x 可以取 0/1)

   * - SDH_SPI_MOSI
     - MMC_CMD_x (x 可以取 0/1)

   * - SDH_SPI_MISO
     - 半双工，不能接收

   * - SDH_SPI_CS
     - 任意没有使用的普通 GPIO 都可以

主要功能
-------------

SDH SDMMC 设备数据读取写入
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**起始条件:**

- SDH SDMMC 初始化

**相关时序API:**

- 调用 ``wm_drv_sdh_sdmmc_init`` 对 SDH SDMMC 模块初始化
- 调用 ``wm_drv_sdh_sdmmc_read_blocks`` 从 SDH SDMMC 设备读取数据块
- 根据 ``wm_drv_sdh_sdmmc_write_blocks`` 向 SDH SDMMC 设备写入数据块

**结果:**

- 成功返回 ``return WM_ERR_SUCCESS`` ,失败返回对应错误码


SDH SPI 设备数据传输接收数据
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**起始条件:**

- 配置 SPI 模块
- SDH SPI 初始化

**相关时序API:**

- 调用 ``wm_drv_sdh_sdspi_init`` 对 SDH SPI 模块初始化
- 调用 ``wm_drv_sdh_sdspi_transceive_sync`` 通过 SDH SPI 设备同步传输和接收数据
- 根据 ``wm_drv_sdh_sdspi_transceive_async`` 通过 SDH SPI 设备异步传输和接收数据

**结果:**

- 成功返回 ``return WM_ERR_SUCCESS`` ,失败返回对应错误码

注意事项
-------------

- SDH SDMMC 存储卡读写，单块数据长度为 512 bytes, 最大传输数据块为 127 block
- SDH SPI 同步最大传输数据长度为 65535 bytes，异步最大传输数据长度为 65532 bytes，且异步传输数据需 4 bytes 对齐
- SDH SDMMC 和 SDH SPI 使用了相关的底层硬件，因此不能同时使用

应用实例
------------------

    使用 SDIO HOST 基本示例请参照 :ref:`examples/peripheral/sdio_host<peripheral_example>`

API参考
-------------
:ref:`API参考 <sdio_host_api>`
