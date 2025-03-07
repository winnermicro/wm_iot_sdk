# RNG demo

## 功能概述

启动后，运行一次 rng demo。

rng demo 中依次产生 5 个 32 位随机数、10 个 32 位随机数。

## 环境要求

含有 w80x 的板子。

## 编译和烧录

示例位置：`examples/peripheral/rng`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.6dev build at Nov 26 2024 16:58:35
[I] (5) main: boot reason 0
[I] (5) main: heap size 272616(266.23KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) rng_example: rng demo.
RNG out:0x5A452A77 0xB48A54EE 0x6914A9DD 0xD22953BB 0xA452A776 
RNG out:0x316914A9 0x62D22953 0xC5A452A7 0x8B48A54E 0x16914A9D 0x2D22953B 0x5A452A77 0xB48A54EE 0x6914A9DD 0xD22953BB 
[I] (11) rng_example: rng demo.
[I] (1011) rng_example: rng demo.
[I] (2011) rng_example: rng demo.
[I] (3011) rng_example: rng demo.
```