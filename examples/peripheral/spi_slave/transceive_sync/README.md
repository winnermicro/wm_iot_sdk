# spi slave 同步收发示例

## 功能概述

此应用程序启动后执行下面2个操作：

1. 初始化 SPI slave
2. 调用 SPI slave 同步收发接口，发送 256 byte，同时接收 256 byte 数据

## 环境要求

1. master 设备和 spi slave 引脚需提前连接好
2. slave 端需提前启动，等待接收且准备好要发送的数据
3. PIN_CS: GPIO20, PIN_CLK: GPIO17, PIN_MISO: GPIO16, PIN_MOSI: GPIO7, mode 0


## 编译和烧录

示例位置：`examples/peripheral/spi_slave/transceive_sync`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (3) main: ver: 2.4-alpha.2 build at Apr 24 2025 13:55:07
[I] (4) main: boot reason 0
[I] (4) main: heap size 272512(266.12KB)
[I] (5) main: flash size 0x200000(2MB)
[I] (5) exam_spis: spi master sync transecie start.
[I] (358) exam_spis: rx data as below
[I] (358) exam_spis: dump [rx data]
0x0000   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0010   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0020   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0030   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0040   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0050   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0060   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0070   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0080   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x0090   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00a0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00b0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00c0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00d0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00e0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
0x00f0   78 56 34 12 78 56 34 12  78 56 34 12 78 56 34 12  xV4.xV4.xV4.xV4.
[I] (384) exam_spis: This example succeeds in running.
```
