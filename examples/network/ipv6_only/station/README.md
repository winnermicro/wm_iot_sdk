# IPv6 Only For Station

## 功能概述
本示例演示如何创建一个只使用 IPv6 网络例程。在设备接入网络后，
应用程序创建一个 UDP 套接字，并尝试使用预定义的 IP 地址和端口号连接到服务器。
成功建立连接后，应用程序发送消息并等待响应。
在服务器应答后，应用程序将收到的应答打印为 ASCII 文本，并发送下一条消息，消息序号加 1。
如果服务器无应答，应用程序等待 10 秒，打印 recvfrom failed: errno 11，重新发送消息。
有许多主机端工具可以用来与 UDP/TCP 服务器/客户端交互。

这里仅以 [NetAssist](http://www.cmsoft.cn/resource/102.html) 工具为例，用于发送和接收各种数据包。

### 使用 NetAssist 监听 IPv6 UDP 报文

1. 协议类型选择 UDP IPv6。
2. 本地主机地址选择与设备同网段的 IP 地址，需要与 menuconfig 中配置 IPV6 Address 相同。
3. 指定主机端口，需要与 menuconfig 中配置 Port 相同。


### 使用 NetAssist 发送 UDP 数据包

1. 远程主机输入设备的 IPv6 地址和端口，格式参考 fe80::2a6d:ceff:fe8d:2dea :3333。
2. 发送框中输入数据点击发送。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 IPV6 Address 和 Port。

## 编译和烧录

示例位置：`examples/network/ipv6_only/station`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
[I] (2) main: ver: 2.0.1dev build at Sep 19 2024 09:24:18
[I] (676) wm_nm_wifi: start connecting 61test(12345678)
[I] (3526) wm_nm_core: 61test is connected
[I] (5002) wm_nm_core: sta got ip6: FE80::2A6D:CEFF:FE8D:2DEA
[I] (5002) ipv6 only: GOT IPv6!
[I] (5002) ipv6 only: Socket created, sending to fe80::be5d:ec85:964b:8969:3333
[I] (5004) ipv6 only: Message sent
[E] (15004) ipv6 only: recvfrom failed: errno 11
[E] (15004) ipv6 only: Shutting down socket and restarting...
[I] (15004) ipv6 only: Socket created, sending to fe80::be5d:ec85:964b:8969:3333
[I] (15004) ipv6 only: Message sent
[E] (25006) ipv6 only: recvfrom failed: errno 11
[E] (25006) ipv6 only: Shutting down socket and restarting...
[I] (25006) ipv6 only: Socket created, sending to fe80::be5d:ec85:964b:8969:3333
[I] (25006) ipv6 only: Message sent
[I] (30076) ipv6 only: Received 25 bytes from fe80::be5d:ec85:964b:8969:
[I] (30078) ipv6 only: Message from PC, id is 1
[I] (32078) ipv6 only: Message sent
[I] (37204) ipv6 only: Received 24 bytes from fe80::be5d:ec85:964b:8969:
[I] (37204) ipv6 only: Message from PC, id is 2
```

## 注意

若需接入公网，请注意是否有向运营商申请 Global IPv6 地址
