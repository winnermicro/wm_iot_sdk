.. _bluetooth_central:

central
##################

功能概述
*********

本示例演示了 BLE 中央设备基本功能。程序运行后开始扫描，如果扫描到的设备
信号足够强(高于-70dBm)将建立连接。连接建立后，立即断开连接。连接断开后，将重新执行扫描、连接和断开操作，如此反复。


环境要求
************

* 支持 BLE 功能的开发板
* 支持 BLE 外设功能的蓝牙设备

编译和烧录
********************

示例位置：``examples/bluetooth/central``   

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 16:45:11
	[I] (25) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (25) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (25) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (25) central: Bluetooth initialized
	[I] (120) central: Device found: 62:A5:CD:FC:CE:BA (random) (RSSI -74)
	[I] (176) central: Device found: 28:6D:CE:11:22:33 (public) (RSSI -36)
	[I] (934) central: Connected: 28:6D:CE:11:22:33 (public)
	[I] (983) central: Disconnected: 28:6D:CE:11:22:33 (public) (reason 0x16)
	[I] (1012) central: Device found: 98:B6:73:27:96:71 (public) (RSSI -99)
	[I] (1012) central: Device found: 60:DE:F4:62:5D:13 (public) (RSSI -83)
	[I] (1012) central: Device found: 68:A1:07:7A:30:93 (random) (RSSI -75)
	[I] (1134) central: Device found: C1:29:46:29:00:D8 (public) (RSSI -95)
