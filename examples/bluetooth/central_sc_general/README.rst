.. _ble_central_sc_general_sample:

central_sc_general
######################

功能概述
*********

本示例演示了使能 BLE 中央设备角色，和 :ref:`central_general<ble_central_general_sample>` 示例不同的是本示例支持加密流程。
在连接建立后，用户可以在控制台下输入相关指令以完成加密流程，如发起配对操作、设置 IO 能力和输入配对码。

配对指令 (cli commands)
******************************

security:
   * 发起配对流程

auth_method:
   * 设置设备输入输出能力，支持display， display and yes/no， input， no-input-no-output，display and input

auth_passkey:
   * 给对方设备回复配对码

auth_cancel:
   * 取消配对流程

auth_passkey_confirm:
   * 确认配对码正确

auth_pairing_confirm
	* 确认配对流程

环境要求
************

* 支持 BLE 功能的开发板 A，运行本示例
* 支持 BLE 功能的开发板 B，运行 :ref:`peripheral_sc_general<ble_peripheral_sc_general_sample>` 示例

编译和烧录
********************

示例位置：``examples/bluetooth/central_sc_general`` 

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_

运行结果 (开发板 A)
********************

1. 成功运行将输出如下日志

.. code-block:: console

	[I] (84) bt: LE SC enabled
	[I] (212) bt: No ID address. App must call settings_load()
	[I] (214) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (214) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (214) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (227) sc_central: Bluetooth initialized
	[I] (229) sc_central: Scanning successfully started

2. 等待开发板 B 烧录 :ref:`peripheral_sc_general<ble_peripheral_sc_general_sample>` 示例固件

3. 开发板 A 扫描到开发板 B 的特定广播信息后，自动执行连接请求

.. code-block:: console

	[I] (18383) sc_central: Device found: 28:6D:CD:68:D7:99 (public) (RSSI -41)
	[I] (18400) sc_central: Updated MTU: TX: 23 RX: 23 bytes
	[I] (18401) sc_central: Connected: 28:6D:CD:68:D7:99 (public)

4. 连接成功后自动执行配对功能，提示输入配对码

.. code-block:: console

	[I] (18902) bt: auth_req:0x0d, init_key:0x05, resp_key:0x07
	[I] (19039) sc_central: Remote pairing features: IO: 0x00, OOB: 0, AUTH: 0x0d, Key: 16, Init Kdist: 0x04, Resp Kdist: 0x04
	[I] (19040) sc_central: LE conn  param req: 28:6D:CD:68:D7:99 (public) int (0x0010, 0x0014) lat 0 to 500
	[I] (19189) sc_central: input passkey for 28:6D:CD:68:D7:99 (public)

5. 在开发板 A 的控制台使用 ``auth_passkey`` 指令输入开发板 B 上显示的配对码 **764892**

.. code-block:: console

	auth_passkey 764892

6. 配对成功后，自动执行更新 MTU 操作，输出如下日志
::

	[W] (36197) bt: chan 0x20004040 conn 0x20003bb8 handle 0 encrypt 0x01 hci status 0x00
	[I] (36197) sc_central: Security changed: 28:6D:CD:68:D7:99 (public) level 4
	[I] (36197) sc_central: mtu_exchange: Current MTU = 23
	[I] (36197) sc_central: mtu_exchange: Exchange MTU...
	[I] (36285) sc_central: BONDED with 28:6D:CD:68:D7:99 (public)
	[I] (36286) sc_central: Updated MTU: TX: 247 RX: 247 bytes
	[I] (36286) sc_central: mtu_exchange_cb: MTU exchange successful (247)
	
7. 然后，示例程序自动执行查询服务列表和订阅功能，输出如下日志

.. code-block:: console

	[I] (6400) central: [ATTRIBUTE] uuid 2803, handle 17
	[I] (6401) central: handle 17, prop 0x0a, value_handle 18 
	[I] (6500) central: [ATTRIBUTE] uuid 2800, handle 16
	[I] (6600) central: [ATTRIBUTE] uuid 2803, handle 19
	[I] (6700) central: [ATTRIBUTE] uuid 2902, handle 21
	[I] (6700) central: [SUBSCRIBED]
	
8. 订阅成功后，开发板 A 接收到开发板 B 发送的数据，并输出速率信息 ( 本示例数据仅仅用于功能演示 )

.. code-block:: console

	[I] (37717) sc_central: rx data rate:240 B/s
	[I] (38717) sc_central: rx data rate:4560 B/s
	[I] (39768) sc_central: rx data rate:4080 B/s
	[I] (40817) sc_central: rx data rate:3840 B/s
	[I] (41817) sc_central: rx data rate:4080 B/s
	[I] (42818) sc_central: rx data rate:4320 B/s
	[I] (43842) sc_central: rx data rate:4080 B/s
	
.. note::

   本示例需要和 :ref:`外围安全设备示例 <ble_peripheral_sc_general_sample>` 协同工作 