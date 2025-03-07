# SEG_LCD-Sample

## 功能概述
此应用程序是使用 `WM IoT SDK` 进行 `SEG_LCD` 断码屏功能的全面示例。它演示了如何初始化 SEG_LCD 设备，执行一系列显示操作，包括反复显示所有断码、星期等，并在最后清除显示并反初始化设备。

## 环境要求

1. 断码屏要求：

   - 生产厂家：深圳市亘宇科技有限公司
   - 型号：GY-H70248 (举例)
   - 可以替换成其他任意断码屏

2. 引脚连接：

   | 引脚编号 | 连接描述  | GY-H70248 PIN（举例） |
   | ---- | ----- | ----------- |
   | PB25 | COM0  | 1 和 37     |
   | PB21 | COM1  | 2 和 38     |
   | PB22 | COM2  | 3 和 39     |
   | PB27 | COM3  | 4           |
   | PB26 | SEG01 | 5           |
   | PB24 | SEG02 | 6           |
   | PA7  | SEG03 | 7           |
   | PA8  | SEG04 | 8           |
   | PA9  | SEG05 | 9           |
   | PA10 | SEG06 | 10          |
   | PA11 | SEG07 | 11          |
   | PA12 | SEG08 | 12          |
   | PA13 | SEG09 | 13          |
   | PA14 | SEG10 | 14          |
   | PA15 | SEG11 | 15          |
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
   | PB14 | SEG26 | 30          |
   | PB15 | SEG27 | 31          |
   | PB16 | SEG28 | 32          |
   | PB17 | SEG29 | 33          |
   | PB18 | SEG30 | 34          |
   | PA6  | SEG31 | 35          |

## 编译和烧录

示例位置：`examples\peripheral\seg_lcd`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志
```
[I] (10) main: ver: 2.2-beta.2 build at Dec 10 2024 09:33:51
[I] (10) main: boot reason 0
[I] (11) main: heap size 272648(266.26KB)
[I] (11) main: flash size 0x200000(2MB)
[I] (13) example: seg_lcd display demo start.
[I] (13) example: Initializing segment LCD device...
[I] (15) example: Starting segment LCD display/clear cycle...
[I] (10131) example: Deinitializing segment LCD device.
[I] (10132) example: This example succeeds in running.
```
