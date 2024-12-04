# CRYPTO demo

## 功能概述

启动一个 FreeRTOS 任务，运行一次 crypto demo。

cryp demo 中遍历了 W800 支持的 crypto 类型，以及该 crypto 类型支持的子模式。

## 环境要求

含有 w80x 的板子。

## 编译和烧录

示例位置：`examples/peripheral/crypto`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.6dev build at Nov 26 2024 17:03:57
[I] (5) main: boot reason 0
[I] (5) main: heap size 272688(266.30KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) crypto_example: crypto demo.
[I] (9) crypto_example: rc4 test success
[I] (9) crypto_example: aes ecb test success
[I] (9) crypto_example: aes cbc test success
[I] (10) crypto_example: aes ctr test success
[I] (10) crypto_example: des ecb test success
[I] (11) crypto_example: des cbc test success
[I] (11) crypto_example: 3des ecb test success
[I] (12) crypto_example: 3des cbc test success
[I] (12) crypto_example: crypto demo.
[I] (1012) crypto_example: crypto demo.
[I] (2012) crypto_example: crypto demo.
[I] (3012) crypto_example: crypto demo.
```