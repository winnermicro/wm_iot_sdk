# TCP Server

## Overview
The application creates a TCP socket with the specified port number and waits for a connection request from the client. 
After accepting a request from the client, connection between server and client is established and the application waits for some data to be received from the client. 
Received data are printed as ASCII text and retransmitted back to the client.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
Only the [netcat](http://netcat.sourceforge.net) tool is used here as an example for send and receive many kinds of packets.
Note: please replace "192.168.3.208 3333" with desired IPV4/IPV6 address and port number in the following command.

### TCP client using netcat
```
nc 192.168.3.208 3333
```

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Port in menuconfig.
4. Configure the TCP keep-alive idle time(s) in menuconfig, This time is the time between the last data transmission.
5. Configure the TCP keep-alive interval time(s) in menuconfig, This time is the interval time of keepalive probe packets.
6. Configure the TCP keep-alive packet retry send counts in menuconfig, This is the number of retries of the keepalive probe packet.

## Building and Flashing

Example Location： `examples/network/sockets/tcp_server`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [4.986] got ip: 192.168.3.208
D/wm_nm_core      [4.992] WM_NM_EV post event 4 success
I/tcp server      [5.496] wifi station start success, network is ready!
I/tcp server      [5.504] Socket created
I/tcp server      [5.508] Socket bound, port 3333
I/tcp server      [5.514] Socket listening
I/tcp server      [154.600] Socket accepted ip address: 192.168.3.220
I/tcp server      [161.136] Received 6 bytes: hello

I/tcp server      [169.246] Received 4 bytes: bye

```