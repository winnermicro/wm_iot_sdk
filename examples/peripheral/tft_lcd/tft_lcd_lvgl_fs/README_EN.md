# TFT_LCD_LVGL_FS

## Overview of Functions
LVGL supports storing files (such as pictures, music, etc.) in the file system. It allows for the customization of memory allocation methods and enables software decoders to decode original pictures in some formats (e.g.,.PNG).
This application demonstrates the combined capabilities of `LVGL` + `pSRAM` + `FATFS` + `PNG Decoder` using the `WM IoT SDK`.
Meanwhile, the application also showcases the functions of `customized partition tables` and `custom partitions`.
This application is mainly used for demonstrating combined functions, and some functions may affect the performance of the screen demonstration. Appropriate trade-offs need to be made in actual projects.
Regarding `PNG Decoder` + `FATFS`, they are mainly applicable to the display of the LOGO in this example. For other original LVGL Demo scenarios after the LOGO display, these two functions are not enabled for demonstration.

## Environmental Requirements


1. TFT LCD Screen:：

   - Manufacturer: New Vision Microelectronics Inc.
   - Model: NV3041A
   
2. TFT LCD Screen Pin Connections:

   The hardware configuration is stored in the device table, including the pin definitions of the SDIO device, the clock settings of SDIO (SPI Mode), and other pin definitions of the TFT LCD screen device.
   The SDIO device and the TFT LCD screen device can be defined and queried in the device table by their device names.
   The definition file of the device table is `wm_dt_hw.c`, but there may be differences due to the different device table files referenced by this project.
   The following figure shows an example of the correspondence between the GPIO numbers of W801S and the pins of NV3041A. In actual situations, reference should be made to the circuit diagram and the corresponding device table configuration.


   |    GPIO序号    | Pin Number	 | Connection Description	| NV3041A PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_22 |   PB6   |   CLK    | 11          |
   | WM_GPIO_NUM_23 |   PB7   |   MOSI   | 13          |
   | WM_GPIO_NUM_27 |   PB11  |   CS     | 9           |
   | WM_GPIO_NUM_26 |   PB10  |   RESET  | 15          |
   | WM_GPIO_NUM_32 |   PB16  |   LED    | 36          |
   | WM_GPIO_NUM_25 |   PB9   |   DCX    | 10          |
   | WM_GPIO_NUM_24 |   PB8   |   TE     | 8(optional) |

3. pSRAM Requirements

   It supports a maximum of 8 MB of external pSRAM.
   The following figure shows an example of the correspondence between the GPIO numbers of W801S and the pins of pSRAM. In actual situations, reference should be made to the circuit diagram and the corresponding device table configuration.

   |    GPIO序号    | Pin Number | Connection Description  | pSRAM PIN |
   | -------------- |-------- | -------- | --------- |
   | WM_GPIO_NUM_15 |   PA15  |   SCLK   | 11        |
   | WM_GPIO_NUM_43 |   PB27  |   CS     | 7         |
   | WM_GPIO_NUM_18 |   PB2   |   SIO0   | 9         |
   | WM_GPIO_NUM_19 |   PB3   |   SIO1   | 3         |
   | WM_GPIO_NUM_20 |   PB4   |   SIO2   | 5         |
   | WM_GPIO_NUM_21 |   PB5   |   SIO3   | 1         |

4. Flash Requirements

   The built-in Flash is used by default, and an independent Flash partition table file `"partition_table_custom.csv"` is adopted.
   This partition table contains a `348KB` `FATFS` partition for storing the FATFS image containing the original pictures.


## Compilation and Burning

Example Location:`examples\tft_lcd\tft_lcd_lvgl_fs`

Compilation options can be configured in `menuconfig` or `prj.config` in the project example directory.

**1. Configuration of SOC and Clock:**
```
CONFIG_BUILD_TYPE_XXX=y
```
Explanation:
XXX represents the chip models supported by WM_IOT_SDK, such as W800, W802, W801S, W803, W805, W806, etc.
The selected `CONFIG_BUILD_TYPE_XXX` and the referenced device table should be consistent with the development board on which the program runs.
In terms of performance, the default RCC CPU Clock is 80MHz. To achieve a higher screen display efficiency, users can set the RCC CPU Clock to a higher value, such as 240MHz, in the device table `wm_dt_hw.c`.

**2. Configuration of TFT-LCD Screen:**
```
CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y
```
Explanation:
The TFT LCD screen used by the application can be replaced, but several conditions need to be met:
The device information of the selected TFT LCD screen should exist in the device table.
The macro definition `WM_LVGL_LCD_MODULE_NAME` in `wm_drv_tft_lcd_cfg.h` should have the same value as the device name of the selected TFT LCD screen in the device table.
In `menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device`, the expected LCD device should be checked, or the compilation switch for the required LCD device can be directly defined in prj.config, such as `CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y`.
The device driver of the selected TFT LCD screen should be fully implemented, mainly the implementation of its ops function list, and the corresponding `ops` should be registered in the `dt_hw_table_entry[]` of the device table.

