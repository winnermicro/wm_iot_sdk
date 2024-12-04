.. _ble_peripheral_mtu_update_sample_en:

peripheral_mtu_update
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
large notification to be sent.
Updating the MTU can be useful to send bigger packets and so have a better
throughput.

To be able to send a large notification both the server and the client need to
update their MTU. The MTU is not a negotiated value, the client and the server
will exchange their MTUs and choose the minimum of the two. Thus the two MTU can
be set to a different value, but the MTU of the server must be greater or equal
to the MTU of the client.

It's important to note that in the HOST protocol stack, SDUs (Service Data Units) are not fragmented unless L2CAP dynamic channels are used. 
If, in the APPLICATION, the data packet being sent is larger than the TX MTU (Maximum Transmission Unit) value, it is necessary to manually fragment the packet into smaller parts for transmission.

.. figure:: assert/mtu.svg
     :align: center
     :alt: Diagram of the MTUs and their corresponding Kconfig symbols

     Diagram of the MTUs and their corresponding Kconfig symbols


Building and Running
********************

Example Location：``examples/bluetooth/peripheral_mtu_update``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_


Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	I/bt_hci_core     [0.058] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.064] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.072] LMP: version 4.2 (0x08) subver 0x0709
	I/perip_mtu_update [0.710] Updated MTU: TX: 23 RX: 23 bytes
	I/perip_mtu_update [14.494] Updated MTU: TX: 23 RX: 23 bytes


References
**********

.. [#mtu_exchange] Bluetooth Core Specification v. 5.3: Vol. 3, Part F, 3.4.2
.. [#mtu] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 5.1
.. [#sud_encapsulation] Bluetooth Core Specification v. 5.3: Vol. 3, Part A, 7.3

.. note::

   This example need to work together with :ref:`central_mtu_update <ble_central_mtu_update_sample_en>` 
