# TFT_LCD_DMA

## Overview

This application serves as a comprehensive demonstration of the TFT LCD display functionality using the WM IoT SDK. It illustrates the process of initializing the TFT LCD device and executing a range of display operations. These operations include a continuous blue background refresh, displaying pre-stored images from flash memory, and rotating the screen on the X and Y axes.
The program interfaces with the TFT LCD module via the SDIO (SPI mode) and utilizes DMA for communication.


## Requirements

1. TFT LCD Display Requirement：

   - Manufacturer： New Vision Microelectronics Inc.
   - Model：NV3041A

   The application utilizes a TFT LCD screen that can be replaced, provided that three conditions are met:
   a) The device information for the TFT LCD screen must be pre-configured in the Device Table.
   b) This application reference program references the macro definition 'WM_LVGL_LCD_MODULE_NAME' in wm_drv_tft_lcd_cfg.h. Its value should be configured as the device name of the preset TFT LCD screen device in the Device Table. And select the LCD device in menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device, or directly define the Compile Option of the required selected LCD device in prj.config, such as "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y".
   c) The device driver for the TFT LCD screen needs to be properly adapted, primarily ensuring that the ops functions of its driver implementation are realized and associated with the dt_hw_table_entry[] in the Device Table.

2. Pin Connections:

   The hardware configuration is stored in the Device Table, which includes pin definitions for the SDIO device and clock settings for SDIO (SPI Mode), as well as other pin definitions for the TFT LCD device.
   Both the SDIO device and the TFT LCD device can be defined and queried by their device names in the Device Table.
   The definition file for the Device Table is wm_dt_hw.c, but there may be variations depending on the device table file referenced by the project.
   The following figure illustrates an example of the correspondence between the W801S GPIO pin numbers and the NV3041A PINs. However, the actual configuration should refer to the settings in the Device Table.

   | GPIO_NUM       | Pin Number | Connection Description | NV3041A PIN |
   | -------------  |----------- | ---------------------- | ----------- |
   | WM_GPIO_NUM_22 | PB6        | CLK                    | 11          |
   | WM_GPIO_NUM_23 | PB7        | MOSI                   | 13          |
   | WM_GPIO_NUM_27 | PB11       | CS                     | 9           |
   | WM_GPIO_NUM_26 | PA13       | RESET                  | 15          |
   | WM_GPIO_NUM_32 | PA16       | LED                    | 36          |
   | WM_GPIO_NUM_25 | PA2        | DCX                    | 10          |
   | WM_GPIO_NUM_24 | PB25       | TE                     | 8           |


## Building and Flashing

Example Location：`examples\tft_lcd\tft_lcd_dma`

During compilation, please ensure that the appropriate CONFIG_BUILD_TYPE_XXX is selected and confirm that the referenced Device Table matches the development board being used for the experiment.
For higher picture display efficiency, users can set the RCC CPU Clock to a relatively high value, such as 240MHz, through the device table.

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Successfully running will output the following logs

```
[I] (7) lcd_example: init rcc, mcu clock as 240MHz.
[I] (711) lcd_example: LCD x_resolution = 480
[I] (711) lcd_example: LCD y_resolution = 272
[I] (711) lcd_example: LCD rotation = 0

[I] (711) lcd_example: DEMO:block_size=38400
[I] (712) lcd_example: wm_lcd_demo show blue background
[I] (2853) lcd_example: wm_lcd_demo show image(w=480, h=272)
[I] (5044) lcd_example: wm_lcd_demo show blue background
[I] (7185) lcd_example: wm_lcd_demo show image(w=480, h=272)
```