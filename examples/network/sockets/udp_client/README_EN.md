# UDP Client

## Overview
The application creates a UDP socket and attempts to connect to the server using a predefined IP address and port number. Once the connection is successfully established, 
the application sends a message and waits for a response. After receiving the server's reply, the application prints the received response as ASCII text, waits for 2 seconds, 
and sends another message. If there is no response from the server, the application waits for 10 seconds, prints "recvfrom failed: errno 11," and resends the message. 
There are many host-side tools available to interact with UDP/TCP servers/clients.
Only the [netcat](http://netcat.sourceforge.net) tool is used here as an example for send and receive many kinds of packets.
Note: please replace "192.168.3.220 3333" with desired IPV4/IPV6 address and port number in the following command.

In addition, you can borrow some public servers to test the UDP connection function of the device. You can use the [HeZhou TCP/UDP web testing tool](http://netlab.luatos.com)
Note that this server is not operated by Lianshengde and there may be scenarios where the service is unavailable. Users can search for available servers themselves to try.

### Send UDP packet via netcat
```
echo "hello" | nc -w1 -u 192.168.3.220 3333
```

### Receive UDP packet via netcat
```
echo "hello" | nc -w1 -u 192.168.3.220 3333
```

### UDP server using netcat
```
nc -u -l 192.168.3.220 3333
```

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the IPV4 Address and Port in menuconfig.

## Building and Flashing

Example Location： `examples/network/sockets/udp_client`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

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