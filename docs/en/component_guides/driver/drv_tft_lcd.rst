.. _tft_lcd:

TFT_LCD
=============

Introduction
---------------

Thin-Film Transistor Liquid Crystal Display (TFT-LCD) is a display that uses the electro-optic effect of liquid crystal materials to display images.
TFT-LCD technology is currently a display technology widely used in televisions, computer monitors, mobile phones, tablets and other devices.

The TFT-LCD display is composed of multiple layers, including a backlight layer, a polarization layer, a color filter, a liquid crystal layer, a thin-film transistor array, and a front panel. 
Its working principle is that the backlight layer emits light. After passing through the polarization layer, the polarization direction of the light is changed. 
The liquid crystal molecules in the liquid crystal layer change their arrangement under the action of an electric field, and then change the polarization state of the light. 
The thin-film transistor array controls the voltage at each pixel point, thereby controlling the arrangement of liquid crystal molecules and realizing the display of images.

Display Architecture
-------------------------

In WM IOT SDK, the display module is divided into four layer:

1. Application layer (applications developed by users based on the middle layer or directly based on the driver layer).
2. Middle layer (integrating LVGL to achieve functions such as points and lines).
3. Driver layer (for the implementation of device drivers for various screens).
4. Communication layer (such as SDIO, SPI, I2C, etc. W80X does not support RGB, I80, MIPI interfaces).


Driver Layer Feature List
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Support LCD parameter configuration (mainly through the Device Table, including SPI frequency, screen pin configuration, etc.).
- Support drawing functions (including the position of the drawing window and the transmission of image data).
- Support asynchronous and synchronous transmission of drawing content.
- Support screen rotation function (simply control the x and y axis order of the screen).
- Support screen configuration parameter acquisition function (mainly obtained from the Device Table).
- Support screen command transmission (the application can send special commands for the screen).
- Support screen backlight control.

Driver usage method
^^^^^^^^^^^^^^^^^^^^^^^^^

Currently, the driver layer supports multiple TFT LCD screens, such as screens driven by NV3041A and ST7735.
The W80X chip can send image data to the screen at high speed through the SPI mode of SDIO.
The driver layer provides a unified API interface to facilitate users to control the screen.
The following is an introduction to the usage methods in some scenarios.

Display Pictures (CPU Polling)
"""""""""""""""""""""""""""""""""""
Starting conditions:

- Refer to the readme instructions of the tft_lcd_polling example and connect the hardware pins of the screen and the development board properly. `Polling Example description <../../examples_reference/examples/peripheral/tft_lcd/tft_lcd_polling/README_EN.html>`_.
- Configure the corresponding device parameters in the device table, including the device table parameters of the SDIO Controller and TFT LCD.
- Prepare the picture content to be displayed. In the current example, use an image conversion tool (such as lvgl Image Converter) to convert the picture into a const array in RGB565 format and compile it into the firmware and burn it to flash.

API call sequence:

- Call ``wm_drv_sdh_spi_init`` to initialize the SDIO Controller Driver.
- Call ``wm_drv_tft_lcd_init`` to initialize the TFT LCD Device Driver.
- Call ``wm_drv_tft_lcd_set_backlight`` to turn on the backlight of the TFT LCD.
- Call ``malloc`` to apply for a temporary buffer ``buf1`` (optional).
- Call ``wm_drv_tft_lcd_draw_bitmap`` to send the picture content in ``buf1`` to the GRAM of the TFT LCD in the window set by this function.

Result:

- The TFT LCD screen will display pictures.


.. note:: 

    - ``buf1`` is optional. It is only needed if the picture is stored in Flash because DMA cannot directly access Flash.
    - ``buf1`` is limited by the size of the hardware SRAM and may not be able to temporarily store an entire picture. In this case, it can be divided into multiple blocks for storage and transmission.

      For example, for a picture with a resolution of 480 x 272, if the RGB565 Pixel Format is used, its data volume is 480 x 272 x 2 = 261120 bytes.
      In this scenario, the picture can be divided into four blocks. The application layer applies for a ``buf1`` with a size of 65280 bytes for block-by-block caching and transmission.

    - The picture format used must be consistent with the preset pixel-format in the device driver.

      + In the default TFT LCD driver of NV3041A, the supported Color Mode is RGB565. The specific setting is in the initialization sequence of the ST7735 driver ``st7735_init_seq[]``.
      + In the default TFT LCD driver of ST7735, the supported Color Mode is RGB565. The specific setting is in the initialization sequence of the NV3041A driver ``nv3041a_init_seq[]``.


Display Pictures (DMA)
"""""""""""""""""""""""""""""""""""

