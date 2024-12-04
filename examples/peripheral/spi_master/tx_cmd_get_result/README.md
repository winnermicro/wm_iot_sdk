# spi master 发送命令获取结果示例

## 功能概述

此应用程序通过SPI操控flash

1. 初始化SPI master
2. 通过SPI发送快速读取命令
3. 读取到flash的数据并dump出来


## 环境要求

1. SPI-flash 提前接入到板子上的GPIO
2. 默认的GPIO pin如下所示
    PIN_CS: GPIO0, PIN_CLK: GPIO17, PIN_MISO: GPIO16, PIN_MOSI:GPIO7


## 编译和烧录

示例位置：`examples/peripheral/spi_master/tx_cmd_get_result`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

[I] (2) exam_spim: spi master get flash data examle.

[I] (4) exam_spim: read flash data succ.

0x0000   01 02 03 04 05 06 07 08  09 0a 0b 0c 0d 0e 0f 10  11 12 13 14  ....................
0x0014   15 16 17 18 19 1a 1b 1c  1d 1e 1f 20 21 22 23 24  25 26 27 28  ........... !"#$%&'(
0x0028   29 2a 2b 2c 2d 2e 2f 30  31 32 33 34 35 36 37 38  39 3a 3b 3c  )*+,-./0123456789:;<
0x003c   3d 3e 3f 40 41 42 43 44  45 46 47 48 49 4a 4b 4c  4d 4e 4f 50  =>?@ABCDEFGHIJKLMNOP
0x0050   51 52 53 54 55 56 57 58  59 5a 5b 5c 5d 5e 5f 60  61 62 63 64  QRSTUVWXYZ[\]^_`abcd
0x0064   65 66 67 68 69 6a 6b 6c  6d 6e 6f 70 71 72 73 74  75 76 77 78  efghijklmnopqrstuvwx
0x0078   79 7a 7b 7c 7d 7e 7f 80  81 82 83 84 85 86 87 88  89 8a 8b 8c  yz{|}~..............
0x008c   8d 8e 8f 90 91 92 93 94  95 96 97 98 99 9a 9b 9c  9d 9e 9f a0  ....................
0x00a0   a1 a2 a3 a4 a5 a6 a7 a8  a9 aa ab ac ad ae af b0  b1 b2 b3 b4  ....................
0x00b4   b5 b6 b7 b8 b9 ba bb bc  bd be bf c0 c1 c2 c3 c4  c5 c6 c7 c8  ....................
0x00c8   c9 ca cb cc cd ce cf d0  d1 d2 d3 d4 d5 d6 d7 d8  d9 da db dc  ....................
0x00dc   dd de df e0 e1 e2 e3 e4  e5 e6 e7 e8 e9 ea eb ec  ed ee ef f0  ....................
0x00f0   f1 f2 f3 f4 f5 f6 f7 f8  f9 fa fb fc fd fe ff 01               ................

