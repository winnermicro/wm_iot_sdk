.. _ble_central_general_sample_en:

central_general
#################

Overview
********

Application demonstrating the BLE Central role. It scan and connect ble peripheral ( ``BT_UUID_SERVICE`` exists in advertisement field), then discovery and 
subscribe ``BT_UUID_CHAR_INDCIATE`` or ``BT_UUID_CHAR_NOTIFY`` according to ``menuconfig -> Example configuration -> selectable item central example`` choice. It also discovery ``BT_UUID_CHAR_READ_WRITE`` and perform periodic writting.

::

    #define BT_UUID_SERVICE            0xFFF0
    #define BT_UUID_CHAR_READ_WRITE    0xFFF1
    #define BT_UUID_CHAR_INDCIATE      0xFFF2
    #define BT_UUID_CHAR_NOTIFY        0xFFF3

Requirements
************

* Board A with Bluetooth Low Energy support running this application
* Board B with Bluetooth Low Energy support running  :ref:`periphal_general <ble_peripheral_general_sample_en>` application

Building and Running
********************

Example Location：``examples/bluetooth/central_general``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_


Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 17:04:44
	[I] (25) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (25) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (25) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (25) central: Bluetooth initialized
	[I] (27) central: Scanning successfully started 

2. Wait for development board B to be flashed and run the firmware of the :ref:`periphal_general <ble_peripheral_general_sample_en>` example.


3. After board A find the advertisement information of board B, it performs connect、 exchange MTU、discovery service table and subscribe notification

.. code-block:: console

	[I] (5094) central: Device found: 28:6D:CE:11:22:33 (public) (RSSI -59)
	[I] (6153) central: Connected: 28:6D:CE:11:22:33 (public)
	[I] (6153) central: mtu_exchange: Current MTU = 23
	[I] (6153) central: mtu_exchange: Exchange MTU...
	[I] (6250) central: mtu_exchange_cb: MTU exchange successful (247)
	[I] (6400) central: [ATTRIBUTE] uuid 2803, handle 17
	[I] (6401) central: handle 17, prop 0x0a, value_handle 18 
	[I] (6500) central: [ATTRIBUTE] uuid 2800, handle 16
	[I] (6600) central: [ATTRIBUTE] uuid 2803, handle 19
	[I] (6700) central: [ATTRIBUTE] uuid 2902, handle 21
	[I] (6700) central: [SUBSCRIBED]

4. Once subscribe successfully finished, board A will receive data packet sent from board B (the data rate is for demonstration purposes only)

.. code-block:: console

	[I] (22597) central: rx data rate:244 B/s
	[I] (23622) central: rx data rate:4392 B/s
	[I] (24622) central: rx data rate:4392 B/s
	[I] (25722) central: rx data rate:3904 B/s
	[I] (26747) central: rx data rate:4392 B/s
	[I] (27747) central: rx data rate:3904 B/s
	[I] (28747) central: rx data rate:4636 B/s
	[I] (29747) central: rx data rate:4392 B/s

.. note::

   This example need to work together with :ref:`peripheral_general <ble_peripheral_general_sample_en>` 
