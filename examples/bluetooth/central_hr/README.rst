.. _bluetooth_central_hr:

central_hr
#######################################

功能概述
*********

和 :ref:`central <bluetooth_central>` 示例工程类似， 本示例演示了扫描和连接 BLE 心率计外围设备功能。
连接成功后，周期性向外围设备发送心率信息。

环境要求
************

* 支持 BLE 功能的开发板 A，运行本示例
* BLE 心率计外围设备或者使用另外一块支持 BLE 功能的开发板 B，运行 :ref:`peripheral_hr <ble_peripheral_hr_sample>` 示例

编译和烧录
********************

示例位置：``examples/bluetooth/central_hr``     

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_


运行结果 (开发板 A)
********************

1. 成功运行将输出如下日志

.. code-block:: console


	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 17:31:25
	[I] (25) bt: LE SC enabled
	[I] (147) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (147) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (147) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (147) central_hr: Bluetooth initialized
	[I] (149) central_hr: Scanning successfully started
	[I] (963) central_hr: [DEVICE]: 51:0C:84:0F:95:F2 (random), AD evt type 3, AD data len 31, RSSI -75
	[I] (1022) central_hr: [DEVICE]: 61:CB:82:A1:3F:A0 (random), AD evt type 3, AD data len 31, RSSI -73

2. 等待开发板 B 烧录并运行 :ref:`peripheral_hr <ble_peripheral_hr_sample>` 示例的固件
	
3. 开发板 A 发现心率设备开发板 B 后，建立连接、查询服务列表和订阅服务

.. code-block:: console


	[I] (1023) central_hr: [DEVICE]: 28:6D:CE:11:22:33 (public), AD evt type 0, AD data len 11, RSSI -34
	[I] (2536) central_hr: Connected: 28:6D:CE:11:22:33 (public)
	[I] (2628) central_hr: [ATTRIBUTE] handle 25
	[I] (2728) central_hr: [ATTRIBUTE] handle 26
	[I] (2829) central_hr: [ATTRIBUTE] handle 28
	[I] (2829) central_hr: [SUBSCRIBED]
	
4. 订阅成功后，开发板 B 开始发送心率信息，开发板 A 日志信息

.. code-block:: console


	[I] (3731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (4781) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (5731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (6731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (7731) central_hr: [NOTIFICATION] data 0x2000aefc length 2

运行结果 (开发板 B)
********************

1. 烧录 :ref:`peripheral_hr <ble_peripheral_hr_sample>` 示例的固件，成功运行将输出如下日志

.. code-block:: console

	[I] (6) main: ver: 2.0.6dev build at Oct 17 2024 17:31:48
	[I] (28) bt: LE SC enabled
	[I] (158) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (158) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (159) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (159) peripheral_hr: Bluetooth initialized
	[I] (161) peripheral_hr: Advertising successfully started
	
2. 开发板 B 接收到开发板 A 的连接请求，输出如下日志

.. code-block:: console

	[I] (1251) peripheral_hr: Connected

3. 开发板 B 接收到开发板 A 的订阅消息，输出如下日志。然后定期向开发板 A 发送心率信息

.. code-block:: console

	[I] (1692) bt: HRS notifications enabled
	
.. note::

   本示例需要和 :ref:`心率计外围设备示例 <ble_peripheral_hr_sample>` 协同工作 