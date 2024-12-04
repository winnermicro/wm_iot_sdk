.. _bluetooth_central_multilink:

central_multilink
###############################

功能概述
*********

本示例演示了 BLE 作为中央设备时，连接多个外围设备的功能。示例运行后，开启扫描功能。如果扫描到的外围设备的信号强度大于（-70dBm）就发起连接操作，连接成功后继续扫描并再次发起连接请求。
直至连接外围设备设备的个数到达 ``CONFIG_BT_MAX_CONN`` 后，依次断开上述连接。然后重复执行上述操作，重复次数到达 ``CONFIG_SAMPLE_CONN_ITERATIONS`` 后程序退出。

- ``CONFIG_BT_MAX_CONN`` 配置可以同时连接外围设备的个数，设置路径 ``menuconfig -> Components configuration -> Bluetooth -> Maximum number of simultaneous connections``

- ``CONFIG_SAMPLE_CONN_ITERATIONS`` 配置重复连接的次数，设置路径 ``menuconfig -> Connection iterations``

环境要求
************

* 支持 BLE 功能的开发板
* 多个运行 BLE 外设功能的设备

编译和烧录
********************

示例位置：``examples/bluetooth/central_multilink``   

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html>`_

运行结果
************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bt_hci_core     [0.078] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.084] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.092] LMP: version 4.2 (0x08) subver 0x0709
	I/multilink       [0.098] Bluetooth initialized, CONFIG_BT_MAX_CONN is 3 , loop is 10
	I/multilink       [0.108] Scanning successfully started
