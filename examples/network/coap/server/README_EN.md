# CoAP Server

## Overview
This example demonstrates how to implement a simple CoAP Server.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.

## Building and Flashing

Example Location： `examples/network/coap/server`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
D/wifi [1.580] distribution event 1
D/wifi [1.584] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
D/wm_nm_core [2.536] distribution WM_LWIP_EV event 0
I/wm_nm_core [2.540] sta got ip: 192.168.3.34
D/wm_nm_core [2.544] WM_NM_EV post event 4 success
I/coap_srv [2.718] wifi station start success, network is ready!
```