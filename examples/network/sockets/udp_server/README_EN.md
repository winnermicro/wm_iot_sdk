# UDP Server

## Overview
The application creates a UDP socket using a specified port number and waits for connection requests from clients.
 After accepting a client request, a connection is established between the server and the client, and the application
  waits to receive some data from the client. The received data is printed as ASCII text and sent back to the client. 
  After the client responds, the application prints the received reply as ASCII text, waits for 2 seconds, 
  and sends another message. If there is no response from the client, the application waits for 10 seconds, 
  prints "recvfrom failed: errno 11," and resends the message.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
Only the [netcat](http://netcat.sourceforge.net) tool is used here as an example for send and receive many kinds of packets.
Note: please replace "192.168.3.208 3333" with desired IPV4/IPV6 address and port number in the following command.
If want to use this RECVINFO function, please enable "Enable IP_PKTINFO option" in menuconfig,
this function can only resolve the destination address of IPV4.

### Send UDP packet via netcat
```
echo "hello" | nc -w1 -u 192.168.3.208 3333
```

### Receive UDP packet via netcat
```
echo "hello" | nc -w1 -u 192.168.3.208 3333
```

### UDP server using netcat
```
nc -u -l 192.168.3.208 3333
```

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Port in menuconfig.

## Building and Flashing

Example Location： `examples/network/sockets/udp_server`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [5.032] got ip: 192.168.3.208
D/wm_nm_core      [5.036] WM_NM_EV post event 4 success
I/udp server      [5.542] wifi station start success, network is ready!
I/udp server      [5.550] Socket created
I/udp server      [5.554] Socket bound, port 3333
I/udp server      [5.558] Waiting for data
E/udp server      [15.562] recvfrom failed: errno 11
E/udp server      [15.568] Shutting down socket and restarting...
I/udp server      [15.574] Socket created
I/udp server      [15.580] Socket bound, port 3333
I/udp server      [15.584] Waiting for data
E/udp server      [25.586] recvfrom failed: errno 11
E/udp server      [25.592] Shutting down socket and restarting...
I/udp server      [25.598] Socket created
I/udp server      [25.604] Socket bound, port 3333
I/udp server      [25.608] Waiting for data
I/udp server      [32.734] Received 6 bytes from 192.168.3.220:
I/udp server      [32.740] hello

I/udp server      [32.746] Waiting for data

```