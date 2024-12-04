# EEPROM write,read

## 功能概述

此应用程序启动后执行下面几个操作：

1. 初始化I2C控制器驱动和EEPROM设备驱动
2. 从EEPROM内部偏移为5的地方写入一个字符串
3. 从EEPROM内部偏移为5的地方读出一个字符串

## 环境要求

1. Demo使用EVB Board，EEPROM型号为NV24C02，在该Board上的I2C地址为0x52；使用其他board和EEPROM型号时注意修改设备表中配置。

## 编译和烧录

示例位置：`examples/peripheral/eeprom`
相关示例：`examples/peripheral/i2c`

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
I/example_eeprom  [0.006] eeprom write/read example.

D/dt              [0.006] add device i2c0
D/dt              [0.008] add device eeprom0
I/example_eeprom  [0.016] write end,len=24,ret=0
I/example_eeprom  [0.016] read end,ret=0
I/example_eeprom  [0.018] read OK:0123456789abcdefghijklmn
```
