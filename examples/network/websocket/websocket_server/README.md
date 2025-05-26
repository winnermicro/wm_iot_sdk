# Websocket 服务端

## 功能概述
本例演示了如何实现一个简单的 Websocket 服务端。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID ， WiFi Password 和 是否使用加密。

## 编译和烧录

示例位置：`examples/network/httpd_websoecket/Websocket_server`

编译、烧录等操作请参考：[快速入门](Websocket://zone.winnermicro.net:60800/w800/zh_CN/latest/get_started/index.html)

## 注意

本示例实现的一个 echo 功能的 Websocket server, 收到客户端发送过来的数据，会发回到客户端。

客户端可以使用 Postman 软件发送数据到本示例实现的 Server 端。


## 证书

当使用 https 时，需要证书，示例 main.c 中使用的自签名证书由下面命令生成：

  generate a cert which signed by itself:

    1 Generate ECDSA private key:

        openssl ecparam -genkey -name prime256v1 -out server.key

    2 Generate Certificate Signing Request (CSR):

        openssl req -new -key server.key -out server.csr

    3 Generate self signed certificate (99 Years):

        openssl x509 -req -in server.csr -signkey server.key -out server.crt -days 36135

实际产品中把 main.c 中的证书换成自己的证书。

## 运行结果

成功运行将输出如下日志

```
flash device complete
connecting serial...
serial connected
[I] (10) main: ver: 2.3-alpha.2 build at Feb  6 2025 15:35:27
[I] (16) main: boot reason 4
[I] (19) main: heap size 262792(256.63KB)
[I] (23) main: flash size 0x200000(2MB)

W800: [I] (29) example_lws: Start Websocket server example
[I] (1972) wm_nm_wifi: start connecting new(12345678)
[I] (2717) wm_nm_core: new is connected
[I] (4463) wm_nm_core: sta got ip: 10.17.1.40
[I] (4477) example_lws: wifi station start success, network is ready!
[2025/02/06 16:06:30:4830] N: 179816: lws_create_context: LWS: 4.3.3-WM, MbedTLS-3.4.0 NET CLI SRV H1 WS ConMon IPv6-absent
[2025/02/06 16:06:30:5000] N: 179360:  mem: platform fd map:   120 bytes
[2025/02/06 16:06:30:5070] N: 178584: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2025/02/06 16:06:30:5140] N: 177496: __lws_lc_tag:  ++ [vh|0|default||443] (1)
[2025/02/06 16:06:30:5210] N: 177360:  Vhost 'default' using TLS mode
[2025/02/06 16:06:30:5280] N: 177032: lws_tls_server_vhost_backend_init: vh default: mem CA OK
[2025/02/06 16:06:30:5410] N: 174744: no client cert required
[2025/02/06 16:06:30:5480] N: 173896: [vh|0|default||443]: lws_socket_bind: source ads 0.0.0.0
[2025/02/06 16:06:30:5570] N: 173264: __lws_lc_tag:  ++ [wsi|1|listen|default||443] (2)
[I] (4565) example_lws: The websocket server listened on port 443
[2025/02/06 16:06:35:5470] N: 173264: [vh|0|default||443]: lws_tls_check_cert_lifetime:    vhost default: cert expiry: -13577d


W800: [2025/02/06 16:06:52:6310] N: 171680: __lws_lc_tag:  ++ [wsisrv|0|adopted] (1)
[2025/02/06 16:06:52:6390] N: 171680: lws_gate_accepts: on = 0
[2025/02/06 16:06:53:5540] N: 147248: lws_gate_accepts: on = 0
[I] (27609) example_lws: Client connected

[I] (31065) example_lws: Received msg from client:
hello
[I] (31072) example_lws: Send msg to client:
hello
```
