# scan

## 功能概述
本例演示了如何扫描可用的 AP。

## 环境要求

1. 在 menuconfig  中配置扫描列表的大小 "Max size of scan list"，默认为 10，范围是 0-20。

## 编译和烧录

示例位置：`examples/wifi/scan`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志（由于版本间可能存在差异，具体细节以实际使用版本为准）

```
D/wifi            [1.806] distribution event 0
D/wifi            [1.810] scan finish number 16
I/scan_example    [1.818] Total APs scanned = 16
I/scan_example    [1.822] SSID          test_123
I/scan_example    [1.826] RSSI          -37
I/scan_example    [1.830] Authmode      WM_WIFI_AUTH_WPA_WPA2_PSK
I/scan_example    [1.834] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.840] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.846] Channel               2

I/scan_example    [1.850] SSID          test_1010
I/scan_example    [1.854] RSSI          -37
I/scan_example    [1.858] Authmode      WM_WIFI_AUTH_WPA2_PSK
I/scan_example    [1.862] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.868] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.874] Channel               13

I/scan_example    [1.878] SSID          test_001
I/scan_example    [1.882] RSSI          -43
I/scan_example    [1.886] Authmode      WM_WIFI_AUTH_WPA_WPA2_PSK
I/scan_example    [1.890] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_TKIP_CCMP
I/scan_example    [1.898] Group Cipher  WM_WIFI_CIPHER_TYPE_TKIP
I/scan_example    [1.904] Channel               13

I/scan_example    [1.906] SSID
I/scan_example    [1.910] RSSI          -54
I/scan_example    [1.914] Authmode      WM_WIFI_AUTH_WPA2_PSK
I/scan_example    [1.918] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.924] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.930] Channel               1

I/scan_example    [1.934] SSID          Nohsi Fire System
I/scan_example    [1.938] RSSI          -56
I/scan_example    [1.942] Authmode      WM_WIFI_AUTH_WPA_WPA2_PSK
I/scan_example    [1.948] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.954] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.960] Channel               11

I/scan_example    [1.964] SSID          MEXS
I/scan_example    [1.968] RSSI          -70
I/scan_example    [1.970] Authmode      WM_WIFI_AUTH_WPA_WPA2_PSK
I/scan_example    [1.976] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.982] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [1.988] Channel               2

I/scan_example    [1.992] SSID          Ruijie-Test-Ipv6
I/scan_example    [1.996] RSSI          -22
I/scan_example    [2.000] Authmode      WM_WIFI_AUTH_WPA_PSK
I/scan_example    [2.004] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.010] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.016] Channel               6

I/scan_example    [2.020] SSID          WinnerMicro-Guest
I/scan_example    [2.024] RSSI          -43
I/scan_example    [2.028] Authmode      WM_WIFI_AUTH_WPA2_PSK
I/scan_example    [2.034] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.040] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.046] Channel               6

I/scan_example    [2.050] SSID          ChinaNet-4gQa
I/scan_example    [2.054] RSSI          -50
I/scan_example    [2.056] Authmode      WM_WIFI_AUTH_WPA_WPA2_PSK
I/scan_example    [2.062] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_TKIP_CCMP
I/scan_example    [2.070] Group Cipher  WM_WIFI_CIPHER_TYPE_TKIP
I/scan_example    [2.074] Channel               3

I/scan_example    [2.078] SSID          test_999
I/scan_example    [2.082] RSSI          -52
I/scan_example    [2.086] Authmode      WM_WIFI_AUTH_WPA_PSK
I/scan_example    [2.090] Pairwise Cipher       WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.096] Group Cipher  WM_WIFI_CIPHER_TYPE_CCMP
I/scan_example    [2.102] Channel               13

```