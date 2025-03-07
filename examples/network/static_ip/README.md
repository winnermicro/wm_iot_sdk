# 设置静态 IP 地址

## 功能概述
本例演示了如何设置 Station 模式静态 IP 地址。

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Static IP address、Static netmask address 和 Static gateway address。

## 编译和烧录

示例位置：`examples/network/static_ip`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [5.360] got ip: 192.168.3.208
D/wm_nm_core      [5.366] WM_NM_EV post event 4 success
I/wm_nm_api       [5.370] set ip:192.168.3.208 succes
I/wm_nm_api       [5.376] set gw:255.255.255.0 succes
I/wm_nm_api       [5.380] set net mask:192.168.3.1 succes
D/wifi station    [5.386] set static ip: 192.168.3.208, netmask: 255.255.255.0, gw: 192.168.3.1
```