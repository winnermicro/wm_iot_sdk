# CoAP 服务器

## 功能概述
本例演示了如何实现一个简单的 CoAP 服务器。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。

## 编译和烧录

示例位置：`examples/network/coap/server`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/wifi [1.580] distribution event 1
D/wifi [1.584] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
D/wm_nm_core [2.536] distribution WM_LWIP_EV event 0
I/wm_nm_core [2.540] sta got ip: 192.168.3.34
D/wm_nm_core [2.544] WM_NM_EV post event 4 success
I/coap_srv [2.718] wifi station start success, network is ready!
```