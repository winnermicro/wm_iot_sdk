# TCP Server

## Overview
The application setup a softap and waits for the connection from wifi station device.
The application also creates a TCP server socket with the specified port number and waits for a connection request from the client. 
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
1. One or more working Wi-Fi Station is required, (maximum 8 stations).
2. Configure WiFi SSID and WiFi Password and WiFi Channel in menuconfig.
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
<SoftAP is start>
[I] (2003) tcp server: wifi_init_softap finished. SSID:mywifissid password:12345678 channel:1
[I] (2004) tcp server: softap ip: 192.168.10.1

<Created TCP Server socket successfully>
[I] (2004) tcp server: Socket created
[I] (2005) tcp server: Socket bound, port 3333
[I] (2005) tcp server: Socket listening
[I] (2291) tcp server: station 4A:CF:EE:A8:2F:12 join, AID=1

<A Station is connect to the SoftAP>
[I] (2305) wm_nm_core: 4a:cf:ee:a8:2f:12 assigned ip: 192.168.10.2

<A TCP Client on the station is connect to the TCP Sever on the SoftAP>
I/tcp server      [154.600] Socket accepted ip address: 192.168.3.220

<TCP Server on the SoftAP received TCP packets from TCP Client>
I/tcp server      [161.136] Received 6 bytes: hello
I/tcp server      [169.246] Received 4 bytes: bye
...

```