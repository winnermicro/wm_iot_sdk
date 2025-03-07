.. _bluetooth_central_en:

Central
#########

Overview
********

This example demonstrates the basic functionality of a BLE Central device. Upon running the program, it begins scanning. If a device with a sufficiently strong signal (stronger than -70dBm) is detected, a connection will be established. Immediately after the connection is established, it will be disconnected. Once disconnected, the scanning, connecting, and disconnecting process will be repeated. This cycle will continue indefinitely.



Requirements
************

* A board with BLE support
* Bluetooth device supporting BLE peripheral functionality

Building and Running
********************

Example Location：``examples/bluetooth/central``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 16:45:11
	[I] (25) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (25) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (25) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (25) central: Bluetooth initialized
	[I] (120) central: Device found: 62:A5:CD:FC:CE:BA (random) (RSSI -74)
	[I] (176) central: Device found: 28:6D:CE:11:22:33 (public) (RSSI -36)
	[I] (934) central: Connected: 28:6D:CE:11:22:33 (public)
	[I] (983) central: Disconnected: 28:6D:CE:11:22:33 (public) (reason 0x16)
	[I] (1012) central: Device found: 98:B6:73:27:96:71 (public) (RSSI -99)
	[I] (1012) central: Device found: 60:DE:F4:62:5D:13 (public) (RSSI -83)
	[I] (1012) central: Device found: 68:A1:07:7A:30:93 (random) (RSSI -75)
	[I] (1134) central: Device found: C1:29:46:29:00:D8 (public) (RSSI -95)
