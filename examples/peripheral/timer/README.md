# timer 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 初始化 timer0, timer1, timer2
2. 启动 timer，并观察 timer 执行

## 环境要求

请注意，w800 支持6路 timer，在 device tree 中的设备名称为 timer0, timer1, timer2, timer3, timer4, timer5，请按需注册

## 编译和烧录

示例位置：`examples/peripheral/timer`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.038] add device timer0
D/dt              [0.042] add device timer1
D/dt              [0.046] add device timer2
I/timer_test      [2.052] timer1 TRIGGERED
I/timer_test      [3.052] timer2 TRIGGERED
I/timer_test      [4.052] timer1 TRIGGERED
I/timer_test      [6.052] timer1 TRIGGERED
I/timer_test      [6.052] timer2 TRIGGERED
I/timer_test      [8.046] timer1 TRIGGERED
I/timer_test      [9.046] timer2 TRIGGERED
I/timer_test      [10.046] timer0 TRIGGERED
I/timer_test      [10.046] timer1 TRIGGERED
I/timer_test      [10.050] all timer stop
```