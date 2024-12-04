# 使用 SNTP 以获取网络时钟

## 功能概述
本例演示了如何通过 SNTP 同步网络时钟

1. 连接到指定路由器
2. 启动 SNTP
3. 查询默认 SNTP 服务器域名
4. 设置时区为北京时间 "UTC+8"
5. 通过默认 SNTP 服务器获取网络时间
6. 设置指定 SNTP 域名，并获取网络时间
7. 设置指定 SNTP IP 地址，并获取网络时间

## 环境要求
1. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
2. SSID 配置路径: SNTP example configuration -> WiFi SSID 。
3. Password 配置路径: SNTP example configuration -> WiFi Password 。

## 编译和烧录

示例位置：`examples/network/sntp`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

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
