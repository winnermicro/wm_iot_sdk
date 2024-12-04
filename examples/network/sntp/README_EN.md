# Using SNTP to obtain network clock

## Overview
This example demonstrates how to synchronize network clocks through SNTP.

1. Connect to the designated router
2. Start SNTP
3. Query the default SNTP server domain name
4. Set the time zone to Beijing time "UTC+8"
5. Obtain network time through default SNTP server
6. Set the specified SNTP domain name and obtain the network time
7. Set the specified SNTP IP address and obtain the network time

## Requirements

1.Configure the WiFi SSID and WiFi Password in menuconfig.
2.SSID configuration path: SNTP example configuration -> WiFi SSID.
3.Password configuration path: SNTP example configuration -> WiFi Password.


## Building and flashing

Example Location：`examples/network/sntp`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Result

The following logs is displayed after successful operation

```
I/wm_nm_wifi      [0.722] start connecting new(12345678)
D/wm_nm_core      [2.262] distribution WM_WIFI_EV event 1
I/wm_nm_core      [2.266] new is connected
D/wifi            [2.272] distribution event 1
D/wifi            [2.276] sta connected 6C-CD-D6-EC-79-61 channel 13 authmode 3
D/wm_nm_core      [5.544] distribution WM_LWIP_EV event 0
I/wm_nm_core      [5.548] sta got ip: 192.168.1.3
D/wm_nm_core      [5.552] WM_NM_EV post event 4 success
I/example         [5.724] waiting for connect to new
I/example         [5.730] start sntp demo
I/example         [5.734] default 0st sntp server: ntp.sjtu.edu.cn
I/example         [5.740] default 1st sntp server: cn.ntp.org.cn
I/example         [5.744] default 2st sntp server: us.pool.ntp.org
D/dt              [39.474] add device rtc
I/example         [39.482] Beijing time: 2024-08-22 10:29:29
I/example         [40.486] get NTP from ntp.ntsc.ac.cn
I/example         [40.492] Beijing time: 2024-08-22 10:29:30
I/example         [41.544] get NTP from 120.25.115.20
I/example         [41.548] Beijing time: 2024-08-22 10:29:31
I/example         [41.554] finish sntp
```