Starting conditions:

- Refer to the readme instructions of the tft_lcd_dma example and connect the hardware pins of the screen and the development board properly. `DMA Example description <../../examples_reference/examples/peripheral/tft_lcd/tft_lcd_dma/README_EN.html>`_.
- Configure the corresponding device parameters in the device table, including the device table parameters of the SDIO Controller and TFT LCD.
- Prepare the picture content to be displayed. In the current example, use an image conversion tool (such as lvgl Image Converter) to convert the picture into a const array in RGB565 format and compile it into the firmware and burn it to flash.

API call sequence:

- Create a semaphore, for example: ``lcd_demo_sem``.
- Call ``wm_drv_sdh_spi_init`` to initialize the SDIO Controller Driver.
- Call ``wm_drv_tft_lcd_init`` to initialize the TFT LCD Device Driver.
- Call ``wm_drv_tft_lcd_set_backlight`` to turn on the backlight of the TFT LCD.
- Call ``malloc`` to apply for a temporary buffer ``buf1`` (optional. It is only needed if the picture is stored in Flash because DMA cannot directly access Flash).
- Call ``wm_drv_tft_lcd_register_tx_callback`` to register the tx done callback and release ``lcd_demo_sem`` after the previous data transmission is completed.
- Call ``xSemaphoreTake`` to obtain ``lcd_demo_sem``. If it is not obtained, the current task will enter a blocked state.
- Call ``wm_drv_tft_lcd_draw_bitmap`` to send the picture content in ``buf1`` to the GRAM of the TFT LCD in the window set by this function.

Result:

- The TFT LCD screen displays pictures.

.. note::

    - ``buf1`` is optional. It is only needed if the picture is stored in Flash because DMA cannot directly access Flash.
    - ``buf1`` is limited by the size of the hardware SRAM and may not be able to temporarily store an entire picture. In this case, it can be divided into multiple blocks for storage and transmission.

      For example, for a picture with a resolution of 480x272, if the RGB565 Pixel Format is used, its data volume is 480 x 272 x 2 = 261120 bytes.
      In this scenario, the picture can be divided into four blocks. The application layer applies for a ``buf1`` with a size of 65280 bytes for block-by-block caching and transmission.

    - The picture format used should be consistent with the preset pixel-format in the device table.
    - Compared to the CPU Polling method, this DMA display scheme can improve CPU utilization.
    - When the tx done callback is registered, ``wm_drv_tft_lcd_draw_bitmap`` will internally default to considering DMA transmission, but there are exceptions.

      1. The total length of the transmitted data is less than 32 bytes because the efficiency improvement of using DMA for this length is not significant.
      2. The starting address of the transmitted data is not a 4-byte alignment address, that is, not an integer multiple of 4.
      3. The total length of the transmitted data is not a 4-byte alignment address, that is, not an integer multiple of 4.

    - In case of 2 and 3, ``wm_drv_tft_lcd_draw_bitmap`` will internally segment this data for transmission.
      Data segments that meet 4 Byte alignment will be transmitted by DMA, and the remaining will be transmitted by CPU Polling.


