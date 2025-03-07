# 设置CPU频率

## 功能概述

此应用程序启动后执行下面2个操作：

1. 初始化 RCC 模块
2. 将 CPU clock设置为120MHz


## 环境要求

1. 无


## 编译和烧录

示例位置：`examples/peripheral/rcc`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)


## 运行结果

成功运行将输出如下日志

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.002] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.004] flash size 0x200000(2MB)
I/exam_rcc        [0.006] rcc set cpu clock start.

set cpu clock as 120Mhz pass
```
