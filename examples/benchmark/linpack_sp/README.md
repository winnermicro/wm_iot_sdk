# Linpack

## 功能概述

Linpack 用于测试浮点性能。

## 环境要求

无。

## 编译和烧录

示例位置：`examples/benchmark/linpack_sp`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
Unrolled Single Precision Linpack

Unrolled Single Precision Linpack

     norm. resid      resid           machep         x[0]-1        x[n-1]-1
       2.2                     e               e               e               e
    times are reported for matrices of order   120
      dgefa      dgesl      total       kflops     unit      ratio
 times for array with leading dimension of  121
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33834       0.06       0.62
 times for array with leading dimension of 120
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      33737       0.06       0.62
       0.03       0.00       0.03      34729       0.06       0.61
       0.03       0.00       0.03      33834       0.06       0.62
Unrolled Single  Precision 33834 Kflops ; 10 Reps
```