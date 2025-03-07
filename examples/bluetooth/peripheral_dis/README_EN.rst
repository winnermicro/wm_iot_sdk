.. _ble_peripheral_dis_en:

peripheral_dis
#########################

Overview
********

Similar to the :ref:`Peripheral <ble_peripheral_sample_en>` sample, except that this
application specifically exposes the DIS (Device Information) GATT Service.


Requirements
************

* A board with BLE support
* nRF Connect APP

Building and Running
********************

Example Location：``examples/bluetooth/peripheral_dis``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_


Running result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (34) main: ver: 2.0.6dev build at Oct 21 2024 17:12:40
	[I] (81) bt: No ID address. App must call settings_load()
	[I] (83) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (83) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (83) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (99) peripheral_dis: Bluetooth initialized
	[I] (102) peripheral_dis: Advertising successfully started

2. Running nRF Connect APP, you will find  **DIS peripheral** and connect it. After successful connection, you can read the device information 

.. figure:: assert/peripheral_dis.svg
    :align: center 
	
3. After a successful connection, the development board will output the following log:

.. code-block:: console

	[I] (31560) peripheral_dis: Connected