# CoAP 客户端

## 功能概述
本例演示了如何实现一个简单的 CoAP 客户端。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 CoAP 服务器的 Target URI，例如：coap://example.com:5683。

## 编译和烧录

示例位置：`examples/network/coap/client`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/wifi            [2.662] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
D/wm_nm_core      [3.476] distribution WM_LWIP_EV event 0
I/wm_nm_core      [3.480] sta got ip: 192.168.3.36
D/wm_nm_core      [3.486] WM_NM_EV post event 4 success
I/coap_cli        [3.490] wifi station start success, network is ready!
I/coap_cli        [3.496] CoAP Client Start !
Received:
Hello World!
D/coap_cli        [3.616] 10... 
D/coap_cli        [4.118] 9... 
D/coap_cli        [4.620] 8... 
D/coap_cli        [5.122] 7... 
D/coap_cli        [5.624] 6... 
D/coap_cli        [6.126] 5... 
D/coap_cli        [6.628] 4... 
D/coap_cli        [7.130] 3... 
D/coap_cli        [7.632] 2... 
D/coap_cli        [8.134] 1... 
D/coap_cli        [8.636] 0... 
I/coap_cli        [9.138] Starting again!
Received:
Hello World!
```