# fast_scan

## 功能概述
本例演示如何使用 Wi-Fi 快速扫描功能连接 AP。

支持快速扫描和全信道扫描两种扫描方式。

- `快速扫描`:在这种模式下，即使没有完全扫描信道，扫描也会在检测到匹配的 AP 后立即结束。可以设置信号强度的阈值，以及选择AP 提供的所需认证模式。Wi-Fi 驱动程序将忽略不符合上述标准的 AP。
- `全信道扫描`:扫描完成后才结束。AP 的排序方法包括信号强度和认证模式。扫描完成后，Wi-Fi 驱动会选择最适合的 AP。

扫描后，Wi-Fi 将尝试连接。在大多数情况下，连接到信号强度最好的 AP。

## 环境要求

1. 需要有一个能正常连接的AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。

## 编译和烧录

示例位置：`examples/wifi/fast_scan`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

```
I/fast scan       [2.248] fast_scan finished.
D/wifi            [7.218] distribution event 1
D/wifi            [7.222] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
I/fast scan       [8.046] got ip: 192.168.3.102
D/wifi            [8.050] distribution event 0
D/wifi            [8.056] sta got ip 192.168.3.102

```