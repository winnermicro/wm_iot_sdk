.. _ble_peripheral_csc_sample:

peripheral_csc
#########################


功能概述
*********

和 :ref:`外围设备 <ble_peripheral_sample>` 示例类似，本示例提供了骑行速度和节奏配置 (Cycling Speed and Cadence) GATT 服务。


环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect APP 或 nRF Toolbox APP

编译和烧录
********************

示例位置：``examples/bluetooth/peripheral_csc``     

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	[I] (34) main: ver: 2.0.6dev build at Oct 21 2024 15:10:17
	[I] (81) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (82) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (82) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (82) peripheral_csc: Bluetooth initialized
	[I] (84) peripheral_csc: Advertising successfully started

2. 运行 nRF Toolbox APP，发现 **CSC peripheral** 的设备后，执行连接操作。
   连接成功后，在 APP 上可以看到动态刷新的 SPEED 和 CADENCE 参数

.. figure:: assert/peripheral_csc.svg
    :align: center 
	
3. 连接成功后，开发板输入如下日志

.. code-block:: console

	[I] (1212633) peripheral_csc: Connected   