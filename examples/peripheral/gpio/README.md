# gpio 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 初始化 gpio
2. 配置 gpio，包括GPIO的输入输出以及配置中断，对于未在设备表中定义的IO配置也进行了描述
3. 使用中断触发，观察设备log输出

## 环境要求

请注意，gpio 应该作为设备使用 devcie tree 配置，但是由于 iomux 的特性，也提供了针对 pin 的 io 配置

## 编译和烧录

示例位置：`examples/peripheral/gpio`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.090] add device gpio
I/gpio            [0.094] WM_GPIO_DEFINE3 data value high
I/gpio            [1.098] WM_GPIO_DEFINE3 data value low
I/gpio            [2.102] gpio init ok
I/gpio            [19.178] GPIO[4] intr, val: 1
I/gpio            [21.214] GPIO[4] intr, val: 0
```