# TCP 非阻塞客户端和服务器

## 功能概述
本示例演示如何在非阻塞模式下简单使用 TCP 套接字。
在项目配置设置中，它可以配置作为 TCP 服务器或 TCP 客户端运行，或同时运行两者。

注意，默认只作为 TCP 客户端，这个例子使用字符串表示IP地址和端口，因此可以在 IPv4 和 IPv6 协议上使用。

### TCP客户端

在客户端模式下，该示例连接到一个配置好的主机名或地址，发送指定的有效载荷数据并等待响应。
然后关闭连接。默认情况下，它连接到一个公共 http 服务器并执行一个简单的 http `GET` 请求。

### TCP服务器

服务器示例使用指定的端口号创建一个非阻塞 TCP 套接字，并轮询直到来自客户端的连接请求到达结束。
接受客户端的请求后，服务器和客户端的连接就建立起来了。
建立后，应用程序轮询要从客户端接收的一些数据。
接收到的数据会被打印成ASCII文本并重传回客户端。

服务器可以监听指定的接口(通过配置的绑定地址)，并服务多个客户端。
当客户端的套接字关闭时，它恢复监听新连接。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 TCP server 和 TCP client。
4. 在 menuconfig 中配置 Client connection address or hostname 和 Client connection port。

## 编译和烧录

示例位置：`examples/network/sockets/non_blocking`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [4.546] got ip: 192.168.3.208
D/wm_nm_core      [4.550] WM_NM_EV post event 4 success
I/nonblocking-socket-client [5.056] wifi station start success, network is ready!
I/nonblocking-socket-client [5.074] Socket created, connecting to www.baidu.com:80
D/nonblocking-socket-client [5.084] connection in progress
I/nonblocking-socket-client [5.130] Client sends data to the server...
I/nonblocking-socket-client [5.138] Written: GET / HTTP/1.1


I/nonblocking-socket-client [5.248] Received: HTTP/1.1 200 OK
Accept-Ranges: bytes
Cache-Control: no-cache
Connection: keep-alive
Content-Length: 9508
Content-Type: tex

```