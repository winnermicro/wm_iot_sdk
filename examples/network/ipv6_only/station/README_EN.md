# IPv6 Only For Station

## Overview
This example demonstrates how to create an IPv6 only network routine. After the device is connected to the network,
the application creates a UDP socket and attempts to connect to the server using a predefined IPv6 address and port number. Once the connection is successfully established, 
the application sends a message and then waits for a response. Upon receiving the server's reply, the application prints the received response as ASCII text, 
and sends another message. If however,there is no response from the server, the application waits for 10 seconds, prints "recvfrom failed: errno 11," and resends the message. 
There are many host-side tools available to interact with UDP/TCP servers/clients.
Only the  [NetAssist](http://www.cmsoft.cn/resource/102.html) tool is used here as an example for send and receive many kinds of packets.

### Listening to IPv6 UDP packets using NetAssist

1. Select UDP IPv6 protocol type.
2. Select an IP address in the same network segment as the device for the local host address, which needs to be the same as the IPV6 address configured in menuconfig.
3. Specify the host port, which needs to be the same as the configured port in menuconfig.

### Sending IPv6 UDP packets using NetAssist

1. Input the IPv6 address and port of the remote host device in the format of fe80:: 2a6d: eff: fe8d: 2dea: 3333.
2. Enter data in the send box and click send.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the IPv6 Address and Port in menuconfig.

## Building and Flashing

Example Location：`examples/network/ipv6_only/station`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

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

## Attention

If you need to access the public network, please pay attention to whether you have applied for a Global IPv6 address from the operator.
