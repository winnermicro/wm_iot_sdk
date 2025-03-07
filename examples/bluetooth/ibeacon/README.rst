.. _bluetooth_ibeacon_sample:

ibeacon
##################

功能概述
*********

本示例演示了 Apple iBeacon 功能。程序运行后开始广播 Apple 的 iBeacon 
数据包。广播包的信号强度可以通过 IBEACON_RSSI 调整(e.g. IBEACON_RSSI=0xb8 for -72 dBm RSSI @ 1 meter)。

环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect APP

编译和烧录
********************

示例位置：``examples/bluetooth/ibeacon``   

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_


运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/ibeacon         [0.002] Starting iBeacon Demo
	I/bt_hci_core     [0.028] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.034] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.040] LMP: version 4.2 (0x08) subver 0x0709
	I/ibeacon         [0.046] Bluetooth initialized
	I/ibeacon         [0.054] iBeacon started

2. 运行 nRF Connect APP, 执行扫描功能可以发现 **iBeacon** 的设备

.. figure:: assert/ibeacon.svg
    :align: center

.. warning::
   示例发送广播时，使用随机地址。根据 **广播信息** 确认示例是否正确运行

