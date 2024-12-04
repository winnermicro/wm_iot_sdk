# UDP 组播

## 功能概述
本示例展示了如何通过 BSD 风格的套接字接口使用 IPV4 和 IPV6 的 UDP 组播功能。
本示例主要演示的功能为:
- 监听指定的组播地址(IPV4 或 IPV6)。
- 打印接收到的所有 ASCII 格式 UDP 报文。
- 如果没有收到报文，它会周期性地(2 秒)打印 recvfrom failed: errno 11 并向组播地址发送自己的 UDP 报文。

在 IPV4 和 IPV6 双模式下，创建一个 IPV6 套接字，并使用 [RFC4038](https://tools.ietf.org/html/rfc4038) 中描述的"双模式"选项将其绑定到 IPV4 和 IPV6 的默认地址，同时加入配置的 IPV4 和 IPV6 组播组。否则只创建一个适合类型的套接字。

套接字总是绑定到默认地址，因此它也会接收单播分组。如果你只想接收特定地址的多播数据包，可以使用`bind()`绑定到该多播地址。

有许多主机端工具可以用来与 UDP 组播示例进行交互。其中一个命令行工具是 [socat](http://www.dest-unreach.org/socat/)，它可以发送和接收多种类型的数据包。

### 通过 socat 发送 IPV4 组播
```
echo "Hi there, IPv4!" | socat STDIO UDP4-DATAGRAM:232.10.11.12:3333,ip-multicast-if=(host_ip_addr)
```
将 `232.10.11.12:3333` 替换为 IPV4 组播地址和端口，将 `(host_ip_addr)` 替换为主机的 IP 地址。

### 通过 socat 接收 IPV4 组播
```
socat STDIO UDP4-RECVFROM:3333,ip-add-membership=232.10.11.12:(host_ip_addr)
```
将 `:3333` 和 `232.10.11.12` 分别替换为端口和 IPV4 组播地址，将 `(host_ip_addr)` 替换为主机IP地址。(“ip-add-membership=…” 可能不是必需的，这取决于你的网络配置。)

### 通过 socat 发送 IPV6 组播
```
echo "Hi there, IPV6!" | socat STDIO UDP6-DATAGRAM:[ff02::fc]:3333
```
将 `[ff02::fc]:3333` 分别替换为 IPV6 组播地址和端口。

### 通过 socat 接收 IPV6 组播
socat 不支持此功能，需要使用其它的工具或编程语言来接收 IPV6 组播数据包。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Multicast IP type。
4. 在 menuconfig 中配置 Multicast IPV4 Address。
5. 在 menuconfig 中配置 Multicast port。
6. 在 menuconfig 中配置 Multicast packet TTL。

## 编译和烧录

示例位置：`examples/network/sockets/udp_multicast`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [4.988] got ip: 192.168.3.208
D/wm_nm_core      [4.994] WM_NM_EV post event 4 success
I/udp multicast   [5.498] wifi station start success, network is ready!
I/udp multicast   [5.506] Configured IPV4 Multicast address 232.10.11.12
I/udp multicast   [7.516] Sending to IPV4 multicast address 232.10.11.12:3333...
I/udp multicast   [9.526] Sending to IPV4 multicast address 232.10.11.12:3333...
I/udp multicast   [11.536] Sending to IPV4 multicast address 232.10.11.12:3333...
I/udp multicast   [11.956] received 16 bytes from 192.168.3.220:
I/udp multicast   [11.962] Hi there, IPv4!

```