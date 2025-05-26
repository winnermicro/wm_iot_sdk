# Websocket 客户端

## 功能概述
本例演示了如何实现一个简单的 Websocket 客户端。

## 环境要求

1. 需要有一个能正常连接的 AP；
2. 在 menuconfig 中配置 WiFi SSID ， WiFi Password 以及 Websocket 服务端的 URL 地址；
3. 需要一个 Websocket Echo server, 客服发送消息给服务端，服务端把消息发送回客服端。

## 编译和烧录

示例位置：`examples/network/httpd_websoecket/Websocket_client`

编译、烧录等操作请参考：[快速入门](Websocket://zone.winnermicro.net:60800/w800/zh_CN/latest/get_started/index.html)


## 运行结果

成功运行将输出如下日志

```
flash device complete
connecting serial...
serial connected
[I] (12) main: ver: 2.3-alpha.2 build at Feb  8 2025 15:56:51
[I] (18) main: boot reason 4
[I] (20) main: heap size 262856(256.70KB)
[I] (25) main: flash size 0x200000(2MB)

W800: [I] (31) example_lws: Start websocket client example
[I] (1991) wm_nm_wifi: start connecting new(12345678)
[I] (2675) wm_nm_core: new is connected
[I] (4482) wm_nm_core: sta got ip: 10.17.1.40
[I] (4496) example_lws: wifi station start success, network is ready!
[I] (4502) example_lws: protocol=wss,host=echo.websocket.org,port=443,path=/
[2025/02/08 15:57:35:5090] N: 167304: lws_create_context: LWS: 4.3.3-WM, MbedTLS-3.4.0 NET CLI SRV H1 WS ConMon IPv6-absent
[2025/02/08 15:57:35:5250] N: 166896:  mem: platform fd map:   120 bytes
[2025/02/08 15:57:35:5320] N: 166120: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2025/02/08 15:57:35:5390] N: 164976: __lws_lc_tag:  ++ [vh|0|default||-1] (1)
[2025/02/08 15:57:35:5590] N: 160376: __lws_lc_tag:  ++ [wsicli|0|WS/h1/default/echo.websocket.org] (1)
[2025/02/08 15:57:36:9260] N: 155000: lws_gate_accepts: on = 0
[2025/02/08 15:57:39:6240] N: 123584: lws_gate_accepts: on = 0
[I] (9104) example_lws: ev=LWS_CALLBACK_CLIENT_ESTABLISHED
[I] (9109) example_lws: send message:
Hello, server!
[I] (9115) example_lws: LWS_CALLBACK_CLIENT_ESTABLISHED
[I] (9122) example_lws: ev=LWS_CALLBACK_RECEIVE
[I] (9127) example_lws: Received message:
Request served by 7811941c69e658
[I] (9445) example_lws: ev=LWS_CALLBACK_RECEIVE
[I] (9449) example_lws: Received message:
Hello, server!
```
