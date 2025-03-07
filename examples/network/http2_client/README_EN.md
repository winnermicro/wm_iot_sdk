# HTTP2 Client

## Overview

This example demonstrates how to implement a simple HTTP2 client and establish a connection to the server.

## Requirements

1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the server's host and port in menuconfig.

## Building and Flashing

Example Location： `examples/network/http2_client`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [2.540] sta got ip: 192.168.3.34
D/wm_nm_core      [2.546] WM_NM_EV post event 4 success
I/example         [2.718] wifi station start success, network is ready!
I/http2           [2.734] +IPV4:"192.168.3.8"
I/http2           [2.798]  ok

I/http2           [2.800]   . Performing the SSL/TLS handshake...
I/http2           [2.948]  ok

I/example         [2.950] event 1 

I/example         [2.954] HTTP2 CONNECT
D/http2           [2.958] submit weight 16 exclusive 0
I/http2           [2.962] ID 1 submit weight 16 exclusive 0
D/http2           [2.968] submit weight 16 exclusive 1
I/http2           [2.972] ID 3 submit weight 16 exclusive 1
D/http2           [2.978] submit weight 192 exclusive 0
I/http2           [2.984] ID 5 submit weight 192 exclusive 0
D/http2           [2.988] submit weight 16 exclusive 0
I/http2           [2.994] ID 7 submit weight 16 exclusive 0
D/http2           [2.998] submit weight 16 exclusive 0
I/http2           [3.004] ID 9 submit weight 16 exclusive 0
I/example         [3.024] event 3 

I/example         [3.028] HTTP2 HEADER SEDNTED:0 :method: GET
I/http2           [3.032] header :method value GET 
I/example         [3.038] event 3 

...

<p>For online documentation and support please refer to
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>

I/http2           [4.606] streamid 9 close
I/http2           [9.608] ssl timeout or ready to close -26624 


EOF

I/http2           [9.616] [INFO] C ----------------------------> S (GOAWAY)

I/example         [9.624] HTTP status 200 200 405 200 200 
```