# station

## 功能概述
本例演示如何使用 Wi-Fi 的 station 功能。

## 环境要求

1. 需要有一个能正常连接的AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。

## 编译和烧录

示例位置：`examples/wifi/station`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

```
I/wifi station    [2.296] wifi_init_sta finished.
I/wifi station    [8.688] retry to connect to the AP
I/wifi station    [8.694] connect to the AP fail
D/wifi            [8.698] distribution event 2
D/wifi            [8.702] sta disconnected reason 2
D/wifi            [9.490] distribution event 1
D/wifi            [9.494] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
I/wifi station    [10.182] got ip: 192.168.3.102
D/wifi            [10.186] distribution event 0
D/wifi            [10.192] sta got ip 192.168.3.102
I/wifi station    [10.196] connected to ap SSID:Test-WiFi password:11111111

```