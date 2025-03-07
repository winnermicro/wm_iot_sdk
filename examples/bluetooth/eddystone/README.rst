.. _bluetooth_eddystone_sample:

eddystone
####################

功能概述
*********

本示例演示了 `Eddystone Configuration Service`_ 功能。
Eddystone 配置服务在信标上作为 GATT 服务运行，它是可连接的，
并允许配置广播数据、广播功率水平和广播间隔。它还包含了
如何配置和注册 Eddystone-EID 信标的定义。


环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect APP

编译和烧录
********************

示例位置：``examples/bluetooth/eddystone``   

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_


运行结果
************

1. 成功运行将输出如下日志，示例发送可连接广播。广播 ADV Slot Data 默认为 URL “http://www.winnermicro.com”

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 18 2024 10:12:41
	[I] (26) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (26) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (26) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (26) eddystone: Bluetooth initialized
	[I] (29) eddystone: Initial advertising as 28:6D:CD:68:D7:99 (public)
	[I] (29) eddystone: Configuration mode: waiting connections...
	
.. note::

   此时的可连接广播持续 **30** 秒。**30** 秒后，设备自动转换为不可连接的广播

2. 运行 nRF Connect APP, 执行扫描功能可以发现 **WM Eddystone** 的设备

.. figure:: assert/eddystone_adv.svg
    :align: center
	
3. 在 nRF Connect APP 执行连接操作，连接成功后，开发板输出如下日志
::

	[I] (18493) eddystone: Connected

4. 连接成功后，对 Eddystone Configuration Service 进行读写操作。以更改 ADV Slot Data 为例

.. figure:: assert/eddystone_service.svg
    :align: center

5. 配置成功后，在 nRF Connect APP 断开连接，开发板输出如下日志
::

	[I] (101308) eddystone: Disconnected (reason 0x13)
	[I] (101310) eddystone: Switching to Beacon mode 0.
	[I] (101318) eddystone: Advertising as 28:6D:CD:68:D7:99 (public)

6. 再次在 nRF Connect APP 执行扫描操作

.. warning::
   特别注意此时的广播内容不包含名称字段，请根据设备的地址来区分是否为当前设备发送的广播

.. figure:: assert/eddystone_nadv.svg
    :align: center

.. _Eddystone Configuration Service: https://github.com/google/eddystone/tree/master/configuration-service
