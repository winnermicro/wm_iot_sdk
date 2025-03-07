.. _ble_scan_adv_sample_en:

scan_adv
###########################

Overview
********

This example demonstrates how to enable both **non-connectable advertising** and scanning functions of BLE simultaneously.


Requirements
************

* A board with BLE support
* nRF Connect APP

Building and Running
********************

Example Location：``examples/bluetooth/scan_adv``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 24 2024 08:04:19
	[I] (3) scan_adv: Starting Scanner/Advertiser Demo
	[I] (22) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (23) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (23) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (23) scan_adv: Bluetooth initialized

2. Run nRF Connect APP, perform ``scan`` and you will find **WM BROADCASTER** .Check the  ``Manufacturer data`` field in the advertisement information. The last byte represents the number of times the scan function has received the advertisement message.

.. figure:: assert/scan_adv.svg
    :align: center
	
.. tip::

   In this example, the advertising is not connected, and the dynamic advertising data can be viewed by refreshing the scan results on the APP side	
