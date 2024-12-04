.. _bluetooth_bthome_sensor_template_sample:

bthome_sensor_template
##################################

功能概述
*********

本示例实现了 `BTHome <https://bthome.io/>`_ 传感器的实例。

环境要求
************

* 支持 BLE 功能的开发板
* 支持 BTHome 的接收设备, 如运行 `Home Assistant <https://www.home-assistant.io/>`_ 集成了 BTHome 功能的 APP

编译和烧录
********************

示例位置：``examples/bluetooth/bthome_sensor_template`` 

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bthome_sensor   [0.056] Starting BTHome sensor template
	I/bt_hci_core     [0.082] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.088] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.096] LMP: version 4.2 (0x08) subver 0x0709
	I/bthome_sensor   [0.102] Bluetooth initialized

2. 运行 Home Assistant APP，在设置页面找到 Devices & Services 后， APP 将提示进行 BTHome Sensor 的配置
