# Websocket Client

## Overview
This example demonstrates how to implement a simple Websocket client.

## Requirements
1. A normally connectable AP is required.
2. Configure the WiFi SSID, WiFi Password, and the URL address of the Websocket server in menuconfig.
3. A Websocket Echo server is needed. The client sends a message to the server, and the server sends the message back to the client.

## Building and Flashing

Example Location： `examples/network/httpd_websoecket/Websocket_client`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

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
