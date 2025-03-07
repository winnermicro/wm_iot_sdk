# RTC 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 初始化 RTC
2. 设置 RTC 日历
3. 查询 RTC 日历，并打印当前时间
4. 注册 RTC 闹钟回调，并启用 RTC 闹钟
5. 在 RTC 闹钟回调函数中，循环启动用 RTC 闹钟

## 编译和烧录

示例位置：`examples/peripheral/rtc`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/dt              [0.010] add device rtc
I/rtc_example     [0.014] now is: 2024-07-01 12:00:00
I/rtc_example     [4.912] alarm arrived: 2024-07-01 12:00:05
I/rtc_example     [9.904] alarm arrived: 2024-07-01 12:00:10
I/rtc_example     [14.898] alarm arrived: 2024-07-01 12:00:15
I/rtc_example     [19.892] alarm arrived: 2024-07-01 12:00:20
I/rtc_example     [24.888] alarm arrived: 2024-07-01 12:00:25
I/rtc_example     [29.880] alarm arrived: 2024-07-01 12:00:30
```
