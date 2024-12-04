# fast_scan

## Overview

This example demonstrates how to use Wi-Fi to quickly scan the functional connection AP.

It supports fast scan and full-channel scan.

- `Fast Scan` : In this mode, the scan ends as soon as a matching AP is detected, even if all channel have not been fully scanned. You can set a threshold for signal strength and select the desired authentication mode provided by the AP. The Wi-Fi driver will ignore APS that do not meet the above criteria.
- `Full-Channel Scan` : Only end when the scan is complete. The ranking method of APS includes signal strength and authentication mode. After scanning, the Wi-Fi driver selects the most suitable AP.

After scanning, Wi-Fi will try to connect. In most cases, the AP with the best signal strength is connected.

## Requirements
1. A properly connected AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig .

## Building and Flashing

Example Location： `examples/wifi/fast_scan`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, the following logs will be output(Due to possible differences between versions, specific details shall be subject to the actual version used)

```
I/fast scan       [2.248] fast_scan finished.
D/wifi            [7.218] distribution event 1
D/wifi            [7.222] sta connected 42-C1-82-80-2A-15 channel 6 authmode 3
I/fast scan       [8.046] got ip: 192.168.3.102
D/wifi            [8.050] distribution event 0
D/wifi            [8.056] sta got ip 192.168.3.102

```