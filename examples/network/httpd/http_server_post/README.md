# HTTP 服务端

## 功能概述
本例演示了如何实现一个简单的 HTTP 服务端。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password 以及是否使用HTTPS。

## 编译和烧录

示例位置：`examples/network/httpd_websoecket/http_server_post`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 具体步骤

1. PC 和 设备使用同一个局域网；
2. 编译时在 menuconfig 中配置 WiFi SSID 和 WiFi Password 以及是否使用HTTPS，之后烧录到设备中；
3. 设备启动后会开始连接 WiFi, 获取 IP； 当打印 "start httpd on port 80"，表示 http server 已经创建
4. PC 端使用工具 POST 数据到 Http server 端，参考 bash 命令如下：

  ```
    curl -X POST -H "Content-Type: application/json" -d '{"key1":"value1", "key2":"value2"}' http://10.17.1.40:80/jsontest
  ```

  如果curl发送到https服务器加参数 --insecure

  ```
    curl -X POST -H "Content-Type: application/json" -d '{"key1":"value1", "key2":"value2"}' --insecure https://10.17.1.40:443/jsontest
  ```

  可以使用 post man 等软件发送，注意上面 URL 中的 IP 改为设备实际获取到的 IP 地址。

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
[I] (14) example_lws: Start http server example
[I] (1970) wm_nm_wifi: start connecting new(12345678)
[I] (2744) wm_nm_core: new is connected
[I] (4960) wm_nm_core: sta got ip: 10.17.1.40
[I] (4970) example_lws: wifi station start success, network is ready!
[I] (4970) example_lws: start httpd on port 80
[2000/01/01 10:13:33:9700] N: 179344: lws_create_context: LWS: 4.3.3-WM, MbedTLS-3.4.0 NET CLI SRV H1 WS ConMon IPv6-absent
[2000/01/01 10:13:33:9760] N: 178936:  mem: platform fd map:   120 bytes
[2000/01/01 10:13:33:9760] N: 178160: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2000/01/01 10:13:33:9780] N: 177048: __lws_lc_tag:  ++ [vh|0|default||80] (1)
[2000/01/01 10:13:33:9790] N: 176504: [vh|0|default||80]: lws_socket_bind: source ads 0.0.0.0
[2000/01/01 10:13:33:9800] N: 175872: __lws_lc_tag:  ++ [wsi|1|listen|default||80] (2)
[2000/01/01 10:14:01:5360] N: 174632: __lws_lc_tag:  ++ [wsisrv|0|adopted] (1)
[I] (32547) example_lws: LWS_CALLBACK_HTTP
[I] (32547) example_lws: LWS_CALLBACK_HTTP_BODY
[I] (32548) example_lws: recv body=
{"key1":"value1", "key2":"value2"}
[I] (32548) example_lws: LWS_CALLBACK_HTTP_BODY_COMPLETION
[I] (32549) example_lws: response:
{"status": "success", "message": "Received your request."}
[2000/01/01 10:14:01:5660] N: 175056: __lws_lc_untag:  -- [wsisrv|0|adopted] (0) 30.000ms
```

## PC 端发送数据

```
curl -X POST -H "Content-Type: application/json" -d '{"key1":"value1", "key2":"value2"}' http://10.17.1.40:80/jsontest

% Total    % Received % Xferd  Average Speed   Time    Time     Time  Current  Dload  Upload   Total   Spent    Left  Speed
100    92  100    58  100    34    131     77 --:--:-- --:--:-- --:--:--   208{"status": "success", "message": "Received your request."}
```
