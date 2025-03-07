# softap

## 功能概述
本例演示如何使用 Wi-Fi 的 softap 功能。

## 环境要求

1. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password
2. 如果有需要，可在 menuconfig 中配置 AP 的信道 WiFi Channel 和最大 station 连接数 Maximal STA connections

## 编译和烧录

示例位置：`examples/wifi/softap`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

```
I/wifi softap     [0.046] WM_WIFI_MODE_AP
D/wifi            [2.278] distribution event 5
D/wifi            [2.282] softap started
I/wifi softap     [2.290] wifi_init_softap finished. SSID:test-WiFi password:11111111 channel:1
I/wifi softap     [29.530] station 7E-B1-7B-28-F3-55 join, AID=1
D/wifi            [29.536] distribution event 7
D/wifi            [29.540] sta 7E-B1-7B-28-F3-55 aid 1 joined softap
D/dhcps           [29.918] ClientMacAddr=7E-B1-7B-28-F3-55, MsgType=1, ReqIpAddr=0.0.0.0, ServerIp=0.0.0.0
D/dhcps           [29.930] sent dhcp offer, ClientMacAddr=7E-B1-7B-28-F3-55, GivenIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/dhcps           [29.948] ClientMacAddr=7E-B1-7B-28-F3-55, MsgType=3, ReqIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/dhcps           [29.960] sent dhcp ack, ClientMacAddr=7E-B1-7B-28-F3-55, GivenIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/wifi            [29.974] distribution event 4
D/wifi            [29.978] sta 7E-B1-7B-28-F3-55 assigned ip 192.168.10.2

```