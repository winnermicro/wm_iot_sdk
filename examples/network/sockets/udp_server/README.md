# UDP 服务器

## 功能概述
应用程序使用指定的端口号创建一个 UDP 套接字，并等待来自客户端的连接请求。
接受客户端请求后，服务器和客户端之间建立连接，应用程序等待从客户端接收一些数据。
接收到的数据会被打印成 ASCII 文本并传回客户端。
在客户端应答后，应用程序将收到的应答打印为 ASCII 文本，等待 2 秒并发送另一个消息。
如果客户端无应答，应用程序等待 10 秒，打印 recvfrom failed: errno 11，重新发送消息。
有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。
这里仅以 [netcat](http://netcat.sourceforge.net) 工具为例，用于发送和接收各种数据包。
注意：请在以下命令中将 “192.168.3.208 3333” 替换为设备获得的 IPV4/IPV6 地址和端口号。
如果要使用 RECVINFO 功能，请在 menuconfig 中启用 "Enable IP_PKTINFO option" 功能，该功能只能解析 IPV4 的目的地址。

### 通过 netcat 发送 UDP 数据包
```
echo "hello" | nc -w1 -u 192.168.3.208 3333
```
### 通过 netcat 接收 UDP 数据包
```
echo "hello" | nc -w1 -u 192.168.3.208 3333
```

### 使用 netcat 的 UDP 服务器
```
nc -u -l 192.168.3.208 3333
```

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Port。

## 编译和烧录

示例位置：`examples/network/sockets/udp_server`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [5.032] got ip: 192.168.3.208
D/wm_nm_core      [5.036] WM_NM_EV post event 4 success
I/udp server      [5.542] wifi station start success, network is ready!
I/udp server      [5.550] Socket created
I/udp server      [5.554] Socket bound, port 3333
I/udp server      [5.558] Waiting for data
E/udp server      [15.562] recvfrom failed: errno 11
E/udp server      [15.568] Shutting down socket and restarting...
I/udp server      [15.574] Socket created
I/udp server      [15.580] Socket bound, port 3333
I/udp server      [15.584] Waiting for data
E/udp server      [25.586] recvfrom failed: errno 11
E/udp server      [25.592] Shutting down socket and restarting...
I/udp server      [25.598] Socket created
I/udp server      [25.604] Socket bound, port 3333
I/udp server      [25.608] Waiting for data
I/udp server      [32.734] Received 6 bytes from 192.168.3.220:
I/udp server      [32.740] hello

I/udp server      [32.746] Waiting for data

```