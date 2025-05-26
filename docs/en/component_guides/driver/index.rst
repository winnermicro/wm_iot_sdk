.. _driver:

Peripheral Drivers
===================

Driver Architecture
^^^^^^^^^^^^^^^^^^^^^^

.. figure:: ../../../_static/component-guides/driver/drv_arch_framework.svg
    :align: center
    :alt: Driver Architecture Diagram

- LL (Low Layer)

  Used for encapsulating the operations of various chip registers to prevent the application layer from directly calling register interfaces.

- HAL (Hardware Adaption Layer)

  Implements basic hardware functions based on LL layer interfaces.

- Controller Driver

  Implements various functionalities of controllers within the Chip based on HAL layer interfaces.
  It is OS-dependent and capable of executing mutex, semaphore, and other resource protection functions. 
  (Types of controllers include: I2C, SPI, HSPI slave, DMA, Timer, WDT, I2S, Clock, USB, SDIO, IRQ, PWM, ADC, LCD, Touch, Uart, RTC, PMU, Crypto Engine, GPIO, etc.)

- Device Driver

  Implements the driving functions of external devices of the chip based on Controller Driver interfaces (e.g., sensors, EEPROM, SIP Flash, TFT screens, etc.).

- Device Manager

  A device management unit used for unified management of the configurations of all devices (controllers and external devices) in the current project, similar to the Linux device tree function.

Users can refer to Examples and use Controller Driver APIs for application development.


Driver Introduction
^^^^^^^^^^^^^^^^^^^^^

.. toctree::
   :maxdepth: 1

   Touch Sensor <drv_touch_sensor>
   Uart <drv_uart>
   I2C <drv_i2c>
   EEPROM <drv_eeprom>
   EEPROM Driver Development <drv_eeprom_dev>
   Flash <drv_flash>
   RCC <drv_rcc>
   SPI Master <drv_spim>
   SPI Slave <drv_spis>
   HSPI_SLAVE <drv_hspi_slave>
   GPIO <drv_gpio>
   IRQ <drv_irq>
   ADC <drv_adc>
   TIMER <drv_timer>
   DMA <drv_dma>
   WDT <drv_wdt>
   SEG_LCD <drv_seg_lcd>
   TFT_LCD <drv_tft_lcd>
   SDIO_HOST <drv_sdio_host>
   SDIO_SLAVE <drv_sdio_slave>
   Touch_Panel <drv_touch_panel>
   PWM <drv_pwm>
   RTC <drv_rtc>
   PSRAM <drv_psram>
   CRC <drv_crc>
   CRYPTO <drv_crypto>
   Hash <drv_hash>
   RNG <drv_rng>
   RSA <drv_rsa>
   I2S <drv_i2s>
   CODEC I2S <drv_codec_i2s>


Menuconfig configuration for peripheral drivers
-------------------------------------------------

The main configuration is as follows:

.. list-table::
  :widths: 50 50 25
  :header-rows: 0
  :align: center

  * - Configuration name
    - Configuration description
    - Default values

  * - CONFIG_COMPONENT_DRIVER_RCC_ENABLED
    - Whether to start the RCC driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_PMU_ENABLED
    - Enable the PMU driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_IRQ_ENABLED
    - Whether to start the IRQ driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_DMA_ENABLED
    - Enable the DMA driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_INTERNAL_FLASH_ENABLED
    - Enable the internal Flash driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_EXTERNAL_FLASH_ENABLED
    - Enable the external FLASH driver
    - N

  * - CONFIG_COMPONENT_DRIVER_TIMER_ENABLED
    - Enable the Timer driver
    - N

  * - CONFIG_COMPONENT_DRIVER_UART_ENABLED
    - Enable the UART driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_GPIO_ENABLED
    - Enable the GPIO driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED
    - Enable the PSRAW driver
    - N

  * - CONFIG_COMPONENT_DRIVER_PWM_ENABLED 
    - Enable PWM driver
    - N     

  * - CONFIG_COMPONENT_DRIVER_RTC_ENABLED  
    - Enable RTC driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_SDMMC_ENABLED
    - Enable the SDIO Host (SDMMC) driver
    - N

  * - CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED
    - Enable the SDIO Host (SPI Mode) driver
    - N

  * - CONFIG_COMPONENT_DRIVER_SDSPI_ENABLED
    - Enable the SPI Master driver
    - N

  * - CONFIG_COMPONENT_DRIVER_WDT_ENABLED
    - Enable the WATCH Dog driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_TFT_LCD_ENABLED
    - Enable the TFT LCD driver
    - N

  * - CONFIG_COMPONENT_DRIVER_ADC_ENABLED
    - Enable the ADC driver
    - N

  * - CONFIG_COMPONENT_DRIVER_CRC_ENABLED
    - Enable the CRC driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_CRYPTO_ENABLED
    - Enable the CRYPTO driver
    - N

  * - CONFIG_COMPONENT_DRIVER_HASH_ENABLED
    - Enable the HASH driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_RNG_ENABLED
    - Enable the RNG driver
    - Y

  * - CONFIG_COMPONENT_DRIVER_RSA_ENABLED
    - Enable the RSA driver
    - N

  * - CONFIG_COMPONENT_DRIVER_SEG_LCD_ENABLED
    - Enable the SEG LCD driver
    - N

  * - CONFIG_COMPONENT_DRIVER_TOUCH_SENSOR_ENABLED
    - Enable the Touch Sensor driver 
    - N

  * - CONFIG_COMPONENT_DRIVER_I2C_ENABLED
    - Enable the I2C driver
    - N

  * - CONFIG_COMPONENT_DRIVER_SDIO_SLAVE_ENABLED
    - Whether to start the SDIO Slave driver
    - N

  * - CONFIG_COMPONENT_DRIVER_HSPI_SLAVE_ENABLED
    - Whether to start the HSPI Slave driver
    - N

How to Add a New Device Driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Based on the current driver architecture, adding a new device driver generally involves the following steps and considerations

1. Research and Configure Pinmux Requirements,Refer to :ref:`pinmux`
   
2. Add New Device Configuration in the Device Table,Refer to :ref:`lable_device_table`

3. Plan the ``wm_device_t`` Structure for the New Device, For example :

.. figure:: ../../../_static/component-guides/driver/eeprom_drv_data_structure_en.svg
    :align: center
    :alt: EEPROM Data Structure

4. Plan the New Device Driver Architecture, Including Ops Definition and Reentrant Mechanism Design : For example

.. figure:: ../../../_static/component-guides/driver/eeprom_drv_arch.svg
    :align: center
    :alt: EEPROM Data Structure

5. File Directory Management

.. figure:: ../../../_static/component-guides/driver/eeprom_sdk_folder.svg
    :align: center
    :alt: EEPROM Directory Structure

.. note::
  In peripheral drivers, it is advisable to avoid calling HAL interfaces directly. The reason is that HAL interfaces lack reentrant protection, and direct calls from multiple tasks may lead to resource access conflicts or deadlock

