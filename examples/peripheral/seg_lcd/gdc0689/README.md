# SEG_LCD-GDC0689

## 功能概述
此应用程序是使用 `WM IoT SDK` 进行 `SEG_LCD` 断码屏功能的全面示例。它演示了如何初始化 SEG_LCD 设备，执行一系列显示操作，包括反复显示所有断码、服务图标、电池电量、信号强度、单位、时间、整数和小数，并在最后清除显示并反初始化设备。

## 环境要求

1. 断码屏要求：

   - 生产厂家：大连佳显电子有限公司
   - 型号：WM_GDC0689

2. 引脚连接：

   | 引脚编号 | 连接描述  | WM_GDC0689 PIN |
   | ---- | ----- | ----------- |
   | PB25 | COM0  | 12          |
   | PB21 | COM1  | 13          |
   | PB22 | COM2  | 14          |
   | PB27 | COM3  | 15          |
   | PB26 | SEG01 | 1           |
   | PB24 | SEG02 | 2           |
   | PA7  | SEG03 | 3           |
   | PA8  | SEG04 | 4           |
   | PA9  | SEG05 | 5           |
   | PA10 | SEG06 | 6           |
   | PA11 | SEG07 | 7           |
   | PA12 | SEG08 | 8           |
   | PA13 | SEG09 | 9           |
   | PA14 | SEG10 | 10          |
   | PA15 | SEG11 | 11          |
   | PBO  | SEG12 | 16          |
   | PB1  | SEG13 | 17          |
   | PB2  | SEG14 | 18          |
   | PB3  | SEG15 | 19          |
   | PB4  | SEG16 | 20          |
   | PB5  | SEG17 | 21          |
   | PB6  | SEG18 | 22          |
   | PB7  | SEG19 | 23          |
   | PB8  | SEG20 | 24          |
   | PB9  | SEG21 | 25          |
   | PB10 | SEG22 | 26          |
   | PB11 | SEG23 | 27          |
   | PB12 | SEG24 | 28          |
   | PB13 | SEG25 | 29          |


## 编译和烧录

