.. _ble_direct_adv:

direct_adv
############

功能概述
*********

本示例演示了 BLE 的定向广播功能。在没有 ``BONDED`` 的时候，程序发送常规性可连接广播。一旦 ``BOND``  成功，
后续每次设备重新启动将发送指向 ``BONDED`` 设备的定向广播。除此之外，程序还提供了两种 BLE characteristics：分别支持 ``BOND`` 的写操作和不支持 ``BOND`` 的读操作。


.. warning::
   特别注意 iOS 设备不支持接收定向广播功能，具体信息请参考 "Accessory Design Guidelines for Apple Devices" 

环境要求
************

* 支持 BLE 功能的开发板
* 使能 Privacy 特性的中央功能的 BLE 设备。如另外一块支持 BLE 功能的开发板或者智能手机

编译和烧录
********************

示例位置：``examples/bluetooth/direct_adv``  

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bt_smp          [0.024] LE SC enabled
	I/bt_hci_core     [0.156] No ID address. App must call settings_load()
	I/direct_adv      [0.162] Bluetooth initialized
	I/bt_hci_core     [0.168] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.172] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.180] LMP: version 4.2 (0x08) subver 0x0709
	I/direct_adv      [0.190] Advertising successfully started
