# TFT_LCD_LVGL

## 功能概述
此应用程序是使用 `WM IoT SDK` 进行 `LVGL` 功能的示例。它演示了如何初始化 TFT LCD 设备，并创建 LVGL DEMO Task 进行 LVGL 模块的初始化，并展示 LVGL 原生的不同 Demo 场景, 例如：
Widgets,  Music Player,  Benchmark,  Stress 等。 
此程序采用 SDIO(SPI 模式) 与连接屏通信， 通信过程采用 DMA 或 CPU Polling 方式进行。

## 环境要求

1. TFT LCD屏要求：

   - 生产厂家： New Vision Microelectronics Inc。
   - 型号：NV3041A
   
   应用程序 使用的 TFT LCD 屏可以被替换,但需要满足3个条件：

   a) Device Table 中 要预先设定 该 TFT LCD 屏的设备信息。
   b) 此应用参考程序 引用 `wm_drv_tft_lcd_cfg.h` 中 的宏定义 `WM_LVGL_LCD_MODULE_NAME`， 其值应配置成 Device Table 中已预设的 TFT LCD 屏设备 的设备名。
      并在 `menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device` 中选中 LCD 设备, 或者在 `prj.config` 中直接定义所需选中 LCD 设备的 Compile 开关，如 `CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y` 。
   c) 该 TFT LCD 屏的设备驱动 需要适配好，主要是其驱动实现的 `ops` 函数需要实现，并关联到 Device Table 中 `dt_hw_table_entry[]` 中。

2. 引脚连接：

   硬件配置 存储在 Device Table 中，包含 SDIO 设备的引脚定义 和 SDIO(SPI Mode) 时钟设定 以及 TFT LCD 屏设备的其他引脚定义。
   SDIO 设备 及 TFT LCD 屏设备 可在 Device Table中 通过设备名 进行定义查询。
   Device Table 的定义文件是 `wm_dt_hw.c`，但具体可能会因为此工程所引用的设备表文件不同而由差异。

   下图为 W801S 的 GPIO 序号  与 NV3041A PIN 的对应关系 示例， 实际应该参考Device Table中的配置。

   |    GPIO序号    | 引脚编号 | 连接描述  | NV3041A PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_22 |   PB6   |   CLK    | 11          |
   | WM_GPIO_NUM_23 |   PB7   |   MOSI   | 13          |
   | WM_GPIO_NUM_27 |   PB11  |   CS     | 9           |
   | WM_GPIO_NUM_26 |   PB10  |   RESET  | 15          |
   | WM_GPIO_NUM_32 |   PB16  |   LED    | 36          |
   | WM_GPIO_NUM_25 |   PB9   |   DCX    | 10          |
   | WM_GPIO_NUM_24 |   PB8   |   TE     | 8           |


## 编译和烧录

示例位置：`examples\tft_lcd\tft_lcd_lvgl`

Demo 场景的选择 是在 `menuconfig -> LVGL -> Demos` 中选中所需的 Demos选项，或者在 `prj.config` 中直接定义所需选中 Demo 场景的 Compile 开关，比如：打开 `CONFIG_LV_USE_DEMO_STRESS=y`。
需要注意的是，为了避免运行冲突, 不能同时打开两个及以上的 Demo 场景的宏定义开关。

编译时，请注意选择 `CONFIG_BUILD_TYPE_XXX` 以及 确认所引用的 Device Table 需要与所实验的开发板相匹配。
为了更高的画面显示效率，用户可通过 device table 将 RCC CPU Clock 设定成较高的值，比如：240MHz。

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)


## 运行结果

成功运行将输出如下日志

```
[I] (7) lvgl_example: init rcc, mcu clock as 240MHz.
[I] (720) lvgl_example: LCD x_resolution = 480
[I] (720) lvgl_example: LCD y_resolution = 272
[I] (720) lvgl_example: LCD rotation = 0

other logs which may difference between each of lvgl demos
```




## 其他可选演示功能

可选演示功能1： 客制化 Memeoy malloc/realloc 函数，从 PSRAM 中分配 LVGL 所需的 heap 空间。

硬件上，须确保 pSRAM 元件有正确接到 测试平台上。
软件上，可启用 prj.config 中的如下选项(默认关闭)， 或者在 menuconfig UI界面中 选择启用。

CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"

说明：考虑到 pSRAM 访问效率弱于 sRAM， 打开此功能后，可能 LVGL 的画面显示速度会降低。



可选演示功能2： 启用 CLI 模块，通过 CLI 查询工程运行时状态

软件上，可启用 prj.config 中的如下选项(默认打开)， 或者在 menuconfig UI界面中 选择启用。

CONFIG_COMPONENT_CLI_ENABLED=y
CONFIG_CLI_COMMANDS_SYSTEM=y

说明：CLI 为 debug 用途，在用户工程中的量产版本 可以考虑关闭。
      如下是上电后，通过 Uart0（默认 CLI 端口，可通过 menuconfig 调整), 输入 help 命令 呈现出的 CLI 命令.

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