# CoreMark

## 功能概述

CoreMark 用于测试处理器性能。

## 环境要求

无。

## 编译和烧录

示例位置：`examples/benchmark/coremark`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 10318000
Total time (secs): 10.318010
Iterations/Sec   : 639.658209
Iterations       : 6600
Compiler version : GCC6.3.0
Compiler flags   :
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x33ff
Correct operation validated. See readme.txt for run and reporting rules.
CoreMark 1.0 : 639.658209 / GCC6.3.0  / STACK
Score (Coremarks/MHz):   2.67
```