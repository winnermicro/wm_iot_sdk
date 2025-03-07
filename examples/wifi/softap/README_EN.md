# softap

## Overview
This example demonstrates how to utilize the SoftAP feature for Wi-Fi.

## Requirements
1. Configure WiFi SSID and WiFi Password in menuconfig
2. In menuconfig, you can also configure the WiFi Channel for the AP and the Maximal STA (station) connections, which refers to the maximum number of station connections.

## Building and Flashing

Example Location： `examples/wifi/softap`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, the following logs will be output(Due to possible differences between versions, specific details shall be subject to the actual version used)

```
I/wifi softap     [0.046] WM_WIFI_MODE_AP
D/wifi            [2.278] distribution event 5
D/wifi            [2.282] softap started
I/wifi softap     [2.290] wifi_init_softap finished. SSID:test-WiFi password:11111111 channel:1
I/wifi softap     [29.530] station 7E-B1-7B-28-F3-55 join, AID=1
D/wifi            [29.536] distribution event 7
D/wifi            [29.540] sta 7E-B1-7B-28-F3-55 aid 1 joined softap
D/dhcps           [29.918] ClientMacAddr=7E-B1-7B-28-F3-55, MsgType=1, ReqIpAddr=0.0.0.0, ServerIp=0.0.0.0
D/dhcps           [29.930] sent dhcp offer, ClientMacAddr=7E-B1-7B-28-F3-55, GivenIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/dhcps           [29.948] ClientMacAddr=7E-B1-7B-28-F3-55, MsgType=3, ReqIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/dhcps           [29.960] sent dhcp ack, ClientMacAddr=7E-B1-7B-28-F3-55, GivenIpAddr=192.168.10.2, ServerIp=192.168.10.1
D/wifi            [29.974] distribution event 4
D/wifi            [29.978] sta 7E-B1-7B-28-F3-55 assigned ip 192.168.10.2

```