示例位置：`examples\peripheral\seg_lcd\gdc0689\`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.002] add device flash
D/ptb             [0.006] partition table base 0xe000
D/ft              [0.012] ft_0 base 0, ft_1 base 1000
D/ft              [0.016] ft init too
D/ft              [0.020] ft init 1
D/main            [0.026] boot reason 0
D/main            [0.030] heap size 276480(270.00KB)
D/main            [0.034] flash size 0x200000(2MB)
I/example         [0.040] Initializing segment LCD device...
D/dt              [0.044] add device seg_lcd
I/example         [0.050] Displaying all segments.
I/example         [0.116] Clearing segment LCD.
I/example         [0.170] Displaying all segments.
I/example         [0.236] Clearing segment LCD.
I/example         [0.290] Displaying all segments.
I/example         [0.356] Clearing segment LCD.
I/example         [0.410] Displaying all segments.
I/example         [0.476] Clearing segment LCD.
I/example         [0.530] Displaying all segments.
I/example         [0.596] Clearing segment LCD.
I/example         [0.650] Displaying time: hour 0, minute 0.
I/example         [0.708] Displaying time: hour 1, minute 0.
I/example         [0.766] Displaying time: hour 2, minute 0.
I/example         [0.824] Displaying time: hour 3, minute 0.
I/example         [0.882] Displaying time: hour 4, minute 0.
I/example         [0.940] Displaying time: hour 5, minute 0.
I/example         [0.998] Displaying time: hour 6, minute 0.
I/example         [1.056] Displaying time: hour 7, minute 0.
I/example         [1.114] Displaying time: hour 8, minute 0.
I/example         [1.172] Displaying time: hour 9, minute 0.
I/example         [1.230] Displaying time: hour 10, minute 0.
I/example         [1.288] Displaying time: hour 11, minute 0.
I/example         [1.346] Displaying time: hour 12, minute 0.
I/example         [1.404] Displaying time: hour 13, minute 0.
I/example         [1.462] Displaying time: hour 14, minute 0.
I/example         [1.520] Displaying time: hour 15, minute 0.
I/example         [1.578] Displaying time: hour 16, minute 0.
I/example         [1.636] Displaying time: hour 17, minute 0.
I/example         [1.694] Displaying time: hour 18, minute 0.
I/example         [1.752] Displaying time: hour 19, minute 0.
I/example         [1.810] Displaying time: hour 20, minute 0.
I/example         [1.868] Displaying time: hour 21, minute 0.
I/example         [1.926] Displaying time: hour 22, minute 0.
I/example         [1.984] Displaying time: hour 23, minute 0.
I/example         [2.042] Displaying time: hour 0, minute 0.
I/example         [2.100] Displaying time: hour 0, minute 1.
I/example         [2.158] Displaying time: hour 0, minute 2.
I/example         [2.216] Displaying time: hour 0, minute 3.
I/example         [2.274] Displaying time: hour 0, minute 4.
I/example         [2.332] Displaying time: hour 0, minute 5.
I/example         [2.390] Displaying time: hour 0, minute 6.
I/example         [2.448] Displaying time: hour 0, minute 7.
I/example         [2.506] Displaying time: hour 0, minute 8.
I/example         [2.564] Displaying time: hour 0, minute 9.
I/example         [2.622] Displaying time: hour 0, minute 10.
I/example         [2.680] Displaying time: hour 0, minute 11.
I/example         [2.738] Displaying time: hour 0, minute 12.
I/example         [2.796] Displaying time: hour 0, minute 13.
I/example         [2.854] Displaying time: hour 0, minute 14.
I/example         [2.912] Displaying time: hour 0, minute 15.
I/example         [2.970] Displaying time: hour 0, minute 16.
I/example         [3.028] Displaying time: hour 0, minute 17.
I/example         [3.086] Displaying time: hour 0, minute 18.
I/example         [3.144] Displaying time: hour 0, minute 19.
I/example         [3.202] Displaying time: hour 0, minute 20.
I/example         [3.260] Displaying time: hour 0, minute 21.
I/example         [3.318] Displaying time: hour 0, minute 22.
I/example         [3.376] Displaying time: hour 0, minute 23.
I/example         [3.434] Displaying time: hour 0, minute 24.
I/example         [3.492] Displaying time: hour 0, minute 25.
I/example         [3.550] Displaying time: hour 0, minute 26.
I/example         [3.608] Displaying time: hour 0, minute 27.
I/example         [3.666] Displaying time: hour 0, minute 28.
I/example         [3.724] Displaying time: hour 0, minute 29.
I/example         [3.782] Displaying time: hour 0, minute 30.
I/example         [3.840] Displaying time: hour 0, minute 31.
I/example         [3.898] Displaying time: hour 0, minute 32.
I/example         [3.956] Displaying time: hour 0, minute 33.
I/example         [4.014] Displaying time: hour 0, minute 34.
I/example         [4.072] Displaying time: hour 0, minute 35.
I/example         [4.130] Displaying time: hour 0, minute 36.
I/example         [4.188] Displaying time: hour 0, minute 37.
I/example         [4.246] Displaying time: hour 0, minute 38.
I/example         [4.304] Displaying time: hour 0, minute 39.
I/example         [4.362] Displaying time: hour 0, minute 40.
I/example         [4.420] Displaying time: hour 0, minute 41.
I/example         [4.478] Displaying time: hour 0, minute 42.
I/example         [4.536] Displaying time: hour 0, minute 43.
I/example         [4.594] Displaying time: hour 0, minute 44.
I/example         [4.652] Displaying time: hour 0, minute 45.
I/example         [4.710] Displaying time: hour 0, minute 46.
I/example         [4.768] Displaying time: hour 0, minute 47.
I/example         [4.826] Displaying time: hour 0, minute 48.
I/example         [4.884] Displaying time: hour 0, minute 49.
I/example         [4.942] Displaying time: hour 0, minute 50.
I/example         [5.000] Displaying time: hour 0, minute 51.
I/example         [5.058] Displaying time: hour 0, minute 52.
I/example         [5.116] Displaying time: hour 0, minute 53.
I/example         [5.174] Displaying time: hour 0, minute 54.
I/example         [5.232] Displaying time: hour 0, minute 55.
I/example         [5.290] Displaying time: hour 0, minute 56.
I/example         [5.348] Displaying time: hour 0, minute 57.
I/example         [5.406] Displaying time: hour 0, minute 58.
I/example         [5.464] Displaying time: hour 0, minute 59.
I/example         [5.522] Displaying service icon 0.
I/example         [5.576] Displaying service icon 1.
I/example         [5.630] Displaying battery level 0.
I/example         [5.684] Displaying battery level 1.
I/example         [5.738] Displaying battery level 2.
I/example         [5.792] Displaying battery level 3.
I/example         [5.846] Displaying battery level 4.
I/example         [5.900] Displaying battery level 5.
I/example         [5.954] Displaying signal level 0.
I/example         [6.008] Displaying signal level 1.
I/example         [6.062] Displaying signal level 2.
I/example         [6.116] Displaying signal level 3.
I/example         [6.170] Displaying signal level 4.
I/example         [6.224] Displaying integer value 0.
I/example         [6.284] Displaying integer value 111111.
I/example         [6.344] Displaying integer value 222222.
I/example         [6.404] Displaying integer value 333333.
I/example         [6.464] Displaying integer value 444444.
I/example         [6.524] Displaying integer value 555555.
I/example         [6.584] Displaying integer value 666666.
I/example         [6.644] Displaying integer value 777777.
I/example         [6.704] Displaying integer value 888888.
I/example         [6.764] Displaying integer value 999999.
I/example         [6.824] Displaying decimal value 0000.00.
I/example         [6.884] Displaying decimal value 1111.11.
I/example         [6.944] Displaying decimal value 2222.22.
I/example         [7.004] Displaying decimal value 3333.33.
I/example         [7.064] Displaying decimal value 4444.44.
I/example         [7.124] Displaying decimal value 5555.55.
I/example         [7.184] Displaying decimal value 6666.66.
I/example         [7.244] Displaying decimal value 7777.77.
I/example         [7.304] Displaying decimal value 8888.88.
I/example         [7.364] Displaying decimal value 9999.99.
I/example         [7.424] Displaying unit 0.
I/example         [7.478] Displaying unit 1.
I/example         [7.532] Displaying unit 2.
I/example         [7.586] Displaying unit 3.
I/example         [7.640] Displaying unit 4.
I/example         [7.694] Displaying unit 5.
I/example         [7.748] Displaying unit 6.
I/example         [7.802] Displaying unit 7.
I/example         [7.856] Displaying unit 8.
I/example         [7.910] Displaying unit 9.
I/example         [7.964] Clearing segment LCD again.
I/example         [8.018] Deinitializing segment LCD device.
I/example         [8.024] This example succeeds in running.
```