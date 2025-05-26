# TFT_LCD_LVGL_TOUCH

## 功能概述 
此应用程序通过LVGL库与触摸控制器驱动对接，能够实时显示触摸屏上所触摸点的坐标。

## 环境要求

   TFT LCD屏要求：
   - 生产厂家： New Vision Microelectronics Inc。
   - 型号：NV3041A

   应用程序 使用的 TFT LCD 屏可以被替换,但需要满足3个条件：
   - a) Device Table 中 要预先设定 该 TFT LCD 屏的设备信息。  
   - b) 此应用参考程序 引用 wm_drv_tft_lcd_cfg.h 中 的宏定义 'WM_LVGL_LCD_MODULE_NAME'， 其值应配置成 Device Table 中已预设的 TFT LCD 屏设备 的设备名。
      并在 ``menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device`` 中选中 LCD 设备, 或者在 prj.config 中直接定义所需选中 LCD 设备的 Compile 开关，如 "CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y" 。  
   - c) 该 TFT LCD 屏的设备驱动 需要适配好，主要是其驱动实现的 ops 函数需要实现，并关联到 Device Table 中 dt_hw_table_entry[] 中。

1. 触摸驱动芯片：
    - 型号：ft6336 或 xpt2046
    通过 menuconfig -> PERIPHERALS -> Touch driver -> touch Device 中选择触摸驱动控制器。

2. 引脚连接：

   TFT LCD屏的硬件配置 存储在 Device Table 中，包含 SDIO 设备的引脚定义 和 SDIO(SPI Mode) 时钟设定 以及 TFT LCD 屏设备的其他引脚定义。
   SDIO 设备 及 TFT LCD 屏设备 可在 Device Table中 通过设备名 进行定义查询。
   Device Table 的定义文件是 wm_dt_hw.c，但具体可能会因为此工程所引用的设备表文件不同而由差异。

   下图为 W802 的 GPIO 序号  与 NV3041A PIN 的对应关系 示例， 实际应该参考Device Table中的配置。

   |    GPIO序号    | 引脚编号 | 连接描述  | NV3041A PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_9  |   PA9   |   CLK    | 11          |
   | WM_GPIO_NUM_10 |   PA10  |   MOSI   | 13          |
   | WM_GPIO_NUM_14 |   PA14  |   CS     | 9           |
   | WM_GPIO_NUM_7  |   PA7   |   RESET  | 15          |
   | WM_GPIO_NUM_2  |   PA2   |   LED    | 36          |
   | WM_GPIO_NUM_12 |   PA12  |   DCX    | 10          |
   | WM_GPIO_NUM_24 |   PB29  |   TE     | 8           |

   ft6336 和 xpt2046 触摸驱动控制器的硬件配置在wm_dt_hw.c文件中进行（默认使用W802芯片）

   |    GPIO序号    | 引脚编号 | 连接描述  | FT6336  PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_1  |   PA1   | I2C_SCL  | 44          |
   | WM_GPIO_NUM_4  |   PA4   | I2C_SDA  | 45          |
   | WM_GPIO_NUM_24 |   PB8   | RESET    | 42          |

   |    GPIO序号    | 引脚编号 | 连接描述  | xpt2046 PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_24 |   PB8   | CLK      | 16          |
   | WM_GPIO_NUM_25 |   PB9   | CS       | 15          |
   | WM_GPIO_NUM_28 |   PB12  | MOSI     | 14          |
   | WM_GPIO_NUM_29 |   PB13  | MISO     | 12          |
   | WM_GPIO_NUM_30 |   PB14  | IRQ      | 11          |

## 编译和烧录

示例位置：`examples/peripheral/touch_panel/touch_panel_lvgl`

编译时，请注意选择 CONFIG_BUILD_TYPE_XXX 以及 确认所引用的 Device Table 需要与所实验的开发板相匹配。
为了更高的画面显示效率，用户可通过 device table 将 RCC CPU Clock 设定成较高的值，比如：240MHz。

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

```
[I] (207624) main: Last touch: x=136, y=435
[I] (207707) main: Last touch: x=250, y=209
[I] (207740) main: Last touch: x=136, y=435
[I] (207819) main: Last touch: x=137, y=431
[I] (207871) main: Last touch: x=159, y=384
[I] (207904) main: Last touch: x=170, y=363
[I] (207937) main: Last touch: x=176, y=353
[I] (207970) main: Last touch: x=249, y=214
[I] (208002) main: Last touch: x=186, y=339
[I] (208048) main: Last touch: x=191, y=334
[I] (208684) main: Last touch: x=136, y=350
[I] (208814) main: Last touch: x=152, y=324
[I] (208866) main: Last touch: x=264, y=180
[I] (208899) main: Last touch: x=176, y=294
[I] (208932) main: Last touch: x=179, y=289
[I] (210021) main: Last touch: x=295, y=158
[I] (210117) main: Last touch: x=273, y=333
[I] (210181) main: Last touch: x=281, y=333
[I] (210233) main: Last touch: x=287, y=333
[I] (210284) main: Last touch: x=295, y=158
[I] (210338) main: Last touch: x=294, y=150
[I] (210371) main: Last touch: x=282, y=140
[I] (210404) main: Last touch: x=276, y=134
[I] (210437) main: Last touch: x=304, y=265
```