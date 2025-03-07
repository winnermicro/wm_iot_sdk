# 使用 Net manager 启动 Wi-Fi station

## 功能概述
本例演示了如何通过 Net manager 快速启动 Wi-Fi station 并连接至指定路由器。

## 环境要求
1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 Wi-Fi SSID 和 Wi-Fi Password。
3. SSID 配置路径: Net manager example configuration -> Wi-Fi SSID 。
4. Password 配置路径: Net manager example configuration -> Wi-Fi Password 。

## 编译和烧录

示例位置：`examples/network/net_manager/wifi_station`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_wifi      [0.720] start connecting 61test(12345678)
D/wm_nm_core      [2.264] distribution WM_WIFI_EV event 1
I/wm_nm_core      [2.268] 61test is connected
D/wifi            [2.274] distribution event 1
D/wifi            [2.278] sta connected 6C-CD-D6-EC-79-61 channel 13 authmode 3
D/wm_nm_core      [4.038] distribution WM_LWIP_EV event 1
I/wm_nm_core      [4.044] sta got ip6: FE80:0000:0000:0000:0226:18FF:FE27:010F
D/wm_nm_core      [5.538] distribution WM_LWIP_EV event 0
I/wm_nm_core      [5.542] sta got ip: 192.168.1.2
D/wm_nm_core      [5.546] WM_NM_EV post event 4 success
I/example         [5.552] got ip, network is ready!
```
