# deep sleep

## Overview
This example shows how to use the deep sleep mode with low power consumption. 
When the station connects to the AP successfully, 
the station will periodically switch between the active and sleep states, 
and the station can stay connected to the AP.

## Requirements
1. Configure WiFi SSID and WiFi Password in menuconfig
2. Configure WiFi listen interval in menuconfig

## Building and Flashing

Example Location： `examples/low_power/deep_sleep`

For compilation, burning, and more information, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output (Due to possible differences between versions, specific details shall be subject to the actual version used)

```
[I] (12) main: ver: 2.3-alpha.1 build at Jan 16 2025 11:36:08
[I] (18) main: boot reason 0
[I] (20) main: heap size 264944(258.73KB)
[I] (25) main: flash size 0x200000(2MB)
[I] (1980) demo: wifi_init_sta finished.
[I] (3980) demo: got ip: 192.168.3.177
[I] (3983) demo: connected to ap SSID:MY-WiFi password:12345678
```