# CRC demo

## 功能概述

启动一个 FreeRTOS 任务，运行一次 crc demo。

crc demo 中遍历了 W800 支持的 crc 类型，以及是否对输入输出数据进行反向处理。

## 环境要求

含有 w80x 的板子。

## 编译和烧录

示例位置：`examples/peripheral/crc`

编译、烧录等操作请参考：[get started](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.6dev build at Nov 26 2024 17:05:47
[I] (5) main: boot reason 0
[I] (5) main: heap size 272616(266.23KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) crc_example: crc demo.
[I] (8) crc_example: WM_GPSEC_CRC8 normal value:0x00000096
[I] (8) crc_example: WM_GPSEC_CRC8 INPUT_REFLECT value:0x00000003
[I] (9) crc_example: WM_GPSEC_CRC8 OUTPUT_REFLECT value:0x00000069
[I] (9) crc_example: WM_GPSEC_CRC8 OUT_IN_REVERSE value:0x000000C0
[I] (9) crc_example: WM_GPSEC_CRC16_MODBUS normal value:0x00002B74
[I] (9) crc_example: WM_GPSEC_CRC16_MODBUS INPUT_REFLECT value:0x0000F384
[I] (10) crc_example: WM_GPSEC_CRC16_MODBUS OUTPUT_REFLECT value:0x00002ED4
[I] (10) crc_example: WM_GPSEC_CRC16_MODBUS OUT_IN_REVERSE value:0x000021CF
[I] (10) crc_example: WM_GPSEC_CRC16_CCITT normal value:0x0000B685
[I] (10) crc_example: WM_GPSEC_CRC16_CCITT INPUT_REFLECT value:0x0000964E
[I] (11) crc_example: WM_GPSEC_CRC16_CCITT OUTPUT_REFLECT value:0x0000A16D
[I] (18) crc_example: WM_GPSEC_CRC16_CCITT OUT_IN_REVERSE value:0x00007269
[I] (18) crc_example: WM_GPSEC_CRC32 normal value:0x109EAC83
[I] (18) crc_example: WM_GPSEC_CRC32 INPUT_REFLECT value:0x4B1CAC0D
[I] (18) crc_example: WM_GPSEC_CRC32 OUTPUT_REFLECT value:0xC1357908
[I] (42) crc_example: WM_GPSEC_CRC32 OUT_IN_REVERSE value:0xB03538D2
[I] (42) crc_example: crc demo.
[I] (1042) crc_example: crc demo.
[I] (2042) crc_example: crc demo.
[I] (3042) crc_example: crc demo.
[I] (4042) crc_example: crc demo.
```