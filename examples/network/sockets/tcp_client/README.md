# TCP 客户端

## 功能概述
应用程序创建一个 TCP 套接字，并尝试使用预定义的 IP 地址和端口号连接到服务器。
成功建立连接后，应用程序等待服务器发送数据。
在服务器发送数据后，应用程序立即将收到的数据回传给服务器。

有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。
这里仅以 [netcat](http://netcat.sourceforge.net) 工具为例，用于发送和接收各种数据包。
注意：请在以下命令中将 “192.168.3.220 3333” 替换为设备获得的 IPV4/IPV6 地址和端口号。

另外，您可以借用部分公共服务器，测试设备的 TCP 连接功能，可以使用 [合宙 TCP/UDP web 测试工具](http://netlab.luatos.com)

注意：该服务器非联盛德运营，可能存在服务不可用的场景。用户可自行搜索可用服务器进行尝试。


### 使用 netcat 作为 TCP 服务器
```
nc -l 192.168.3.220 3333
```

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 IPv4 或 IPv6 Address 和 Port。

## 编译和烧录

示例位置：`examples/network/sockets/tcp_client`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (5832) wm_nm_core: got ip: 192.168.3.208
[D] (7778) wm_nm_core: WM_NM_EV post event 4 success
[I] (7778) tcp client:  wifi station start success, network is ready!
[I] (9696) tcp client: Socket created, connecting to 192.168.3.220:3333
[I] (11638) tcp client: Successfully connected
[I] (13732) tcp client: Received 861 bytes from fe80::be5d:ec85:964b:8969
[I] (13732) tcp client: Send 861 bytes to fe80::be5d:ec85:964b:8969

```
