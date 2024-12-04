# power_save

## 功能概述
这个例子展示了如何使用 Wi-Fi 的省电模式。

省电模式仅适用于 station 模式。如果 modem 休眠模式被启用，在连接 AP 成功后，站点将周期性地在活动状态和休眠状态之间切换。在睡眠状态下，关闭 RF、PHY 和 BBP 以降低功耗。在 modem 休眠模式下，station 可以与 AP 保持连接。

- none: 这是默认模式。可以全功率工作。
- Minimum modem:在调制解调器最低休眠模式下，station 唤醒每个DTIM以接收信标。广播数据是在 DTIM 之后传输的，不会丢失。
- Maximum modem :在最大调制解调器休眠模式下，station 每隔一段时间就会唤醒一次，以接收信标。由于 station 在 DTIM 时间可能处于休眠状态，可能导致广播数据丢失。监听间隔越长，节省的电量越多，但广播数据更容易丢失。

## 环境要求

1.  在 menuconfig 中配置 WiFi SSID 和 WiFi Password
1.  在 menuconfig  中配置 WiFi listen interval 和 WiFi beacon timeout
1.  在 menuconfig  中配置 power save mode

## 编译和烧录

示例位置：`examples/wifi/power_save`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

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