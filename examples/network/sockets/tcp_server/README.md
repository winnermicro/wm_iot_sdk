# TCP 服务器

## 功能概述
应用程序使用指定的端口号创建一个 TCP 套接字，并等待来自客户端的连接请求。
接受客户端请求后，服务器和客户端之间建立连接，应用程序等待从客户端接收一些数据。
接收到的数据会被打印成 ASCII 文本并传回客户端。

有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。
这里仅以 [netcat](http://netcat.sourceforge.net) 工具为例，用于发送和接收各种数据包。
注意：请在以下命令中将 “192.168.3.208 3333” 替换为设备获得的 IPV4/IPV6 地址和端口号。

### 使用 netcat 作为 TCP 客户端
```
nc 192.168.3.208 3333
```

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Port，这是 TCP 端口号。
4. 在 menuconfig 中配置 TCP keep-alive idle time(s)，这是上一次数据传输之间的时间。
5. 在 menuconfig 中配置 TCP keep-alive interval time(s)，这是 keepalive 探测报文的间隔时间。
6. 在 menuconfig 中配置 TCP keep-alive packet retry send counts，这是 keepalive 探测报文的重试次数。

## 编译和烧录

示例位置：`examples/network/sockets/tcp_server`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [4.986] got ip: 192.168.3.208
D/wm_nm_core      [4.992] WM_NM_EV post event 4 success
I/tcp server      [5.496] wifi station start success, network is ready!
I/tcp server      [5.504] Socket created
I/tcp server      [5.508] Socket bound, port 3333
I/tcp server      [5.514] Socket listening
I/tcp server      [154.600] Socket accepted ip address: 192.168.3.220
I/tcp server      [161.136] Received 6 bytes: hello

I/tcp server      [169.246] Received 4 bytes: bye

```