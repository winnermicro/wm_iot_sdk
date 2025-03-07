.. _driver:

外设驱动
==============

驱动架构
^^^^^^^^^^^^

.. figure:: ../../../_static/component-guides/driver/drv_arch_framework.svg
    :align: center
    :alt: 驱动架构图


- LL (Low Layer)

  用于封装各 Chip 寄存器操作，避免应用层直接调用寄存器接口。

- HAL (Hardware Adaption Layer)

  基于 LL 层接口实现基础硬件功能。

- Controller Driver

  基于 HAL 层接口 实现 Chip 内各控制器的各种功能，具有 OS 依赖性，执行互斥，信号量等资源保护能力。
  （控制器种类如: I2C, SPI, DMA, Timer, WDT, I2S, Clock, USB, SDIO, IRQ, PWM, ADC, LCD, Touch, Uart, RTC, PMU, Crypto Engin, GPIO 等）

- Device Driver

  基于 Controller Driver 接口，实现 Chip 外部设备驱动功能（如: 传感器, EEPROM, SIP Flash, TFT 屏 ……）

- Device Manager

  设备管理单元, 用于统一管理当前工程的所有设备 (Controller 及外部设备) 配置，类似 Linux 设备树功能

用户可参考 Example, 使用 Controller Driver API 进行应用程序开发。


驱动介绍
^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Uart <drv_uart>
   I2C <drv_i2c>
   EEPROM <drv_eeprom>
   EEPROM 驱动开发 <drv_eeprom_dev>
   Flash <drv_flash>
   RCC <drv_rcc>
   SPI Master <drv_spim>
   GPIO <drv_gpio>
   IRQ <drv_irq>
   TIMER <drv_timer>
   DMA <drv_dma>
   WDT <drv_wdt>
   TFT_LCD <drv_tft_lcd>
   SDIO_HOST <drv_sdio_host>
   SDIO_SLAVE <drv_sdio_slave>
   PWM <drv_pwm>
   RTC <drv_rtc>
   PSRAM <drv_psram>
   触摸传感器 <drv_touch_sensor>
   ADC <drv_adc>
   SEG_LCD <drv_seg_lcd>
   CRC <drv_crc>
   CRYPTO <drv_crypto>
   HASH <drv_hash>
   RNG <drv_rng>
   RSA <drv_rsa>
   I2S <drv_i2s>
   CODEC I2S <drv_codec_i2s>


外设驱动的 menuconfig 配置
-----------------------------

主要配置如下：

.. list-table::
   :widths: 50 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_COMPONENT_DRIVER_RCC_ENABLED
     - 是否启用 RCC 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_PMU_ENABLED
     - 是否启用 PMU 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_IRQ_ENABLED 
     - 是否启用 IRQ 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_DMA_ENABLED 
     - 是否启用 DMA 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
     - 是否启用 Flash 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED
     - 是否启用 FLASH 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_TIMER_ENABLED 
     - 是否启用 Timer 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_UART_ENABLED
     - 是否启用 UART 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_GPIO_ENABLED 
     - 是否启用 GPIO 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
     - 是否启用 PSRAW 驱动
     - N     

   * - CONFIG_COMPONENT_DRIVER_PWM_ENABLED 
     - 是否启用 PWM 驱动
     - N     

   * - CONFIG_COMPONENT_DRIVER_RTC_ENABLED  
     - 是否启用 RTC 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED 
     - 是否启用 SDIO Host (SDMMC) 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED 
     - 是否启用 SDIO Host (SPI Mode) 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED
     - 是否启用 SPI Master 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_WDT_ENABLED
     - 是否启用 WATCH Dog 驱动
     - Y 

   * - CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED
     - 是否启用 TFT LCD 驱动
     - N  

   * - CONFIG_COMPONENT_DRIVER_ADC_ENABLED
     - 是否启用 ADC 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_CRC_ENABLED
     - 是否启用 CRC 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED 
     - 是否启用 CRYPTO 驱动
     - N

   * - CONFIG_COMPONENT_DRIVER_HASH_ENABLED 
     - 是否启用 HASH 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_RNG_ENABLED
     - 是否启用 RNG 驱动
     - Y

   * - CONFIG_COMPONENT_DRIVER_RSA_ENABLED    
     - 是否启用 RSA 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED    
     - 是否启用 SEG LCD 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED   
     - 是否启用 Touch Sensor 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_I2C_ENABLED   
     - 是否启用 I2C 驱动
     - N 

   * - CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED 
     - 是否启用 SDIO Slave 驱动
     - N 


如何添加一个新的设备驱动
^^^^^^^^^^^^^^^^^^^^^^^^^

基于现在的驱动架构，若要添加一个新的设备驱动基本上有如下步骤和注意事项

1. Pinmux 的需求调研及配置，具体可参考 :ref:`pinmux`
   
2. Device Table 中添加新设备 配置，具体可参考 :ref:`lable_device_table`

3. 规划 新device 的 ``wm_device_t`` 结构体， 例如：

.. figure:: ../../../_static/component-guides/driver/eeprom_drv_data_structure.svg
    :align: center
    :alt: EEPROM数据结构

4. 规划 新device 驱动架构,包含 ops 定义， 防重入机制的设计， 例如：

.. figure:: ../../../_static/component-guides/driver/eeprom_drv_arch.svg
    :align: center
    :alt: EEPROM驱动架构

5. 文件目录管理

.. figure:: ../../../_static/component-guides/driver/eeprom_sdk_folder.svg
    :align: center
    :alt: EEPROM目录结构

.. note::
  外设驱动中 尽量避免对 HAL 接口的调用，原因是 HAL 接口内部无防重入保护，多个任务直接调用时可能会产生资源访问冲突或死锁问题。
