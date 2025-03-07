.. _ble_peripheral_hr_sample:

peripheral_hr
########################


功能概述
*********

和 :ref:`外围设备 <ble_peripheral_sample>` 示例类似, 本示例提供了 HR (Heart Rate) GATT 服务. 
一旦建立连接，程序将向中央设备发送模拟的心率值。

环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect 或 nRF Toolbox APP

编译和烧录
********************

示例位置：``examples/bluetooth/peripheral_hr``    

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_

操作说明
************

#. 开发板烧录 peripheral_hr 示例编译的固件
#. 手机运行 nRF Toolbox APP

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bt_smp          [0.048] LE SC enabled
	I/bt_hci_core     [0.176] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.182] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.190] LMP: version 4.2 (0x08) subver 0x0709
	I/peripheral_hr   [0.196] Bluetooth initialized
	I/peripheral_hr   [0.202] Advertising successfully started
	
2. 运行 nRF Toolbox APP，选择 **HRM** 功能，发现 **WM Heartrate Sensor** 的设备后执行连接操作。连接成功后，在 APP 上可以看到动态刷新的模拟心率值

.. figure:: assert/peripheral_hr.svg
    :align: center 

	
3. 连接成功后，开发板输入如下日志

.. code-block:: console

	[I] (42636) peripheral_hr: Connected
	[I] (43862) bt: BAS Notifications enabled
	[I] (43962) bt: HRS notifications enabled 