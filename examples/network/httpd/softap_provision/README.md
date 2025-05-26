#SoftAP 配网

## 功能概述
本例演示了如何实现一个简单的 SoftAP 配网

## 环境要求

1. 需要有一个能正常连接的 WiFi 热点；
2. 手机；
3. WM800 arduino开发版。

## 编译和烧录

示例位置：`examples/network/httpd_websoecket/softap_provision`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 具体步骤

1. 启动设备，会自动启动 SoftAP 和 Http server 服务，SoftAP ssid 为 wm-nm ；密码为 mypassword, IP 地址为 192.168.10.1 ；

2. 手机设置 WiFi 连接上面这个热点，正常会自动进入浏览器并显示配置页面；

3. 如果没有自动跳转到配置页面，手机打开浏览器输入网址 http://192.168.10.1，正常会进入一个可以输入 WiFi 名称和密码的页面；

4. 输入 WiFi 名称和密码，点击提交；设备会打印 "Start to connect"，配置页面提示 "已配置，如果失败请重试"

5. 当配网成功后，设备会有成功获取 IP 的打印，如："sta got ip: 10.17.1.147" ，并自动关闭 softAP 热点， 手机自动断开热点连接。

6. 如果设备配网失败，会打印 "Connect wifi station fail!"， 需要重启设备再重新配网。


## 运行结果

成功运行将输出如下日志

