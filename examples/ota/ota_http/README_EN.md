# OTA_HTTP

## Overview
This application serves as a comprehensive example of Over-The-Air (OTA) firmware upgrade functionality on a device using the `WM IoT SDK`. It demonstrates the process of connecting to a Wi-Fi network, initiating an OTA update, and handling various states of the update process.

## Requirements

1. Create a firmware HTTP server using python with the command `python -m http.server [port]`. Here is an example:

   ```
   python -m http.server 8080
   ```

2. Configure `EXPLLE_WIFI_SSID`, `EXPLLE_WIFI_PASSWORD`, and `OTA_IMAGE_URI` using `wm.py menuconfig` or `wm.py guiconfig`.

3. The partition table used in this project must include the `app_ota` partition. For the partition table, please refer to [Partition Table Mechanism](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/partition_table.html)

## Building and Flashing

Example Location： `examples\ota\ota_http`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs.

```
D/wifi            [4.276] sta got ip 10.17.1.122
D/ota_ops         [4.282] ota partition offset=0x0000f000,size=0x109000
D/ota_ops         [4.292] app_ota partition offset=0x00118000,size=0xc8000
D/example         [4.722] OTA progress:0-0%.
D/example         [4.726] OTA progress:1-0%.
D/NO_TAG          [4.732] pHTTPSession->HttpConnection.HttpSocket=1

D/ota_http        [4.888] WM_HTTP_EVENT_CONNECTED
D/ota_http        [4.898] WM_HTTP_EVENT_HEADER_SENTED
D/ota_http        [5.090] WM_HTTP_EVENT_RECV_HEADER
D/example         [5.094] OTA progress:3-0%.
D/example         [5.098] OTA progress:4-0%.
D/ota_ops         [5.106] magic_no: A0FFFF9F
D/ota_ops         [5.110] img_attr: 00100001
D/ota_ops         [5.122] img_addr: 0800F400
D/ota_ops         [5.126] img_len: 00042F84
D/ota_ops         [5.130] img_header_addr: 0800F000
D/ota_ops         [5.134] upgrade_img_addr: 08118000
D/ota_ops         [5.140] org_checksum: 8F9605DC
D/ota_ops         [5.144] upd_no: 00000000
D/ota_ops         [5.148] ver: 0.3.0
D/ota_ops         [5.150] _reserved0: 00000000
D/ota_ops         [5.156] _reserved1: 00000000
D/ota_ops         [5.160] next: 00000000
D/ota_ops         [5.166] hd_checksum: C1C25DDC
.D/example         [5.188] OTA progress:4-1%.
.D/example         [5.224] OTA progress:4-2%.
.D/example         [5.258] OTA progress:4-4%.
.D/example         [5.298] OTA progress:4-5%.
.D/example         [5.332] OTA progress:4-7%.
.D/example         [5.366] OTA progress:4-8%.
.D/example         [5.406] OTA progress:4-10%.
.D/example         [5.440] OTA progress:4-11%.
.D/example         [5.478] OTA progress:4-13%.
.D/example         [5.514] OTA progress:4-14%.
.D/example         [5.546] OTA progress:4-16%.
.D/example         [5.586] OTA progress:4-17%.
.D/example         [5.622] OTA progress:4-19%.
.D/example         [5.656] OTA progress:4-20%.
.D/example         [5.692] OTA progress:4-22%.
.D/example         [5.728] OTA progress:4-23%.
.D/example         [5.762] OTA progress:4-25%.
.D/example         [5.800] OTA progress:4-26%.
.D/example         [5.836] OTA progress:4-28%.
.D/example         [5.870] OTA progress:4-29%.
.D/example         [5.904] OTA progress:4-31%.
.D/example         [5.940] OTA progress:4-32%.
.D/example         [5.974] OTA progress:4-34%.
.D/example         [6.012] OTA progress:4-35%.
.D/example         [6.046] OTA progress:4-37%.
.D/example         [6.080] OTA progress:4-38%.
.D/example         [6.116] OTA progress:4-40%.
.D/example         [6.150] OTA progress:4-41%.
.D/example         [6.194] OTA progress:4-43%.
.D/example         [6.228] OTA progress:4-44%.
.D/example         [6.262] OTA progress:4-46%.
.D/example         [6.304] OTA progress:4-47%.
.D/example         [6.338] OTA progress:4-49%.
.D/example         [6.374] OTA progress:4-50%.
.D/example         [6.412] OTA progress:4-52%.
.D/example         [6.446] OTA progress:4-53%.
.D/example         [6.480] OTA progress:4-55%.
.D/example         [6.518] OTA progress:4-56%.
.D/example         [6.552] OTA progress:4-58%.
.D/example         [6.588] OTA progress:4-59%.
.D/example         [6.624] OTA progress:4-61%.
.D/example         [6.658] OTA progress:4-62%.
.D/example         [6.694] OTA progress:4-64%.
.D/example         [6.730] OTA progress:4-65%.
.D/example         [6.766] OTA progress:4-67%.
.D/example         [6.804] OTA progress:4-68%.
.D/example         [6.836] OTA progress:4-70%.
.D/example         [6.872] OTA progress:4-71%.
.D/example         [6.906] OTA progress:4-73%.
.D/example         [6.940] OTA progress:4-74%.
.D/example         [6.976] OTA progress:4-76%.
.D/example         [7.010] OTA progress:4-77%.
.D/example         [7.044] OTA progress:4-79%.
.D/example         [7.080] OTA progress:4-80%.
.D/example         [7.116] OTA progress:4-82%.
.D/example         [7.150] OTA progress:4-83%.
.D/example         [7.184] OTA progress:4-85%.
.D/example         [7.220] OTA progress:4-86%.
.D/example         [7.258] OTA progress:4-88%.
.D/example         [7.294] OTA progress:4-89%.
.D/example         [7.334] OTA progress:4-91%.
.D/example         [7.370] OTA progress:4-92%.
.D/example         [7.402] OTA progress:4-93%.
.D/example         [7.442] OTA progress:4-95%.
.D/example         [7.476] OTA progress:4-96%.
.D/example         [7.504] OTA progress:4-98%.
.D/example         [7.532] OTA progress:4-99%.
.D/example         [7.542] OTA progress:4-100%.
.
D/example         [7.548] OTA progress:6-100%.
D/example         [7.552] OTA progress:7-100%.
D/example         [7.556] OTA finished.
D/example         [7.560] This example succeeds in running.
D/ota_ops         [7.572] ota reboot, please wait for the restart...
```