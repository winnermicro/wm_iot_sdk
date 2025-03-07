# 深度睡眠

## 功能概述
这个例子展示了如何使用低功耗的深度睡眠模式。 当 station 连接 AP 成功后，station 将周期性地在活动状态和休眠状态之间切换，station 可以与 AP 保持连接。

## 环境要求

1.  在 menuconfig 中配置 WiFi SSID 和 WiFi Password
2.  在 menuconfig  中配置 WiFi listen interval

## 编译和烧录

示例位置：`examples/low_power/deep_sleep`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

```
[I] (12) main: ver: 2.3-alpha.1 build at Jan 16 2025 11:36:08
[I] (18) main: boot reason 0
[I] (20) main: heap size 264944(258.73KB)
[I] (25) main: flash size 0x200000(2MB)
[I] (1980) demo: wifi_init_sta finished.
[I] (3980) demo: got ip: 192.168.3.177
[I] (3983) demo: connected to ap SSID:MY-WiFi password:12345678
```