```
[I] (14) main: ver: 2.3-alpha.3 build at Feb 21 2025 09:19:07
[I] (15) main: boot reason 4
[I] (15) main: heap size 261096(254.98KB)
[I] (16) main: flash size 0x200000(2MB)

W800: [I] (18) example_lws: Start http server example
[I] (69) example_lws: softAP is ready!
[I] (69) wm_nm_wifi: start ap (wm-nm) success
[I] (69) example_lws: start httpd on port 80
[2000/01/02 06:38:43:0690] N: 152664: lws_create_context: LWS: 4.3.3-WM, MbedTLS-3.4.0 NET CLI SRV H1 WS ConMon IPV6-on
[2000/01/02 06:38:43:0750] N: 152256:  mem: platform fd map:   120 bytes
[2000/01/02 06:38:43:0760] N: 151440: __lws_lc_tag:  ++ [wsi|0|pipe] (1)
[2000/01/02 06:38:43:0770] N: 150264: __lws_lc_tag:  ++ [vh|0|default||80] (1)
[2000/01/02 06:38:43:0790] N: 149672: [vh|0|default||80]: lws_socket_bind: source ads 0.0.0.0
[2000/01/02 06:38:43:0800] N: 149000: __lws_lc_tag:  ++ [wsi|1|listen|default||80] (2)
[2000/01/02 06:38:43:0810] N: 148536: [vh|0|default||80]: lws_socket_bind: source ads ::
[2000/01/02 06:38:43:0820] N: 147848: __lws_lc_tag:  ++ [wsi|2|listen|default||80] (3)
[I] (14076) wm_nm_core: fc:02:96:ff:e2:6d assigned ip: 192.168.10.2
[I] (14077) example_lws: assigned IP to staion
[2000/01/02 06:38:57:8430] N: 145192: __lws_lc_tag:  ++ [wsisrv|0|adopted] (1)
[2000/01/02 06:38:57:8660] N: 145960: __lws_lc_untag:  -- [wsisrv|0|adopted] (0) 23.000ms
[2000/01/02 06:38:58:2270] N: 144808: __lws_lc_tag:  ++ [wsisrv|1|adopted] (1)
[2000/01/02 06:38:58:2370] N: 138720: __lws_lc_tag:  ++ [wsisrv|2|adopted] (2)
[2000/01/02 06:38:58:2510] N: 138712: __lws_lc_untag:  -- [wsisrv|1|adopted] (1) 24.000ms
[2000/01/02 06:38:58:2570] N: 139048: __lws_lc_tag:  ++ [wsisrv|3|adopted] (2)
[2000/01/02 06:38:58:2620] N: 139112: __lws_lc_untag:  -- [wsisrv|2|adopted] (1) 25.000ms
[2000/01/02 06:38:58:2840] N: 145096: __lws_lc_untag:  -- [wsisrv|3|adopted] (0) 27.000ms
[2000/01/02 06:38:58:2920] N: 145224: __lws_lc_tag:  ++ [wsisrv|4|adopted] (1)
[2000/01/02 06:38:58:3080] N: 145528: __lws_lc_untag:  -- [wsisrv|4|adopted] (0) 16.000ms
[2000/01/02 06:38:58:3180] N: 145240: __lws_lc_tag:  ++ [wsisrv|5|adopted] (1)
[2000/01/02 06:38:58:3380] N: 145544: __lws_lc_untag:  -- [wsisrv|5|adopted] (0) 20.000ms
[2000/01/02 06:38:58:3530] N: 145224: __lws_lc_tag:  ++ [wsisrv|6|adopted] (1)
[2000/01/02 06:38:58:3680] N: 145144: __lws_lc_untag:  -- [wsisrv|6|adopted] (0) 15.000ms
[2000/01/02 06:38:58:3730] N: 145504: __lws_lc_tag:  ++ [wsisrv|7|adopted] (1)
[2000/01/02 06:38:58:3980] N: 145104: __lws_lc_untag:  -- [wsisrv|7|adopted] (0) 25.000ms
[2000/01/02 06:38:58:4090] N: 145240: __lws_lc_tag:  ++ [wsisrv|8|adopted] (1)
[2000/01/02 06:38:58:4260] N: 145560: __lws_lc_untag:  -- [wsisrv|8|adopted] (0) 17.000ms
[2000/01/02 06:38:58:4320] N: 145240: __lws_lc_tag:  ++ [wsisrv|9|adopted] (1)
[2000/01/02 06:38:58:4480] N: 145544: __lws_lc_untag:  -- [wsisrv|9|adopted] (0) 17.000ms
[2000/01/02 06:38:58:4580] N: 145240: __lws_lc_tag:  ++ [wsisrv|a|adopted] (1)
[2000/01/02 06:38:58:4730] N: 145544: __lws_lc_untag:  -- [wsisrv|a|adopted] (0) 15.000ms
[2000/01/02 06:38:58:4840] N: 145240: __lws_lc_tag:  ++ [wsisrv|b|adopted] (1)
[2000/01/02 06:38:58:4990] N: 145544: __lws_lc_untag:  -- [wsisrv|b|adopted] (0) 15.000ms
[2000/01/02 06:38:58:5080] N: 145224: __lws_lc_tag:  ++ [wsisrv|c|adopted] (1)
[2000/01/02 06:38:58:5280] N: 145240: __lws_lc_untag:  -- [wsisrv|c|adopted] (0) 18.000ms
[2000/01/02 06:38:58:5300] N: 145240: __lws_lc_tag:  ++ [wsisrv|d|adopted] (1)
[2000/01/02 06:38:58:5480] N: 145456: __lws_lc_untag:  -- [wsisrv|d|adopted] (0) 17.000ms
[2000/01/02 06:38:58:5500] N: 145456: __lws_lc_tag:  ++ [wsisrv|e|adopted] (1)
[2000/01/02 06:38:58:5740] N: 144112: __lws_lc_untag:  -- [wsisrv|e|adopted] (0) 24.000ms
[2000/01/02 06:38:58:5760] N: 144112: __lws_lc_tag:  ++ [wsisrv|f|adopted] (1)
[2000/01/02 06:38:58:5780] N: 138448: __lws_lc_tag:  ++ [wsisrv|10|adopted] (2)
[2000/01/02 06:38:58:6000] N: 125952: __lws_lc_tag:  ++ [wsisrv|11|adopted] (3)
[2000/01/02 06:38:58:6060] N: 125744: __lws_lc_untag:  -- [wsisrv|f|adopted] (2) 30.000ms
[2000/01/02 06:38:58:6270] N: 139424: __lws_lc_untag:  -- [wsisrv|11|adopted] (1) 27.000ms
[2000/01/02 06:38:58:6370] N: 139072: __lws_lc_tag:  ++ [wsisrv|12|adopted] (2)
[2000/01/02 06:38:58:6560] N: 139040: __lws_lc_untag:  -- [wsisrv|12|adopted] (1) 19.000ms
[2000/01/02 06:38:58:6620] N: 139120: __lws_lc_tag:  ++ [wsisrv|13|adopted] (2)
[2000/01/02 06:38:58:6790] N: 139080: __lws_lc_untag:  -- [wsisrv|13|adopted] (1) 15.000ms
[2000/01/02 06:38:58:6810] N: 139080: __lws_lc_tag:  ++ [wsisrv|14|adopted] (2)
[2000/01/02 06:38:58:6980] N: 139000: __lws_lc_untag:  -- [wsisrv|14|adopted] (1) 17.000ms
[2000/01/02 06:38:58:7100] N: 139120: __lws_lc_tag:  ++ [wsisrv|15|adopted] (2)
[2000/01/02 06:38:58:7520] N: 138984: __lws_lc_untag:  -- [wsisrv|15|adopted] (1) 42.000ms
[2000/01/02 06:38:58:7680] N: 139120: __lws_lc_tag:  ++ [wsisrv|16|adopted] (2)
[2000/01/02 06:38:58:9830] E: 134256: lws_read_h1: Unhandled state 282
[2000/01/02 06:38:58:9910] N: 138904: __lws_lc_untag:  -- [wsisrv|10|adopted] (1) 412.000ms
[2000/01/02 06:38:58:9970] N: 138856: __lws_lc_tag:  ++ [wsisrv|17|adopted] (2)
[2000/01/02 06:38:59:0210] N: 139336: __lws_lc_untag:  -- [wsisrv|17|adopted] (1) 24.000ms
[2000/01/02 06:38:59:9410] N: 138568: __lws_lc_tag:  ++ [wsisrv|18|adopted] (2)
[2000/01/02 06:38:59:9570] N: 138936: __lws_lc_untag:  -- [wsisrv|18|adopted] (1) 16.000ms
[2000/01/02 06:39:00:5410] N: 138880: __lws_lc_tag:  ++ [wsisrv|19|adopted] (2)
[2000/01/02 06:39:00:5550] N: 138496: __lws_lc_untag:  -- [wsisrv|19|adopted] (1) 14.000ms
[2000/01/02 06:39:01:0070] N: 138656: __lws_lc_tag:  ++ [wsisrv|1a|adopted] (2)
[2000/01/02 06:39:01:0300] N: 138712: __lws_lc_untag:  -- [wsisrv|1a|adopted] (1) 23.000ms
[2000/01/02 06:39:01:2270] N: 145512: __lws_lc_untag:  -- [wsisrv|16|adopted] (0) 2.459s
[2000/01/02 06:39:01:2430] N: 145056: __lws_lc_tag:  ++ [wsisrv|1b|adopted] (1)
[2000/01/02 06:39:01:2600] N: 145096: __lws_lc_untag:  -- [wsisrv|1b|adopted] (0) 17.000ms
[2000/01/02 06:39:02:9320] N: 145056: __lws_lc_tag:  ++ [wsisrv|1c|adopted] (1)
[2000/01/02 06:39:02:9410] N: 144704: __lws_lc_untag:  -- [wsisrv|1c|adopted] (0) 9.000ms
[2000/01/02 06:39:04:2990] N: 144792: __lws_lc_tag:  ++ [wsisrv|1d|adopted] (1)
[2000/01/02 06:39:04:3140] N: 145112: __lws_lc_untag:  -- [wsisrv|1d|adopted] (0) 15.000ms
[2000/01/02 06:39:04:3270] N: 144744: __lws_lc_tag:  ++ [wsisrv|1e|adopted] (1)
[2000/01/02 06:39:04:3460] N: 145128: __lws_lc_untag:  -- [wsisrv|1e|adopted] (0) 19.000ms
[2000/01/02 06:39:04:3530] N: 144768: __lws_lc_tag:  ++ [wsisrv|1f|adopted] (1)
[2000/01/02 06:39:04:3660] N: 145112: __lws_lc_untag:  -- [wsisrv|1f|adopted] (0) 13.000ms
[2000/01/02 06:39:04:3810] N: 144784: __lws_lc_tag:  ++ [wsisrv|20|adopted] (1)
[2000/01/02 06:39:04:4380] N: 144728: __lws_lc_untag:  -- [wsisrv|20|adopted] (0) 57.000ms
[2000/01/02 06:39:04:4470] N: 144792: __lws_lc_tag:  ++ [wsisrv|21|adopted] (1)
[2000/01/02 06:39:04:4650] N: 145112: __lws_lc_untag:  -- [wsisrv|21|adopted] (0) 18.000ms
[2000/01/02 06:39:04:4680] N: 145032: __lws_lc_tag:  ++ [wsisrv|22|adopted] (1)
[2000/01/02 06:39:04:4840] N: 145112: __lws_lc_untag:  -- [wsisrv|22|adopted] (0) 16.000ms
[2000/01/02 06:39:04:4920] N: 144784: __lws_lc_tag:  ++ [wsisrv|23|adopted] (1)
[2000/01/02 06:39:04:5070] N: 145112: __lws_lc_untag:  -- [wsisrv|23|adopted] (0) 15.000ms
[2000/01/02 06:39:04:5130] N: 144792: __lws_lc_tag:  ++ [wsisrv|24|adopted] (1)
[2000/01/02 06:39:04:8190] N: 138856: __lws_lc_tag:  ++ [wsisrv|25|adopted] (2)
[2000/01/02 06:39:04:8290] N: 138472: __lws_lc_untag:  -- [wsisrv|25|adopted] (1) 10.000ms
[2000/01/02 06:39:06:7310] N: 145008: __lws_lc_untag:  -- [wsisrv|24|adopted] (0) 2.218s
[2000/01/02 06:39:06:7330] N: 145008: __lws_lc_tag:  ++ [wsisrv|26|adopted] (1)
[2000/01/02 06:39:06:7550] N: 145088: __lws_lc_untag:  -- [wsisrv|26|adopted] (0) 22.000ms
[2000/01/02 06:39:06:7660] N: 144792: __lws_lc_tag:  ++ [wsisrv|27|adopted] (1)
[2000/01/02 06:39:06:7840] N: 144784: __lws_lc_untag:  -- [wsisrv|27|adopted] (0) 18.000ms
[2000/01/02 06:39:06:7860] N: 144784: __lws_lc_tag:  ++ [wsisrv|28|adopted] (1)
[2000/01/02 06:39:06:8090] N: 144768: __lws_lc_untag:  -- [wsisrv|28|adopted] (0) 23.000ms
[2000/01/02 06:39:06:8110] N: 144768: __lws_lc_tag:  ++ [wsisrv|29|adopted] (1)
[2000/01/02 06:39:06:8240] N: 144696: __lws_lc_untag:  -- [wsisrv|29|adopted] (0) 13.000ms
[2000/01/02 06:39:06:8290] N: 144792: __lws_lc_tag:  ++ [wsisrv|2a|adopted] (1)
[2000/01/02 06:39:06:8430] N: 145040: __lws_lc_untag:  -- [wsisrv|2a|adopted] (0) 14.000ms
[2000/01/02 06:39:06:8520] N: 144704: __lws_lc_tag:  ++ [wsisrv|2b|adopted] (1)
[2000/01/02 06:39:06:8670] N: 145512: __lws_lc_untag:  -- [wsisrv|2b|adopted] (0) 14.000ms
[2000/01/02 06:39:06:8800] N: 144792: __lws_lc_tag:  ++ [wsisrv|2c|adopted] (1)
[2000/01/02 06:39:06:8950] N: 145512: __lws_lc_untag:  -- [wsisrv|2c|adopted] (0) 14.000ms
[2000/01/02 06:39:06:9080] N: 144792: __lws_lc_tag:  ++ [wsisrv|2d|adopted] (1)
[2000/01/02 06:39:06:9260] N: 145512: __lws_lc_untag:  -- [wsisrv|2d|adopted] (0) 17.000ms
[2000/01/02 06:39:06:9380] N: 144792: __lws_lc_tag:  ++ [wsisrv|2e|adopted] (1)
[2000/01/02 06:39:06:9590] N: 144752: __lws_lc_untag:  -- [wsisrv|2e|adopted] (0) 21.000ms
[2000/01/02 06:39:06:9610] N: 144752: __lws_lc_tag:  ++ [wsisrv|2f|adopted] (1)
[2000/01/02 06:39:06:9770] N: 144672: __lws_lc_untag:  -- [wsisrv|2f|adopted] (0) 14.000ms
[2000/01/02 06:39:06:9870] N: 144784: __lws_lc_tag:  ++ [wsisrv|30|adopted] (1)
[2000/01/02 06:39:07:0070] N: 144744: __lws_lc_untag:  -- [wsisrv|30|adopted] (0) 18.000ms
[2000/01/02 06:39:07:0090] N: 144744: __lws_lc_tag:  ++ [wsisrv|31|adopted] (1)
[2000/01/02 06:39:07:0220] N: 145128: __lws_lc_untag:  -- [wsisrv|31|adopted] (0) 13.000ms
[2000/01/02 06:39:07:0400] N: 144792: __lws_lc_tag:  ++ [wsisrv|32|adopted] (1)
[2000/01/02 06:39:07:0590] N: 145112: __lws_lc_untag:  -- [wsisrv|32|adopted] (0) 19.000ms
[2000/01/02 06:39:08:0840] N: 145280: __lws_lc_tag:  ++ [wsisrv|33|adopted] (1)
[2000/01/02 06:39:08:0970] N: 144912: __lws_lc_untag:  -- [wsisrv|33|adopted] (0) 13.000ms
[2000/01/02 06:39:08:3510] N: 144368: __lws_lc_tag:  ++ [wsisrv|34|adopted] (1)
[I] (25357) example_lws: Received POST data: {"cmd":"prov","wifiName":"new","wifiPassword":"12345678"}

[I] (25659) example_lws: Start to connect new,12345678
[I] (27606) wm_nm_wifi: start connecting new(12345678)
[I] (28349) wm_nm_core: new is connected
[I] (28362) wm_nm_core: softap stoped
[I] (28376) example_lws: softAP is ready!
[I] (28377) wm_nm_wifi: start ap (wm-nm) success
[I] (29096) wm_nm_core: sta got ip: 10.17.1.147
[I] (29108) example_lws: wifi station start success, network is ready!
[2000/01/02 06:39:12:1110] N: 136168: __lws_lc_untag:  -- [wsi|0|pipe] (2) 29.034s
[2000/01/02 06:39:12:1130] N: 142064: __lws_lc_untag:  -- [wsisrv|34|adopted] (0) 3.762s
[2000/01/02 06:39:12:1150] N: 143216: __lws_lc_untag:  -- [wsi|2|listen|default||80] (1) 29.033s
[2000/01/02 06:39:12:1200] N: 146488: __lws_lc_untag:  -- [vh|0|default||80] (0) 29.043s
[2000/01/02 06:39:12:1210] N: 146904: __lws_lc_untag:  -- [wsi|1|listen|default||80] (0) 29.041s
[I] (29123) example_lws: http server end
[I] (29125) wm_nm_core: softap stoped
[W] (29126) wm_nm_wifi: stop softAP
[I] (29597) wm_nm_core: sta got ip6: FE80::2A6D:CEFF:FE32:B4E5


W800: ifconfig
ifconfig
w01: flags=6f<UP BROADCAST RUNNING MULTICAST>  mtu 1500
     inet 10.17.1.147  netmask 255.255.254.0  gateway 10.17.1.1
     inet6 FE80::2A6D:CEFF:FE32:B4E5
     ether 28:6d:ce:32:b4:e5

```