.. _ble_peripheral_csc_sample_en:

peripheral_csc
#########################

Overview
********

Similar to the :ref:`peripheral <ble_peripheral_sample_en>` sample, except that this
application specifically exposes the CSC (Cycling Speed and Cadence) GATT Service.


Requirements
************

* A board with BLE support
* nRF Connect APP or nRF Toolbox APP

Building and Running
********************

Example Location：``examples/bluetooth/peripheral_csc``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (34) main: ver: 2.0.6dev build at Oct 21 2024 15:10:17
	[I] (81) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (82) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (82) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (82) peripheral_csc: Bluetooth initialized
	[I] (84) peripheral_csc: Advertising successfully started

2. Running nRF Toolbox APP, you will find  **CSC peripheral** and connect it. After successful connection, **SPEED** and **CADENCE** will be displayed on APP 

.. figure:: assert/peripheral_csc.svg
    :align: center 
	
3. After a successful connection, the development board will output the following log:

.. code-block:: console

	[I] (1212633) peripheral_csc: Connected   