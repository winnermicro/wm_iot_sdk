# 触摸传感器按键

## 功能概述
本示例演示了使用触摸传感器进行按键短按和长按。

持续按住时间少于 500 毫秒的一次触摸并松开为短按，持续按住达到 3 秒以上时为长按。

## 环境要求

触摸传感器按键如下表连接：

| IO 编号 | IO 名称 | touch 编号   |
| :------ | :-----: | :--------- |
| 16      | PB0     | 3          |
| 17      | PB0     | 4          |
| 18      | PB0     | 5          |
| 12      | PA12    | CMOD       |
| 14      | PA14    | CMC        |

## 编译和烧录

示例位置：`examples/peripheral/touch_sensor/touch_button`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.038] add device touch_sensor
D/dt              [0.042] add device gpio
I/touch           [0.046] touch button init ok
I/touch           [7.318] key5 pressed
I/touch           [11.326] key5 pressed
I/touch           [14.768] key5 long pressed
I/touch           [99.176] key4 pressed
I/touch           [99.754] key3 pressed
```