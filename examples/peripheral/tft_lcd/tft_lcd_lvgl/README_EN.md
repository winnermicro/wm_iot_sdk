# TFT_LCD_LVGL

## Overview

This application is a demonstration of `LVGL` functionalities using the `WM IoT SDK`. It illustrates how to initialize the TFT LCD device and create an LVGL DEMO Task for the initialization of the LVGL module, showcasing various native LVGL demo scenarios such as: Widgets, Music Player, Benchmark, Stress etc.
The program interfaces with the TFT LCD module via the SDIO (SPI mode) and utilizes DMA/CPU Polling for communication. 


## Requirements

1. TFT LCD Display Requirement：

   - Manufacturer： New Vision Microelectronics Inc.
   - Model：NV3041A

   The application utilizes a TFT LCD screen that can be replaced, provided that three conditions are met:
   a) The device information for the TFT LCD screen must be pre-configured in the Device Table.
   b) This application reference program references the macro definition `WM_LVGL_LCD_MODULE_NAME` in `wm_drv_tft_lcd_cfg.h`. Its value should be configured as the device name of the preset TFT LCD screen device in the Device Table. And select the LCD device in `menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device`, or directly define the Compile Option of the required selected LCD device in `prj.config`, such as `CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y`.
   c) The device driver for the TFT LCD screen needs to be properly adapted, primarily ensuring that the  `ops` functions of its driver implementation are realized and associated with the `dt_hw_table_entry[]` in the Device Table.

2. Pin Connections:

   The hardware configuration is stored in the Device Table, which includes pin definitions for the SDIO device and clock settings for SDIO (SPI Mode), as well as other pin definitions for the TFT LCD device.
   Both the SDIO device and the TFT LCD device can be defined and queried by their device names in the Device Table.
   The definition file for the Device Table is `wm_dt_hw.c`, but there may be variations depending on the device table file referenced by the project.
   
   The following figure illustrates an example of the correspondence between the W801S GPIO pin numbers and the NV3041A PINs. However, the actual configuration should refer to the settings in the Device Table.

   | GPIO_NUM       | Pin Number | Connection Description | NV3041A PIN |
   | -------------  |----------- | ---------------------- | ----------- |
   | WM_GPIO_NUM_22 | PB6        | CLK                    | 11          |
   | WM_GPIO_NUM_23 | PB7        | MOSI                   | 13          |
   | WM_GPIO_NUM_27 | PB11       | CS                     | 9           |
   | WM_GPIO_NUM_26 | PB10       | RESET                  | 15          |
   | WM_GPIO_NUM_32 | PB16       | LED                    | 36          |
   | WM_GPIO_NUM_25 | PB9        | DCX                    | 10          |
   | WM_GPIO_NUM_24 | PB8        | TE                     | 8           |


## Building and Flashing

Example Location：`examples\tft_lcd\tft_lcd_lvgl`

The selection of the demo scenario is to select the required demo option in `menuconfig -> LVGL -> Demos`, or directly define the compile option of the required selected demo scenario in `prj.config`. For example, turn on `CONFIG_LV_USE_DEMO_STRESS=y`.
It should be noted that to avoid conflicts, two or more demo scenario macro definition switches must not be enabled simultaneously.

During compilation, please ensure that the appropriate `CONFIG_BUILD_TYPE_XXX` is selected and confirm that the referenced Device Table matches the development board being used for the experiment.
For higher picture display efficiency, users can set the RCC CPU Clock to a relatively high value, such as 240MHz, through the device table.

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

The following logs is displayed after successful operation

```
[I] (7) lvgl_example: init rcc, mcu clock as 240MHz.
[I] (720) lvgl_example: LCD x_resolution = 480
[I] (720) lvgl_example: LCD y_resolution = 272
[I] (720) lvgl_example: LCD rotation = 0

other logs which may difference between each of lvgl demos
```


## Other Optional Demonstration Functions

Optional Demonstration Function 1: 
Customize the Memory malloc/realloc functions to allocate the heap space required by LVGL from PSRAM.

Hardware-wise, it is necessary to ensure that the PSRAM component is correctly connected to the test platform.
Software-wise, the following options in prj.config can be enabled (disabled by default), or they can be selected and enabled in the menuconfig UI interface.

CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED = y
CONFIG_LV_MEM_CUSTOM = y
CONFIG_LV_MEM_CUSTOM_INCLUDE = "wm_lv_port_mem.h"

Note: Considering that the access efficiency of PSRAM is lower than that of SRAM, after enabling this function, the screen display speed of LVGL may decrease.


Optional Demonstration Function 2: 
Enable the CLI module to query the running status of the project through CLI.

Software-wise, the following options in prj.config can be enabled (enabled by default), or they can be selected and enabled in the menuconfig UI interface.

CONFIG_COMPONENT_CLI_ENABLED = y
CONFIG_CLI_COMMANDS_SYSTEM = y

Note: The CLI is for debug purposes and can be considered to be disabled in the mass production version of the user project.
The following are the CLI commands that will be presented after power-on by entering the "help" command through Uart0 (the default CLI port, which can be adjusted through menuconfig).

```
W801S: help
COMMAND LIST:
cls                   -- clear command line
help                  -- show help info
log                   -- log level cmd
heap                  -- show heap tracing
free                  -- show free heap size
task                  -- show task list
reboot                -- reboot system
version               -- show sdk version info
partition             -- partition table cmd
```