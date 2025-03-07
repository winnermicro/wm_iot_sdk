# TCP 服务器

## 功能概述
应用程序会启动 WiFi SoftAP ，等待 WiFi Station 设备的 WiFi连线。
应用程序也会使用指定的端口号创建一个 TCP 服务器套接字，等待来自 TCP 客户端的连接请求。
接受客户端请求后，服务器和客户端之间建立连接，应用程序等待从客户端接收一些数据。
接收到的数据会被打印成 ASCII 文本并传回客户端。

有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。
这里仅以 [netcat](http://netcat.sourceforge.net) 工具为例，用于发送和接收各种数据包。
注意：请在以下命令中将 “192.168.3.208 3333” 替换为设备获得的 IPV4 地址和端口号。

### 使用 netcat 作为 TCP 客户端
```
nc 192.168.3.208 3333
```

## 环境要求

1. 需要一个或多个 (默认最多8个设备) WiFi Station 设备，如 手机或电脑。
2. 在 menuconfig 中配置 SoftAP 的 WiFi SSID 和 WiFi Password 和 WiFi Channel。
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
<启动 SoftAP>
[I] (2003) tcp server: wifi_init_softap finished. SSID:mywifissid password:12345678 channel:1
[I] (2004) tcp server: softap ip: 192.168.10.1

<成功创建 TCP Server 套接字>
[I] (2004) tcp server: Socket created
[I] (2005) tcp server: Socket bound, port 3333
[I] (2005) tcp server: Socket listening
[I] (2291) tcp server: station 4A:CF:EE:A8:2F:12 join, AID=1

<一个 WiFi Station 设备连接上 WiFi SoftAP>
[I] (2305) wm_nm_core: 4a:cf:ee:a8:2f:12 assigned ip: 192.168.10.2

< 一个TCP 客户端发起了 TCP 连线请求，并连上 TCP Server>
I/tcp server      [154.600] Socket accepted ip address: 192.168.3.220

<TCP 服务器收到来自 TCP Client 的数据>
I/tcp server      [161.136] Received 6 bytes: hello
I/tcp server      [169.246] Received 4 bytes: bye
...

```