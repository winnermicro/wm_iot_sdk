# I2S write

## 功能概述

此应用程序启动后执行下面几个操作：

1. 初始化 I2S 控制器驱动

2. 启动接收数据 Task

    （1） 注册接收数据回调函数,回调函数中，把数发送到 RX task 中进行处理；

    （2） 准备接收 buffer,添加接收数据 buffer；

    （3） 循环接收处理数据；

    （4） 结束处理，停止接收，注销回调，释放接收buffer；

3. 启动发送数据 Task

    （1） 注册发送数据回调函数,回调函数中；

    （2） 准备发送数据 buffer,初始化为递增的 uint8_t 整数数据；

    （3） 循环发送数据；

    （4） 结束处理，停止发送，注销回调，释放数据buffer。


## 环境要求

1. Demo pin脚使用要求：

```
┌─────────────────┐           ┌──────────────────────────┐
│        W80X     │           │           W80X           │
│                 │           │                          │
│                 │           │                          │
│     I2S_BCLK    ├──────────>│I2S_BCLK                  |
│                 │           │                          │
│    I2S_RLCLK    ├──────────>│I2S_RLCLK                 |
│                 │           │                          │
│        I2S_DI   |<──────────│I2S_DO                    │
│                 │           │                          │
│        I2S_DO   │──────────>|I2S_DI                    │
│                 │           │                          │
│                 │           │                          │
│              GND├───────────┤GND                       │
└─────────────────┘           └──────────────────────────┘
```

2. Demo pin 脚配置，不同开发板，芯片型号可能不一样，需要在设备表中配置 pin 脚一致。

3. 代码中的 I2S 配置

代码中默认配置是 Mstar 模式，如果 2 台设备通信，其中一台需要设置为 slave 模式

## 编译和烧录

示例位置：`examples/peripheral/i2s`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志 （ Log 输出级别调整为 Debug 才能输出 [D] 的日志，这些日志方便调试，但会影响数据传输。）

```
[D] (0) dt: add device rcc
[D] (0) dt: add device pmu
[D] (0) dt: add device iflash
[D] (1) ptb: partition table base 0xe000
[D] (1) dt: add device hash
[D] (1) ft: ft_0 base 0, ft_1 base 1000
[D] (1) ft: ft init too
[D] (2) dt: add device crc
[D] (2) ft: ft init 1


[I] (7) main: ver: 2.1.4dev build at Nov 25 2024 14:48:30
[I] (7) main: boot reason 0
[I] (7) main: heap size 272504(266.12KB)
[I] (7) main: flash size 0x200000(2MB)
[I] (8) example_i2s: I2S example start.
[D] (8) dt: add device i2s
[D] (8) DRV_I2S: clock:160000000, bitrate:1536000, mclk:80000000, extal:0, mdiv:2, bdiv:104
[D] (9) DRV_I2S: cfg use tx
[D] (9) DRV_I2S: cfg use rx
[I] (9) example_i2s: start OK, mode=0,dir=2,std=0,fmt=1,ctype=2,rx_pkt_size=1024,rx_pkt_num=6,tx_pkt_num=6
[I] (9) example_i2s: start read data
[D] (9) HAL_I2S: rx dma start
[I] (10) example_i2s: start write data loop
[D] (10) HAL_I2S: tx dma start
[I] (22) example_i2s: RX done
[D] (84) HAL_I2S: rx dma start
[D] (148) HAL_I2S: rx dma start
[D] (212) HAL_I2S: rx dma start
[D] (275) HAL_I2S: rx dma start
[D] (339) HAL_I2S: rx dma start
[D] (1064) example_i2s: recv:100
[D] (1074) example_i2s: write:100
[D] (2127) example_i2s: recv:200
[D] (2137) example_i2s: write:200
[D] (3189) example_i2s: recv:300
[D] (3200) example_i2s: write:300
[D] (4252) example_i2s: recv:400
[D] (4263) example_i2s: write:400

```