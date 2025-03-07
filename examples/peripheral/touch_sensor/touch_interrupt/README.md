# 触摸传感器中断

## 功能概述
本示例演示了使用触摸传感器中断函数。

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

示例位置：`examples/peripheral/touch_sensor/touch_interrupt`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.038] add device touch_sensor
D/dt              [0.042] add device gpio
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 5 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 3 activated
touch sensor 4 activated
touch sensor 4 activated
touch sensor 4 activated
touch sensor 4 activated
```