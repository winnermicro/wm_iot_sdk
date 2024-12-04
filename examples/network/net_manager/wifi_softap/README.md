# 使用 Net manager 启动 Wi-Fi softAP

## 功能概述
本例演示了如何通过 Net manager 快速启动 Wi-Fi softAP。

## 环境要求
1. 在 menuconfig 中配置 softAP SSID 和 softAP Password。
2. SSID 配置路径: Net manager example configuration -> softAP SSID 。
3. Password 配置路径: Net manager example configuration -> softAP Password 。

## 编译和烧录

示例位置：`examples/network/net_manager/wifi_softap`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
D/wm_nm_core      [0.732] distribution WM_WIFI_EV event 5
D/wm_nm_core      [0.736] softap started
D/wm_nm_core      [0.742] WM_NM_EV post event 8 success
D/wifi            [0.746] distribution event 5
D/wifi            [0.750] softap started
I/example         [0.754] softAP is ready!
I/wm_nm_wifi      [0.758] start ap (wm-nm) success
```
