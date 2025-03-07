# UDP Multicast

## Overview
This example shows how to use the IPV4 & IPV6 UDP multicast features via the BSD-style sockets interface.

The behaviour of the example is:
- Listens to specified multicast addresses (one IPV4 and/or one IPV6).
- Print any UDP packets received as ASCII text.
- If no packet is received, it periodically (2 seconds) prints recvfrom failed: errno 11 and sends its own UDP packet to the multicast address.

In IPV4 & IPV6 dual mode, an IPV6 socket is created and the "dual mode" options described in [RFC4038](https://tools.ietf.org/html/rfc4038) are used to bind it to the default address for both IPV4 & IPV6 and join both the configured IPV4 & IPV6 multicast groups. Otherwise, a single socket of the appropriate type is created.

The socket is always bound to the default address, so it will also receive unicast packets. If you only want to receive multicast packets for a particular address, `bind()` to that  multicast address instead.

There are many host-side tools which can be used to interact with the UDP multicast example. One command line tool is [socat](http://www.dest-unreach.org/socat/) which can send and receive many kinds of packets.

### Send IPV4 multicast via socat
```
echo "Hi there, IPv4!" | socat STDIO UDP4-DATAGRAM:232.10.11.12:3333,ip-multicast-if=(host_ip_addr)
```
Replace `232.10.11.12:3333` with the IPV4 multicast address and port, and `(host_ip_addr)` with the host's IP address.

### Receive IPV4 multicast via socat
```
socat STDIO UDP4-RECVFROM:3333,ip-add-membership=232.10.11.12:(host_ip_addr)
```
Replace `:3333` and `232.10.11.12` with the port and IPV4 multicast address, respectively. Replace `(host_ip_addr)` with the host IP address.(The `,ip-add-membership=...` clause may not be necessary, depending on your network configuration.)

### Send IPV6 multicast via socat
```
echo "Hi there, IPV6!" | socat STDIO UDP6-DATAGRAM:[ff02::fc]:3333
```
Replace `[ff02::fc]:3333` with the IPV6 multicast address and port, respectively.

### Receive IPV6 multicast via socat
socat does not support this function.
Use a different tool or programming language to receive IPV6 multicast packets.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Multicast IP type in menuconfig.
4. Configure the Multicast IPV4 Address in menuconfig.
5. Configure the Multicast port in menuconfig.
6. Configure the Multicast packet TTL in menuconfig.

## Building and flashing

Example Location： `examples/network/sockets/udp_multicast`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

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