**3. Demo Scenario:**
```
CONFIG_LV_USE_DEMO_MUSIC=y
```
Explanation:
By default, `CONFIG_LV_USE_DEMO_MUSIC=y` is enabled. It can be modified in `menuconfig -> LVGL -> Demos`, or the compilation item switch for the required selected Demo scenario can be directly defined in `prj.config`, such as opening `CONFIG_LV_USE_DEMO_STRESS=y`.
It should be noted that to avoid running conflicts, the macro definition switches of two or more LVGL original Demo scenarios cannot be enabled simultaneously.

**4. Customized Configuration of LVGL Memory:**
```
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"
```
Explanation:
After the configuration is enabled, the lv_mem_alloc, lv_mem_realloc, and lv_mem_free APIs of LVGL will be implemented by the pSRAM memory management interface defined in `wm_lv_port_mem.h`.
Hardware-wise, it is also necessary to ensure that the pSRAM component exists on the development board where the program runs.
In terms of performance, the access efficiency of pSRAM is lower than that of sRAM. After enabling this function, the screen display speed of LVGL will decrease.
It is essential to ensure that after this definition is enabled, the pSRAM component exists on the development board; otherwise, an `Exception` related to wm heap will occur during runtime.

**5. Configuration of FATFS + PNG Decoder + Customized Partition Table:**
```
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"

CONFIG_LV_USE_PNG=y
CONFIG_LV_USE_FS_FATFS=y
CONFIG_LV_FS_FATFS_LETTER=68
CONFIG_LV_FS_FATFS_CACHE_SIZE=4096

CONFIG_COMPONENT_FATFS_ENABLED=y
CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y
CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE=y
CONFIG_FATFS_LFN_HEAP=y
CONFIG_FATFS_MAX_LFN=255

CONFIG_WM_PARTITION_TABLE_CUSTOMIZATION=y
```

Explanation:
This example contains a fat16 image file, and there is a PNG picture of Winner Mico LOGO in the image file.
The LVGL component loads the picture through lv_img_set_src(img, src). It should be noted that:
- If src is a valid file path, the LVGL component will obtain the file in the file system through interfaces such as f_open and f_read.
   - If the file is a PNG-format picture, the LVGL component will find the corresponding picture decoder to decode it and then display it on the screen.
   - If the file is a binary array in the RGB565 format of the picture, the LVGL component will skip the file system interfaces and PNG Decoder operations and directly perform subsequent processing and display it on the screen.
   - Regarding the valid file path of src, special attention should be paid to the setting of the drive letter. For example, for the path "D:assert/pic.png" pointed to by src, the "D" must be consistent with the following configurations:
     - CONFIG_LV_FS_FATFS_LETTER 68: This is the internal configuration of the LVGL component, where 68 is the ASCII code of the character "D".
     - CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS “D”: This is the configuration of the FATFS component. When the options CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE and CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE are opened, that is, when the disk used by the file system is selected as the built-in Flash, this drive letter "D" will be used by default.
- If src is a binary array in the RGB565 format of the picture, the LVGL component will skip the file system interfaces and PNG Decoder operations and directly perform subsequent processing and display it on the screen.

In terms of performance, the PNG Decoder is calculated by the CPU, resulting in low drawing efficiency and large memory consumption. In actual projects, it is recommended that the file system stores not the original PNG-format pictures but the bin files converted from the pictures to the RGB565 format. If so, the following configurations do not need to be configured:

```
CONFIG_LV_USE_PNG=y
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"
CONFIG_LV_USE_PNG=y
```

For the FAT-format image file containing pictures, it can be generated by some tools, such as winhex, etc.
It should be noted that the size of the generated image file should be less than or equal to the size of the `fatfs` partition (defined in the Flash partition table file).

**6. Configuration of CLI:**
```
CONFIG_COMPONENT_CLI_ENABLED=y
CONFIG_CLI_COMMANDS_SYSTEM=y
```

Explanation:
CLI is used for debugging purposes. The running state of the project can be queried through CLI. In the mass production version of the user project, it can be considered to be closed.

compile, burn, and more, see: [get started](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Results

Upon successful running, the following logs will be output:
```
W801S: [I] (675) lvgl_example: LCD x_resolution = 480
[I] (675) lvgl_example: LCD y_resolution = 272
[I] (675) lvgl_example: LCD rotation = 0

When the corresponding LOGO file is found in the file system, the original picture will be directly retrieved from the Filesystem and decoded in real time by the PNG Decoder for display.
[I] (702) app_fs: found file in the file system, size: 33074 bytes
[I] (703) lvgl_example: show logo from filesystem
```

```
W801S: [I] (675) lvgl_example: LCD x_resolution = 480
[I] (675) lvgl_example: LCD y_resolution = 272
[I] (675) lvgl_example: LCD rotation = 0

When the corresponding LOGO file is not found in the file system, for example, when the drive letter is changed to "E" in this test.
At this time, the array of pictures converted to the RGB format will be directly retrieved from the Text section and displayed on the screen without going through the PNG Decoder.

[E] (701) app_fs: failed(3) to mount fs: E:assets/winner_micro_logo_120x120.png
[I] (701) lvgl_example: show logo from array
```
The actual running effect will be displayed on the TFT-LCD screen.



The following are the CLI commands presented when the `help` command is entered through Uart0 (the default CLI port, which can be adjusted through menuconfig) after power-on:
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