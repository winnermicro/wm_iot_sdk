# HTTP2 客户端

## 功能概述
本例演示了如何实现一个简单的HTTP2客户端并与服务器建立连接的过程。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置服务器的 Host 和 Port。

## 编译和烧录

示例位置：`examples/network/http2_client`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

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