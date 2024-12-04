# power_save

## Overview
This example shows how to use power save mode of wifi.

Power save mode is only applicable in station mode. If the modem sleep mode is enabled, the station will switch between active and sleep state after periodically connecting to AP successfully. In the sleep state, RF, PHY and BB are turned off to reduce power consumption. Station can keep connection with the AP in modem sleep mode.

- None: This is default mode, allowing for full power operation.
- Minimum Modem Sleep: In minimum modem sleep mode, station wakes up every DTIM to receive beacons. Broadcast data is transmitted after the DTIM and will not be lost.
- Maximum Modem Sleep: In maximum modem sleep mode, station wakes up every listen interval to receive beacons. Broadcast data may be lost because station may be in sleep state at DTIM time. The longer the listen interval, the more power is saved, but the easier it is for broadcast data to be lost.

## Requirements
1. Configure WiFi SSID and WiFi Password in menuconfig
2. Configure WiFi listen intervaland WiFi beacon timeout menuconfig
3. Configure power save mode in menuconfig

## Building and Flashing

Example Location： `examples/wifi/power_save`

For compilation, burning, and more information, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output (Due to possible differences between versions, specific details shall be subject to the actual version used)

```
I/power_save      [2.278] wm_wifi_set_ps().
I/power_save      [17.904] retry to connect to the AP
D/wifi            [20.132] distribution event 2
D/wifi            [20.138] sta disconnected reason 3
D/wifi            [22.944] distribution event 1
D/wifi            [22.948] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
I/power_save      [23.626] got ip: 192.168.3.102
D/wifi            [23.630] distribution event 0
D/wifi            [23.634] sta got ip 192.168.3.102

```