# CoAP Client

## Overview
This example demonstrates how to implement a simple CoAP client.

## Requirements
1. A operational AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Target URI of the CoAP server in menuconfig, like coap://example.com:5683.

## Building and Flashing

Example Location： `examples/network/coap/client`

For compilation, flashing, and other operations, please refer to: [Quick Start](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output.

```
D/wifi            [2.662] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
D/wm_nm_core      [3.476] distribution WM_LWIP_EV event 0
I/wm_nm_core      [3.480] sta got ip: 192.168.3.36
D/wm_nm_core      [3.486] WM_NM_EV post event 4 success
I/coap_cli        [3.490] wifi station start success, network is ready!
I/coap_cli        [3.496] CoAP Client Start !
Received:
Hello World!
D/coap_cli        [3.616] 10... 
D/coap_cli        [4.118] 9... 
D/coap_cli        [4.620] 8... 
D/coap_cli        [5.122] 7... 
D/coap_cli        [5.624] 6... 
D/coap_cli        [6.126] 5... 
D/coap_cli        [6.628] 4... 
D/coap_cli        [7.130] 3... 
D/coap_cli        [7.632] 2... 
D/coap_cli        [8.134] 1... 
D/coap_cli        [8.636] 0... 
I/coap_cli        [9.138] Starting again!
Received:
Hello World!
```