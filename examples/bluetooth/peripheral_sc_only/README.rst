.. _ble_peripheral_sc_only_sample:

peripheral_sc_only
#############################

功能概述
*********

和 :ref:`外围设备 <ble_peripheral_sample>` 示例不同的是，本示例使能了 BLE 的 ``Secure Connections Only`` 模式。


环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect APP

编译和烧录
********************

示例位置：``examples/bluetooth/peripheral_sc_only`` 

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bt_smp          [0.078] LE SC enabled
	I/bt_hci_core     [0.208] No ID address. App must call settings_load()
	I/sc_only         [0.214] Bluetooth initialized
	I/bt_hci_core     [0.220] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.226] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.234] LMP: version 4.2 (0x08) subver 0x0709
	I/sc_only         [0.300] Advertising successfully started

2. 运行 nRF Connect APP，执行扫描操作，发现 **SC only peripheral** 的设备后执行连接操作。
   连接成功后，在 nRF Connect APP 提示配对请求并确认后，弹出输入配对码对话框

.. figure:: assert/peripheral_sc_only.svg
    :align: center

3. 开发板接收到 nRF Connect APP 的连接请求，执行安全等级为 ``BT_SECURITY_L4`` 配对流程并打印配对码 **221727** 输出如下日志

.. code-block:: console

	[I] (9854) sc_only: Connected 45:8B:76:3F:1D:E8 (random)
	[I] (15454) sc_only: Passkey for 45:8B:76:3F:1D:E8 (random): 221727	

4. 在 nRF Connect APP 输入配对码并确认，开发板提示配对成功。将输出如下日志

.. code-block:: console

	[I] (31048) sc_only: Pairing Complete


	
