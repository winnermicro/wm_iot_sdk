# HASH demo

## 功能概述

启动一个 FreeRTOS 任务，运行一次 hash demo。

hash demo 演示了使用 MD5 和 SHA1 模式计算数据的 hash 值，并与提前计算好的期望 hash 值比较。

## 环境要求

含有 w80x 的板子。

## 编译和烧录

示例位置：`examples/peripheral/hash`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5) main: ver: 2.1.6dev build at Nov 26 2024 17:00:33
[I] (5) main: boot reason 0
[I] (5) main: heap size 272616(266.23KB)
[I] (6) main: flash size 0x200000(2MB)
[I] (8) hash_example: hash demo.
[I] (8) hash_example: md5 test success
[I] (8) hash_example: sha1 test success
[I] (8) hash_example: hash demo.
[I] (1008) hash_example: hash demo.
[I] (2008) hash_example: hash demo.
[I] (3008) hash_example: hash demo.
[I] (4008) hash_example: hash demo.
```