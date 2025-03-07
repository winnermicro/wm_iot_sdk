# TCP Client


## Function Overview
The application creates a TCP socket and attempts to connect to the server using a predefined IP address and port number. After successfully establishing the connection, the application waits for the server to send data. Once the server sends data, the application immediately echoes the received data back to the server.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
Only the [netcat](http://netcat.sourceforge.net) tool is used here as an example for send and receive many kinds of packets.
Note: please replace "192.168.3.220 3333" with desired IPV4/IPV6 address and port number in the following command.

In addition, you can borrow some public servers to test the TCP connection function of the device. You can use the [HeZhou TCP/UDP web testing tool](http://netlab.luatos.com)

Note: that this server is not operated by Lianshengde and there may be scenarios where the service is unavailable. Users can search for available servers themselves to try.

### TCP server using netcat
```
nc -l 192.168.3.220 3333
```

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the IPv4 or IPv6 Address and Port in menuconfig.

## Building and Flashing

Example Location： `examples/network/sockets/tcp_client`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
[I] (5832) wm_nm_core: got ip: 192.168.3.208
[D] (7778) wm_nm_core: WM_NM_EV post event 4 success
[I] (7778) tcp client:  wifi station start success, network is ready!
[I] (9696) tcp client: Socket created, connecting to 192.168.3.220:3333
[I] (11638) tcp client: Successfully connected
[I] (13732) tcp client: Received 861 bytes from fe80::be5d:ec85:964b:8969
[I] (13732) tcp client: Send 861 bytes to fe80::be5d:ec85:964b:8969

```
