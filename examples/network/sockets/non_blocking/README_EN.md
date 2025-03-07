# TCP non-blocking client and server

## Overview
The application aims to demonstrate a simple use of TCP sockets in a nonblocking mode.
It could be configured to run either a TCP server, or a TCP client, or both, in the project configuration settings.

The example is configured by default as the TCP client.

Note that the example uses string representation of IP addresses and ports and thus
could be used on both IPv4 and IPv6 protocols.

### TCP Client

In the client mode, the example connects to a configured hostname or address, sends the specified payload data and waits for a response,
then closes the connection. By default, it connects to a public http server and performs a simple http `GET` request.

### TCP Server

The server example creates a non-blocking TCP socket with the specified port number and polls till
a connection request from the client arrives.
After accepting a request from the client, a connection between server and client is
established, and the application polls for some data to be received from the client.
Received data are printed as ASCII text and retransmitted back to the client.

The server could listen on the specified interface (by the configured bound address) and serves multiple clients.
It resumes to listening for new connections when the client's socket gets closed.

## Requirements
1. A working AP is required.
2. Configure the TCP server and TCP client in the menuconfig.
3. In the menuconfig, configure the Client connection address or hostname and the Client connection port.

## Building and Flashing

Example Location： `examples/network/sockets/non_blocking`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result
Upon successful execution, it will output the following logs

```
I/wm_nm_core      [4.546] got ip: 192.168.3.208
D/wm_nm_core      [4.550] WM_NM_EV post event 4 success
I/nonblocking-socket-client [5.056] wifi station start success, network is ready!
I/nonblocking-socket-client [5.074] Socket created, connecting to www.baidu.com:80
D/nonblocking-socket-client [5.084] connection in progress
I/nonblocking-socket-client [5.130] Client sends data to the server...
I/nonblocking-socket-client [5.138] Written: GET / HTTP/1.1


I/nonblocking-socket-client [5.248] Received: HTTP/1.1 200 OK
Accept-Ranges: bytes
Cache-Control: no-cache
Connection: keep-alive
Content-Length: 9508
Content-Type: tex

```