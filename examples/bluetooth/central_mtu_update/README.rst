.. _ble_central_mtu_update_sample:

central_mtu_update
#####################

问答:
********

  问：想更新 ATT 的 MTU , 需要怎么操作？
  
  答：想把 ATT MTU 更新为 X 字节，需要以下步骤：
  
  1， 设置 ``CONFIG_BT_L2CAP_TX_MTU`` 最小值为 x
  
  2， 设置 ``CONFIG_BT_BUF_ACL_RX_SIZE`` 最小值为 x + L2CAP header(4字节)
  
  3， 设置 ``CONFIG_BT_BUF_ACL_RX_SIZE`` 最小值为 X + L2CAP header + SDU length 字段的长度 (6 字节) 如果配置了 ``CONFIG_BT_EATT``
	  
  同时还要确认对方设备支持相同的 MTU 值。
  

  问：仅仅需要发送大数据包，不需要接收大数据包。还有必要设置 ``CONFIG_BT_BUF_ACL_RX_SIZE``?
  
  答：是的，The Bluetooth specification [#mtu_exchange]_ 强制使用对等的 ATT MTU

功能概述
*********

本示例演示了两个 BLE 设备之间如何交换 ATT MTU 功能。使用较大的 ATT MTU 有助于
收发大的数据包，以此来提高吞吐率。

为了能够发送大的数据包，server 和 client 需要更新各自的 MTU 并选择双方中的较小值来使用。
MTU 是一个非负值，尽管双方的收发可以使用不同的值，但是作为 server 端的 MTU 值应该大于
或者等于 client 端的 MTU。

需要注意的是，在 HOST 协议栈中，除非使用 L2CAP 动态通道，否则 SDUS 是不会被分片的。


.. caution::

   在应用程序中如果发送的数据包大于 TX 的 MTU 值，需要自行分包发送。


.. figure:: assert/mtu.svg
     :align: center
     :alt: Diagram of the MTUs and their corresponding Kconfig symbols

     Diagram of the MTUs and their corresponding Kconfig symbols

环境要求
**************

* 支持 BLE 功能的开发板 

编译和烧录
********************

示例位置：``examples/bluetooth/central_mtu_update``    

编译、烧录等操作请参考：`快速入门 <https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html>`_

运行结果 
********************

1. 成功运行将输出如下日志

.. code-block:: console

	I/bt_hci_core     [0.064] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.072] LMP: version 4.2 (0x08) subver 0x0709
	I/central_mtu_update [0.078] Bluetooth initialized
	I/central_mtu_update [0.086] Scanning successfully started
	I/central_mtu_update [2.080] Connected: 4A:C6:42:30:10:56 (random)
	I/central_mtu_update [2.086] mtu_exchange: Current MTU = 23
	I/central_mtu_update [2.092] mtu_exchange: Exchange MTU...
	I/central_mtu_update [2.472] mtu_exchange_cb: MTU exchange successful (247)
	I/central_mtu_update [2.572] Discover complete



参考文档
**********

.. [#mtu_exchange] Bluetooth Core Specification v. 5.3: Vol. 3, Part F, 3.4.2
.. [#mtu] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 5.1
.. [#sud_encapsulation] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 7.3

.. note::

   本示例需要和 :ref:`外围设备 MTU 更新示例 <ble_peripheral_mtu_update_sample>` 协同工作 