Application Example
"""""""""""""""""""""""""""""""""""

  :ref:`examples/peripheral/tft_lcd<peripheral_example>`


API Reference
------------------

  To find TFT LCD related APIs, please refer to:

    :ref:`label_api_tft_lcd`

Method for adding TFT LCD device driver
-------------------------------------------

The following figure is the TFT LCD device driver framework.

.. figure:: ../../../_static/component-guides/driver/tft_lcd_arch_en.svg
    :align: center
    :alt: TFT LCD device driver architecture

If you want to add a device driver for a new SPI TFT LCD device, it is recommended to maintain the current hierarchical structure. 
In most cases, only the yellow-related levels need to be modified.

First step: Create device information in the device table.

.. code-block::

    const static wm_dt_hw_tft_lcd_spi_t dt_hw_nv3041a = {
        .init_cfg = { .init_level = 0, .init_priority = 0 },
        .spi_cfg = {
            .mode = 0,
            .freq = 60000000,               /* Hz clock */
            .pin_cs = {
                .pin_num = WM_GPIO_NUM_27,  /**< CS - PB11 */
                .pin_mux = WM_GPIO_IOMUX_FUN5,
            },
        },
        .io_lcd_reset = WM_GPIO_NUM_26,     /**< RST - PB10 */
        .io_lcd_led = WM_GPIO_NUM_32,       /**< LED - PB16 */
        .io_lcd_dcx = WM_GPIO_NUM_25,       /**< DCX - PB9 */
        .io_lcd_te = WM_GPIO_NUM_24,        /**< TE  - PB8 */
        .spi_device_name = "sdspi",
        .gpio_device_name="gpio",
    };

.. note:: 

    - The modification method of the device table can refer to the relevant chapters: :ref:`pinmux` and  :ref:`lable_device_table` .
    - The ``io_lcd_**`` configuration in the device table must strictly match the IO selected by the development board you are using.
    - If ``io_lcd_te`` is not supported, it can be assigned as ``WM_GPIO_NUM_MAX``.

Second step: Create a header file for the new device.

In this file, mainly two types of data are stored:

- The command set of the new TFT LCD device, presented in macro definition (optional).
- The initialization command table of the new TFT LCD device.

An example of the initialization command table is as follows:

.. code-block::

  const uint8_t nv3041a_init_seq[] = {
    //Format: len , delay, cmd_type,  cmd, data ... 
    //0x03, 0x00, LCD_CMD_TYPE_16BIT, 0x12, 0x34, 0xBE, // Example:16bit command "0x3412" be used
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xFF, 0xA5,
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xE7, 0x10, // TE_output_en
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x35, 0x01, // TE_interface_en 01
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x36, 0x00,
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x3A, 0x01, // 01---565，00---666 (color mode)
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x40, 0x01, // 01:IPS/00:TN
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x44, 0x15, // VBP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x45, 0x15, // VFP 21
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7D, 0x03, // vdds_trim[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC1, 0xBB, // avdd_clp_en avdd_clp[1:0] avcl_clp_en avcl_clp[1:0] 0xbb 88 a2
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC2, 0x05, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC3, 0x10, // vgl_clp_en vgl_clp[2:0]
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC6, 0x3E, // avdd_ratio_sel avcl_ratio_sel vgh_ratio_sel[1:0] vgl_ratio_sel[1:0] 35
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC7, 0x25, // mv_clk_sel[1:0] avdd_clk_sel[1:0] avcl_clk_sel[1:0] 2e
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0xC8, 0x11, // VGL_CLK_sel
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x7A, 0x5F, // user_vgsp  4f:0.8V 3f:1.04V 5f
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x6F, 0x44, // user_gvdd  1C:5.61 5f 53 2a 3a
    0x02, 0x00, LCD_CMD_TYPE_8BIT, 0x78, 0x70, // user_gvcl  50:-3.22 75 58 66
    ..
    0x0 , //End Byte
  };


.. note:: 

    - The command initialization table is designed to uniformly manage the various initialization timing commands of TFT LCD devices.
    - The format of the command initialization table must follow the current design requirements, that is, 
      ``len(1Byte)+delay(1Byte)+cmd type(1Byte)+cmd(1~2 Byte)+data(len-cmd actual length)``.

       + Among them, ``len = cmd + data``. When the parsing function encounters a line with ``len`` = 0, it is considered an end symbol and exits execution.
       + When cmd type is ``LCD_CMD_TYPE_8BIT``, cmd is 1 byte.
       + When cmd type is ``LCD_CMD_TYPE_16BIT``, cmd is 2 bytes.
       + The unit of ``delay`` is millisecond.

    - The initialization commands and sequences of each TFT LCD device may be different. Refer to the specification sheet or driver code of the LCD device manufacturer to set them.
    - The parsing function of the command initialization table is placed in ``lcd_init_cmd()``. The new LCD device driver can continue to use it.


Third step: Create the main driver file for the new device.

Because of the first step of configuring the hardware and the second step of configuring the initialization list, 
and there are already operation interfaces that encapsulate the underlying I/O, the main driver file for the new device becomes simpler.

For the main driver of the new device, you can copy an existing LCD device driver file, such as ``wm_drv_ops_nv3041a_spi.c`` and ``wm_drv_ops_nv3041a_spi.h`` and rename them.
Check whether there are differences between the logic of the ``Ops`` function implemented inside and the expected behavior of the new device.

Generally speaking, only the following places need to be mainly changed:

- Replace the commands defined by the prefixes ``NV3041_LCD_CMD_`` and ``NV3041A_CFG_MADCTL_``.
- Replace the function names defined by the prefix ``wm_drv_ops_nv3041a``.
- In ``lcd_init_cmd()``, reference the new command initialization table.

Fourth step: Improve configuration files.

There are two configuration files here. After adding the new screen driver, you also need to add information to these files. This is to ensure that the newly added LCD Device can be selected through the menuconfig UI during compilation.

- File 1: ``wm_drv_tft_lcd_cfg.h``

  + This file is used to manage the ``Device Name`` as well as information such as ``Resolution`` and ``Rotation`` of all LCD Devices.
  + Among them, the ``Device Name`` must be consistent with the content of ``Device Name`` of the selected screen device in the device table.
  + File 1 will determine the effective LCD Device and its basic information according to the configuration in File 2 and the LCD Device selected by the user through ``menuconfig``.

- File 2: ``components\driver\Kconfig``

  + This file is used to manage the Compile Option of all LCD Devices.


.. code-block::

    //file: wm_drv_tft_lcd_cfg.h
    //This file provides a comprehensive summary of basic information related to LCD devices.
    //  Special notice:
    //  - The device names listed herein must precisely match the definitions in the device table.
    //  - When a new LCD device is added, it is essential to add new options to the COMPONENT_DRIVER_TFT_LCD_OPTIONS 
    //    managed by menuconfig, which is located in components/driver/kconfig.

    // List all lcd device's device name
    #define DEV_NAME_NV3041A_SPI "nv3041a_spi"
    #define DEV_NAME_ST7735_SPI  "st7735_spi"


    //List all lcd device's resoultion and rotation info
    #if defined(CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI)
    #define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_NV3041A_SPI
    #define WM_CFG_TFT_LCD_X_RESOLUTION 480  //Horizontal display resolution in LCD spec,independent of WM_CFG_TFT_LCD_ROTATION.
    #define WM_CFG_TFT_LCD_Y_RESOLUTION 272  //Vertical display resolution in LCD spec, independent of WM_CFG_TFT_LCD_ROTATION.
    #define WM_CFG_TFT_LCD_ROTATION     0    //The rotation to be used.
    #define WM_CFG_TFT_LCD_PIXEL_WIDTH  2    //The pixel width to be used, RGB565(2 Bytes).

    #elif defined(CONFIG_COMPONENT_DRIVER_LCD_ST7735_SPI)
    #define WM_CFG_TFT_LCD_DEVICE_NAME  DEV_NAME_ST7735_SPI
    #define WM_CFG_TFT_LCD_X_RESOLUTION 128
    #define WM_CFG_TFT_LCD_Y_RESOLUTION 160
    #define WM_CFG_TFT_LCD_ROTATION     0
    #define WM_CFG_TFT_LCD_PIXEL_WIDTH  2
    #endif
