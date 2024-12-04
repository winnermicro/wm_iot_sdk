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
  (Types of controllers include: I2C, SPI, DMA, Timer, WDT, I2S, Clock, USB, SDIO, IRQ, PWM, ADC, LCD, Touch, Uart, RTC, PMU, Crypto Engine, GPIO, etc.)

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
   PWM <drv_pwm>
   RTC <drv_rtc>
   PSRAM <drv_psram>
   CRC <drv_crc>
   CRYPTO <drv_crypto>
   Hash <drv_hash>
   RNG <drv_rng>
   RSA <drv_rsa>
.. I2S <drv_i2s>

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

