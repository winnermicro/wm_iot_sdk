.. _ble_peripheral_accept_list_sample:

peripheral_accept_list
########################

功能概述
*********

本示例演示了 BLE 广播接收名单过滤功能。示例运行后，如果没有和其他设备绑定过，
将执行普通的可连接广播，广播名称为 **Accept list**。

一旦同其他设备绑定成功，就会将该地址加入过滤名单。系统重启后将只接收在过滤名单中的设备发起的连接请求。
同样，扫描响应也只会发送给在过滤名单中的设备。

本示例提供了两个 BLE characteristics, 一个支持写操作需要绑定流程，另外一个支持读操作不需要绑定流程。

.. csv-table:: 服务列表
    :header: "描述", "UUID", "属性和权限"
    :widths: 15, 70, 15

    "Primary service", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef0", " "
    "Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef1", "读和读"
    "Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef2", "写和加密写"
	

环境要求
************

* 支持 BLE 功能的开发板
* nRF Connect APP

编译和烧录
********************

示例位置：``examples/bluetooth/peripheral_accept_list``    

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_


运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	[I] (34) main: ver: 2.0.6dev build at Oct 18 2024 17:29:01
	[I] (83) bt: LE SC enabled
	[I] (213) bt: No ID address. App must call settings_load()
	[I] (213) peripheral_acl: Bluetooth initialized
	[I] (215) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (215) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (215) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (232) peripheral_acl: Advertising successfully started
	
2. 运行 nRF Connect，执行扫描操作，可以发现 **Accept list** 的设备，并建立连接。
   连接成功后向 **0x12345678-0x1234-0x5678-0x1234-0x56789abcdef2** 特征值执行写操作，可触发配对流程

.. figure:: assert/accept_list.svg
    :align: center

3. 配对成功后，示例等待 **5** 秒钟自动重启。输入如下日志
::

	[I] (39111) peripheral_acl: Pairing completed. Rebooting in 5 seconds...

4. 手机 B 运行 nRF Connect APP 可以扫描到 **Accept list** 的设备，执行连接操作会提示连接失败。 再次使用手机 A 运行 nRF Connect APP 可以扫描到 ** Accept list** 的设备，执行连接操作会提示连接成功。
   开发板输出如下日志信息

.. code-block:: console

	[I] (24971) peripheral_acl: Connected with 1C:13:86:59:2D:FD (public)