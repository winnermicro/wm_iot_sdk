# UDP 客户端

## 功能概述
应用程序创建一个 UDP 套接字，并尝试使用预定义的 IP 地址和端口号连接到服务器。
成功建立连接后，应用程序发送消息并等待响应。
在服务器应答后，应用程序将收到的应答打印为 ASCII 文本，等待 2 秒并发送另一个消息。
如果服务器无应答，应用程序等待 10 秒，打印 recvfrom failed: errno 11，重新发送消息。
有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。
这里仅以 [netcat](http://netcat.sourceforge.net) 工具为例，用于发送和接收各种数据包。
注意：请在以下命令中将 “192.168.3.220 3333” 替换为设备获得的 IPV4/IPV6 地址和端口号。
另外，您可以借用部分公共服务器，测试设备的 UDP 连接功能，可以使用[合宙 TCP/UDP web 测试工具](http://netlab.luatos.com)
注意，该服务器非联盛德运营，可能存在服务不可用的场景。用户可自行搜索可用服务器进行尝试。


### 通过 netcat 发送 UDP 数据包
```
echo "hello" | nc -w1 -u 192.168.3.220 3333
```
### 通过 netcat 接收 UDP 数据包
```
echo "hello" | nc -w1 -u 192.168.3.220 3333
```

### 使用 netcat 的 UDP 服务器
```
nc -u -l 192.168.3.220 3333
```

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 IPV4 Address 和 Port。

## 编译和烧录

示例位置：`examples/network/sockets/udp_client`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (3502) wm_nm_core: sta got ip: 192.168.3.82
[I] (3690) udp client: wifi station start success, network is ready!
[I] (3690) udp client: Socket created, sending to 192.168.3.72:3333
[I] (3690) udp client: Message sent
[E] (13692) udp client: recvfrom failed: errno 11
[E] (13692) udp client: Shutting down socket and restarting...
[I] (13692) udp client: Socket created, sending to 192.168.3.72:3333
[I] (13692) udp client: Message sent
[I] (17922) udp client: Received 8 bytes from 192.168.3.72:
[I] (17922) udp client: hello123
[I] (19922) udp client: Message sent

```
