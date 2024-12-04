.. _ble_peripheral_identity_sample_en:

Bluetooth: Peripheral Identity
##############################

Overview
********

This sample demonstrates use of multiple identity and the ability to be
connected to from multiple central devices.

.. caution::

   Due to the limited Bluetooth function of the W80X (when doing slave devices, only one central device initiates the connection operation), this example only uses the Identity address when running
   
Requirements
************

* A board with BLE support
* nRF Connect APP

Building and Running
********************

Example Location：``examples/bluetooth/peripheral_identity``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (24) main: ver: 2.0.6dev build at Oct 24 2024 13:27:02
	[I] (74) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (74) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (74) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (74) identity: Bluetooth initialized
	[I] (74) identity: Using current id: 0
	[I] (77) identity: Advertising successfully started
	
2. Run the nRF Connect APP, perform a scanning operation, and after discovering a device with **Peripheral Identity** execute the connection operation.

.. figure:: assert/peripheral_identity.svg
    :align: center

3. The development board outputs the following logs

.. code-block:: console

	[I] (24448) identity: Connected (1): 52:B9:39:43:87:B0 (random)
	
4. After successful connection, the application will disconnect with nRF Connect APP and continue to broadcast and so on

.. code-block:: console

	[I] (24449) identity: Iterations remaining: 0
	[I] (24449) identity: Wait for disconnections...
	[I] (25141) identity: LE conn param updated: 52:B9:39:43:87:B0 (random) int 0x0006 lat 0 to 500
	[I] (25219) identity: Disconnected 52:B9:39:43:87:B0 (random) (reason 0x13)
	[I] (25220) identity: Using current id: 0
	[I] (25222) identity: Advertising successfully started
