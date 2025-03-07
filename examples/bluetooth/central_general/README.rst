.. _ble_central_general_sample:

central_general
################

功能概述
*********

本示例演示了 BLE 中央设备功能。程序运行后，扫描广播内容包含 ``BT_UUID_SERVICE`` 的外围设备，发现设备后执行连接操作。连接成功后根据 ``menuconfig -> Example configuration -> selectable item central example`` 选择项，查询 ``BT_UUID_CHAR_INDCIATE`` 或者 ``BT_UUID_CHAR_NOTIFY`` 的特征值并订阅。
示例同时演示了查询 ``BT_UUID_CHAR_READ_WRITE`` 特征值并向外围设备执行写操作。

::

    #define BT_UUID_SERVICE            0xFFF0
    #define BT_UUID_CHAR_READ_WRITE    0xFFF1
    #define BT_UUID_CHAR_INDCIATE      0xFFF2
    #define BT_UUID_CHAR_NOTIFY        0xFFF3

环境要求
************

* 支持 BLE 功能的开发板 A，运行本示例
* 支持 BLE 功能的开发板 B，运行 :ref:`periphal_general <ble_peripheral_general_sample>` 示例

编译和烧录
********************

示例位置：``examples/bluetooth/central_general``     

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_


运行结果 
*********

1. 开发板 A 烧录 central_general 示例的固件，成功运行将输出如下日志

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 17:04:44
	[I] (25) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (25) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (25) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (25) central: Bluetooth initialized
	[I] (27) central: Scanning successfully started

2. 等待开发板 B 烧录并运行 :ref:`periphal_general <ble_peripheral_general_sample>` 示例的固件

3. 开发板 A 扫描到开发板 B 的广播信息后，执行连接、更新 MTU、查询服务列表和订阅

.. code-block:: console


	[I] (5094) central: Device found: 28:6D:CE:11:22:33 (public) (RSSI -59)
	[I] (6153) central: Connected: 28:6D:CE:11:22:33 (public)
	[I] (6153) central: mtu_exchange: Current MTU = 23
	[I] (6153) central: mtu_exchange: Exchange MTU...
	[I] (6250) central: mtu_exchange_cb: MTU exchange successful (247)
	[I] (6400) central: [ATTRIBUTE] uuid 2803, handle 17
	[I] (6401) central: handle 17, prop 0x0a, value_handle 18 
	[I] (6500) central: [ATTRIBUTE] uuid 2800, handle 16
	[I] (6600) central: [ATTRIBUTE] uuid 2803, handle 19
	[I] (6700) central: [ATTRIBUTE] uuid 2902, handle 21
	[I] (6700) central: [SUBSCRIBED]

4. 订阅成功后，开发板 A 接收开发板 B 以 :c:func:`bt_gatt_notify` API 或者 :c:func:`bt_gatt_indicate` API 发送的数据，并打印统计速率（本示例数据仅仅用于功能演示）

.. code-block:: console


	[I] (22597) central: rx data rate:244 B/s
	[I] (23622) central: rx data rate:4392 B/s
	[I] (24622) central: rx data rate:4392 B/s
	[I] (25722) central: rx data rate:3904 B/s
	[I] (26747) central: rx data rate:4392 B/s
	[I] (27747) central: rx data rate:3904 B/s
	[I] (28747) central: rx data rate:4636 B/s
	[I] (29747) central: rx data rate:4392 B/s

.. note::

   本示例需要和 :ref:`外围设备示例 <ble_peripheral_general_sample>` 协同工作 