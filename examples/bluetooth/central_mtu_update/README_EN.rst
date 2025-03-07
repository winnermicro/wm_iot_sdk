.. _ble_central_mtu_update_sample_en:

central_mtu_update
#####################

Q&A:
****

  Question: What do I have to do to update my ATT MTU?

  Answer: To get an ATT MTU of x octets, do the following:

  1，Set ``CONFIG_BT_L2CAP_TX_MTU`` to at least x

  2, Set ``CONFIG_BT_BUF_ACL_RX_SIZE`` to at least x + L2CAP headersize (4 octets)

  3, Set ``CONFIG_BT_BUF_ACL_RX_SIZE`` to at least x + L2CAP header + SDU length field length (6 octets) if using ``CONFIG_BT_EATT`` 

  Ensure the remote side supports the same MTUs.

  Question: I only want to *send* large packets. I don't need to receive large ones.
  Do I still need to set ``CONFIG_BT_BUF_ACL_RX_SIZE`` ?

  Answer: Yes. [#mtu_exchange]_ The Bluetooth specification mandates a symmetric MTU for ATT.

Overview:
*********

This sample demonstrates the exchange of MTU between two devices to allow a
large notification to be sent.Using a larger ATT MTU helps in transmitting and receiving larger data packets, 
thereby improving throughput.

In order to send larger data packets, both the server and the client need to update their MTU and use the smaller value among the two. 
The MTU is a non-negative value, and although different values can be used for sending and receiving by both parties, 
the MTU value on the server side should be greater than or equal to the MTU on the client side.

It is important to note that in the HOST protocol stack, SDUs will not be fragmented unless dynamic channels of L2CAP are used.

.. caution::
    If the data packet sent in the application is larger than the TX MTU value, it needs to be manually packetized and sent in multiple parts.


.. figure:: assert/mtu.svg
     :align: center
     :alt: Diagram of the MTUs and their corresponding Kconfig symbols

     Diagram of the MTUs and their corresponding Kconfig symbols


Building and Running
********************

Example Location：``examples/bluetooth/central_mtu_update``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_


Running Result
**************

Central:

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


References
**********

.. [#mtu_exchange] Bluetooth Core Specification v. 5.3: Vol. 3, Part F, 3.4.2
.. [#mtu] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 5.1
.. [#sud_encapsulation] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 7.3

.. note::

   This example need to work together with :ref:`periphal_mtu_update <ble_peripheral_mtu_update_sample_en>` 
