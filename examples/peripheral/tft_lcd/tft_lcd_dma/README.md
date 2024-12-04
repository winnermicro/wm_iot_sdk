# TFT_LCD_DMA

## 功能概述
此应用程序是使用 `WM IoT SDK` 进行 `TFT LCD` 屏功能的全面示例。它演示了如何初始化 TFT LCD 设备，执行一系列显示操作，包括蓝色背景刷屏，Flash 内预存图片显示，屏幕 X,Y 轴旋转显示
此程序采用 SDIO ( SPI 模式 ) 与连接屏通信， 通信过程 大数据采用 DMA 方式进行， 命令和小数据采用 CPU Polling 方式进行。 

## 环境要求

1. TFT LCD屏要求：

   - 生产厂家： New Vision Microelectronics Inc。
   - 型号：NV3041A
   
   应用程序 使用的 TFT LCD 屏可以被替换,但需要满足3个条件：
   a) Device Table 中 要预先设定 该 TFT LCD 屏的设备信息。
   b) 此应用参考程序 引用 wm_drv_tft_lcd_cfg.h 中 的宏定义 'WM_LVGL_LCD_MODULE_NAME'， 其值应配置成 Device Table 中已预设的 TFT LCD 屏设备 的设备名。
      并在 menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device 中选中 LCD 设备, 或者在 prj.config 中直接定义所需选中 LCD 设备的 Compile 开关，如 "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y" 。
   c) 该 TFT LCD 屏的设备驱动 需要适配好，主要是其驱动实现的 ops 函数需要实现，并关联到 Device Table 中 dt_hw_table_entry[] 中。

2. 引脚连接：

   硬件配置 存储在 Device Table 中，包含 SDIO 设备的引脚定义 和 SDIO(SPI Mode) 时钟设定 以及 TFT LCD 屏设备的其他引脚定义。
   SDIO 设备 及 TFT LCD 屏设备 可在 Device Table中 通过设备名 进行定义查询。
   Device Table 的定义文件是 wm_dt_hw.c，但具体可能会因为此工程所引用的设备表文件不同而由差异。

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

示例位置：`examples\tft_lcd\tft_lcd_dma`

编译时，请注意选择 CONFIG_BUILD_TYPE_XXX 以及 确认所引用的 Device Table 需要与所实验的开发板相匹配。
为了更高的画面显示效率，用户可通过 device table 将 RCC CPU Clock 设定成较高的值，比如：240MHz。

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

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