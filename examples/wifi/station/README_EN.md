# station

## Overview
This example demonstrates how to utilize the Wi-Fi station feature.

## Requirements
1. A properly connected AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.

## Building and Flashing

Example Location： `examples/wifi/station`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output(Due to possible differences between versions, specific details shall be subject to the actual version used)

```
I/wifi station    [2.296] wifi_init_sta finished.
I/wifi station    [8.688] retry to connect to the AP
I/wifi station    [8.694] connect to the AP fail
D/wifi            [8.698] distribution event 2
D/wifi            [8.702] sta disconnected reason 2
D/wifi            [9.490] distribution event 1
D/wifi            [9.494] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
I/wifi station    [10.182] got ip: 192.168.3.102
D/wifi            [10.186] distribution event 0
D/wifi            [10.192] sta got ip 192.168.3.102
I/wifi station    [10.196] connected to ap SSID:Test-WiFi password:11111111

```