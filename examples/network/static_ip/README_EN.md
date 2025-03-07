# Set a static ip address

## Overview
This example demonstrates how to set the Station mode static IP address.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Static IP address, Static netmask address, and Static gateway address in the menuconfig.

## Building and Flashing

Example Location： `examples/network/static_ip`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [5.360] got ip: 192.168.3.208
D/wm_nm_core      [5.366] WM_NM_EV post event 4 success
I/wm_nm_api       [5.370] set ip:192.168.3.208 succes
I/wm_nm_api       [5.376] set gw:255.255.255.0 succes
I/wm_nm_api       [5.380] set net mask:192.168.3.1 succes
D/wifi station    [5.386] set static ip: 192.168.3.208, netmask: 255.255.255.0, gw: 192.168.3.1
```