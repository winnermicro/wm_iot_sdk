# I2C write,read

## 功能概述

此应用程序启动后执行下面几个操作：

1. 初始化I2C控制器驱动
2. 从EEPROM内部偏移0写入一页16个字节的字符串
3. 从EEPROM内部偏移0读出一页16个字节，打印出来

## 环境要求

1. Demo使用EVB板子，EEPROM型号为NV24C02，在该板子上的I2C地址为0x52；使用其他型号时注意地址等参数要修改匹配。
2. I2C API读写EEPROM,I2C API使用方法，实际读写EEPROM时，使用EEPROM的设备驱动API更方便。

## 编译和烧录

示例位置：`examples/peripheral/i2c`
相关示例：`examples/peripheral/eeprom`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.006] flash size 0x200000(2MB)
I/example_i2c     [0.006] i2c write/read example.

D/dt              [0.006] add device i2c0
I/example_i2c     [0.008] write end,ret=0
I/example_i2c     [0.028] read end,ret=0
I/example_i2c     [0.028] read OK:0123456789abc
```
