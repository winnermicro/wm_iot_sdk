# Start Wi-Fi station and connect to router by net manager

## Overview
This example demonstrates how to quickly start a Wi-Fi station and connect to a specified router through Net Manager.

## Requirements
1. We need an AP that can connect normally.
2. Configure Wi-Fi SSID and Wi-Fi Password in menuconfig.
3. SSID configuration path: Net manager example configuration -> Wi-Fi SSID.
4. Password configuration path: Net manager example configuration -> Wi-Fi Password.

## Building and Flashing

Example Location：`examples/network/net_manager/wifi_station`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

Upon successful execution, it will output the following logs


```
I/wm_nm_wifi      [0.720] start connecting 61test(12345678)
D/wm_nm_core      [2.264] distribution WM_WIFI_EV event 1
I/wm_nm_core      [2.268] 61test is connected
D/wifi            [2.274] distribution event 1
D/wifi            [2.278] sta connected 6C-CD-D6-EC-79-61 channel 13 authmode 3
D/wm_nm_core      [4.038] distribution WM_LWIP_EV event 1
I/wm_nm_core      [4.044] sta got ip6: FE80:0000:0000:0000:0226:18FF:FE27:010F
D/wm_nm_core      [5.538] distribution WM_LWIP_EV event 0
I/wm_nm_core      [5.542] sta got ip: 192.168.1.2
D/wm_nm_core      [5.546] WM_NM_EV post event 4 success
I/example         [5.552] got ip, network is ready!
```
