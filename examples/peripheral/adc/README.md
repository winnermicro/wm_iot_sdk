# adc 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 初始化 adc
2. 启动线程，在线程中去读芯片内部温度，芯片电压以及通道 0 的数据

## 环境要求

请务必注意，adc 的通道和 io 有对应关系，配置时候请参考 wm_adc_channel_t 中的定义

## 编译和烧录

示例位置：`examples/peripheral/adc`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (21) adc_example: chip temp 40.205
[I] (46) adc_example: chip voltage 3233mv
[I] (50) adc_example: result oneshot 817mv
[I] (93) adc_example: result 0 356mv
[I] (94) adc_example: result 1 97mv
[I] (94) adc_example: result 2 -17mv
[I] (94) adc_example: result 3 37mv
[I] (94) adc_example: result 4 179mv
[I] (94) adc_example: result 5 339mv
[I] (94) adc_example: result 6 454mv
[I] (94) adc_example: result 7 465mv
[I] (94) adc_example: result 8 306mv
[I] (94) adc_example: result 9 58mv
[I] (94) adc_example: result 10 -17mv
[I] (94) adc_example: result 11 -17mv
[I] (94) adc_example: result 12 -16mv
[I] (94) adc_example: result 13 61mv
[I] (95) adc_example: result 14 208mv
[I] (95) adc_example: result 15 387mv
[I] (95) adc_example: result 16 478mv
[I] (95) adc_example: result 17 437mv
[I] (95) adc_example: result 18 244mv
[I] (95) adc_example: result 19 0mv
```