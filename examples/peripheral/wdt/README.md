# wdt 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 初始化 wdt，当前 wdt 的初始化值在 wm_dt_hw.c 中配置为 10s
2. 启动线程，在线程中进行喂狗操作，并读取看门狗的周期值以及剩余值


## 环境要求

看门狗的配置在 device tree 中已经配置完成，如需更改可以修改对应的 devcie tree


## 编译和烧录

示例位置：`examples/peripheral/wdt`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.038] add device wdt
I/wdt_test        [0.042] wdt feed
I/wdt_test        [0.046] wdt counter value 400000000 reamining time 399998672
I/wdt_test        [0.550] wdt feed
I/wdt_test        [0.554] wdt counter value 400000000 reamining time 399999252
```