# Start Wi-Fi softAP by net manager

## Overview
This example demonstrates how to quickly start Wi-Fi softAP through Net Manager.

## Requirements
1. Configure softAP SSID and softAP Password in menuconfig.
2. SSID configuration path: Net manager example configuration -> softAP SSID.
3. Password configuration path: Net manager example configuration -> softAP Password.

## Building and Flashing

Example Location：`examples/network/net_manager/wifi_softap`

compile, burn, and more, see: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

Upon successful execution, it will output the following logs

```
D/wm_nm_core      [0.732] distribution WM_WIFI_EV event 5
D/wm_nm_core      [0.736] softap started
D/wm_nm_core      [0.742] WM_NM_EV post event 8 success
D/wifi            [0.746] distribution event 5
D/wifi            [0.750] softap started
I/example         [0.754] softAP is ready!
I/wm_nm_wifi      [0.758] start ap (wm-nm) success
```
