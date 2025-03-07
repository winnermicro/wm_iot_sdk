.. _bluetooth_central_hr_en:

central_hr
#############

Overview
********

Similar to the :ref:`central <bluetooth_central_en>` sample, This example demonstrates the functionality of scanning for and connecting to BLE heart rate monitor peripherals. Once connected successfully, it periodically sends heart rate information to the peripheral device.

Requirements
************

* Board A with BLE support running this application
* A BLE device around acting as a Heart-rate Peripheral or board B with BLE support running  :ref:`peripheral_hr <ble_peripheral_hr_sample_en>` application 

Building and Running
********************

Example Location：``examples/bluetooth/central_hr``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result of Board A 
****************************************************************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 17 2024 17:31:25
	[I] (25) bt: LE SC enabled
	[I] (147) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (147) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (147) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (147) central_hr: Bluetooth initialized
	[I] (149) central_hr: Scanning successfully started
	[I] (963) central_hr: [DEVICE]: 51:0C:84:0F:95:F2 (random), AD evt type 3, AD data len 31, RSSI -75
	[I] (1022) central_hr: [DEVICE]: 61:CB:82:A1:3F:A0 (random), AD evt type 3, AD data len 31, RSSI -73
	
2. Wait for development board B to be flashed with and run the firmware of the :ref:`peripheral_hr <ble_peripheral_hr_sample_en>` example.

3. After board A finds board B, it will connect、query service and subscribe the notification

.. code-block:: console

	[I] (1023) central_hr: [DEVICE]: 28:6D:CE:11:22:33 (public), AD evt type 0, AD data len 11, RSSI -34
	[I] (2536) central_hr: Connected: 28:6D:CE:11:22:33 (public)
	[I] (2628) central_hr: [ATTRIBUTE] handle 25
	[I] (2728) central_hr: [ATTRIBUTE] handle 26
	[I] (2829) central_hr: [ATTRIBUTE] handle 28
	[I] (2829) central_hr: [SUBSCRIBED]
	
4. After successfully subscribing, development board B starts sending heart rate information. Here is the log information from development board A

.. code-block:: console

	[I] (3731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (4781) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (5731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (6731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	[I] (7731) central_hr: [NOTIFICATION] data 0x2000aefc length 2
	
Running result of board B 
****************************************************************

1. Flash the firmware of the :ref:`peripheral_hr <ble_peripheral_hr_sample_en>` example. Upon successful execution, the following log will be output:

.. code-block:: console

	[I] (6) main: ver: 2.0.6dev build at Oct 17 2024 17:31:48
	[I] (28) bt: LE SC enabled
	[I] (158) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (158) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (159) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (159) peripheral_hr: Bluetooth initialized
	[I] (161) peripheral_hr: Advertising successfully started
	
2. Board B receives the connection request from board A will output the following logs

.. code-block:: console

	[I] (1251) peripheral_hr: Connected
	
3. Development board B receives the subscription message from development board A and outputs the following log. It then periodically sends heart rate information to development board A.

.. code-block:: console

	[I] (1692) bt: HRS notifications enabled
	
.. note::

   This example need to work together with :ref:`periphal_hr example <ble_peripheral_hr_sample_en>` 