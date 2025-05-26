# touch_panel

## 功能概述

启动一个 FreeRTOS 任务，初始化触摸驱动芯片，配置触摸芯片基本参数，
在循环中获取触摸点，通过触摸点获取实际所触摸的坐标，并在串口中显示出来。

如何添加新的 touch panel :  
   - a) Device Table 中 要预先设定 该 touch panel 屏的设备信息.  
   - b) 此应用参考程序 引用 wm_drv_touch_panel.h 中 的宏定义 'WM_CFG_TOUCH_PANEL_NAME'， 其值应配置成 Device Table 中已预设的 touch panel 屏设备 的设备名。  
      并在 menuconfig -> PERIPHERALS -> Touch Panel -> touch panel 中选中 touch panel 设备, 或者在 prj.config 中直接定义所需选中 touch panel 设备的 Compile 开关，如 "CONFIG_COMPONENT_XPT2046_DRIVER=y" 。  
   - c) 该 TFT touch panel 屏的设备驱动 需要适配好，主要是其驱动实现的 ops 函数需要实现，并关联到 Device Table 中 dt_hw_table_entry[] 中。

## 环境要求

1. 触摸驱动芯片：
    - 型号：ft6336 或 xpt2046
    通过 menuconfig -> PERIPHERALS -> Touch driver -> touch Device 中选择触摸驱动控制器。

2. 引脚连接：
   ft6336 和 xpt2046 触摸驱动控制器的硬件配置在wm_dt_hw.c文件中进行（默认使用W802芯片）
   这仅是一个范例，用户需要依据自己的开发板及应用需求，选择自己适合的引脚定义，修改device table，然后进行重新编译。

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

示例位置：`examples/peripheral/touch_panel/touch_panel_basic`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

当触摸ft6336触摸屏时，如果只有一个触摸点时，只会打印触摸点1（Touch coordinate point 1）
如果有两个触摸点，则会打印触摸点1（Touch coordinate point 1），触摸点2（Touch coordinate point 2）。
对于电阻触摸屏(如:xpt2046) 执行时也会打印如下log, 只是电阻屏需要做校准后才准确，校准可以参考另外一个例程`examples/peripheral/touch_panel/touch_panel_calibration`

```
[I] (352622) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352623) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352724) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352725) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352826) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352827) test: Touch coordinate point 2, x-coordinate is 251, y-coordinate is 341.
[I] (352928) test: Touch coordinate point 1, x-coordinate is 302, y-coordinate is 240.
[I] (352929) test: Touch coordinate point 2, x-coordinate is 243, y-coordinate is 342.
[I] (353030) test: Touch coordinate point 1, x-coordinate is 227, y-coordinate is 355.
[I] (353031) test: Touch coordinate point 2, x-coordinate is 300, y-coordinate is 209.
[I] (353132) test: Touch coordinate point 1, x-coordinate is 285, y-coordinate is 131.
[I] (353133) test: Touch coordinate point 2, x-coordinate is 227, y-coordinate is 355.
[I] (353234) test: Touch coordinate point 1, x-coordinate is 300, y-coordinate is 130.
[I] (353336) test: Touch coordinate point 1, x-coordinate is 205, y-coordinate is 313.
[I] (353438) test: Touch coordinate point 1, x-coordinate is 284, y-coordinate is 156.
[I] (366476) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (366578) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (366680) test: Touch coordinate point 1, x-coordinate is 127, y-coordinate is 208.
[I] (367085) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367086) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367187) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367188) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367289) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367290) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367391) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367392) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367493) test: Touch coordinate point 1, x-coordinate is 219, y-coordinate is 132.
[I] (367494) test: Touch coordinate point 2, x-coordinate is 144, y-coordinate is 256.
[I] (367999) test: Touch coordinate point 1, x-coordinate is 176, y-coordinate is 246.
[I] (368000) test: Touch coordinate point 2, x-coordinate is 218, y-coordinate is 134.
[I] (368101) test: Touch coordinate point 1, x-coordinate is 218, y-coordinate is 134.
[I] (368102) test: Touch coordinate point 2, x-coordinate is 176, y-coordinate is 246.
[I] (368203) test: Touch coordinate point 1, x-coordinate is 218, y-coordinate is 134.
[I] (368305) test: Touch coordinate point 1, x-coordinate is 148, y-coordinate is 217.
```