.. _ble_central_sc_general_sample_en:

central_sc_general
###################

Overview
********

Application demonstrating the BLE Central role. Similar to the :ref:`central_general <ble_central_general_sample_en>` sample, except that this
application supporting security feature. After the connection is established, users can input relevant commands in the console to complete the encryption process, such as initiating the pairing operation, setting IO capabilities, and entering the pairing code. 

User interface (cli commands)
******************************

security:
   * Initiate the pairing process

auth_method:
   * Setting security IO capability, it supports display, display and yes/no, input, no-input-no-output, display and input

auth_passkey:
   * Response the passkey

auth_cancel:
   * Cancel the pairing request

auth_passkey_confirm:
   * Confirm the passkey value

auth_pairing_confirm
   * Confirm the pairing request

Requirements
************

* Board A with Bluetooth Low Energy support running this application
* Board B with Bluetooth Low Energy support running :ref:`peripheral_sc_general<ble_peripheral_sc_general_sample_en>` application 

Building and Running
********************

Example Location：``examples/bluetooth/central_sc_general``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (84) bt: LE SC enabled
	[I] (212) bt: No ID address. App must call settings_load()
	[I] (214) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (214) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (214) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (227) sc_central: Bluetooth initialized
	[I] (229) sc_central: Scanning successfully started
	
2. Waiting for development board B to be flashed with the  :ref:`peripheral_sc_general<ble_peripheral_sc_general_sample_en>` example firmware.

3. After development board A scans for and detects the specific advertising information from development board B, it automatically initiates a connection request.

.. code-block:: console

	[I] (18383) sc_central: Device found: 28:6D:CD:68:D7:99 (public) (RSSI -41)
	[I] (18400) sc_central: Updated MTU: TX: 23 RX: 23 bytes
	[I] (18401) sc_central: Connected: 28:6D:CD:68:D7:99 (public)

4. Once the connection is successfully established, the pairing function is automatically executed, prompting for the input of a pairing code. 

.. code-block:: console

	[I] (18902) bt: auth_req:0x0d, init_key:0x05, resp_key:0x07
	[I] (19039) sc_central: Remote pairing features: IO: 0x00, OOB: 0, AUTH: 0x0d, Key: 16, Init Kdist: 0x04, Resp Kdist: 0x04
	[I] (19040) sc_central: LE conn  param req: 28:6D:CD:68:D7:99 (public) int (0x0010, 0x0014) lat 0 to 500
	[I] (19189) sc_central: input passkey for 28:6D:CD:68:D7:99 (public)

5. On the console of development board A, use the ``auth_passkey`` command to input the pairing code **764892** displayed on development board B.

.. code-block:: console

	auth_passkey 764892

6. After successful pairing, the MTU update operation is automatically executed, with the following log output
::

	[W] (36197) bt: chan 0x20004040 conn 0x20003bb8 handle 0 encrypt 0x01 hci status 0x00
	[I] (36197) sc_central: Security changed: 28:6D:CD:68:D7:99 (public) level 4
	[I] (36197) sc_central: mtu_exchange: Current MTU = 23
	[I] (36197) sc_central: mtu_exchange: Exchange MTU...
	[I] (36285) sc_central: BONDED with 28:6D:CD:68:D7:99 (public)
	[I] (36286) sc_central: Updated MTU: TX: 247 RX: 247 bytes
	[I] (36286) sc_central: mtu_exchange_cb: MTU exchange successful (247)
	
7. Next, the example program automatically executes the service list query and subscription functions, with the following log output

.. code-block:: console

	[I] (6400) central: [ATTRIBUTE] uuid 2803, handle 17
	[I] (6401) central: handle 17, prop 0x0a, value_handle 18 
	[I] (6500) central: [ATTRIBUTE] uuid 2800, handle 16
	[I] (6600) central: [ATTRIBUTE] uuid 2803, handle 19
	[I] (6700) central: [ATTRIBUTE] uuid 2902, handle 21
	[I] (6700) central: [SUBSCRIBED]
	
8. Upon successful subscription, board A will receives dummy data sent from board B (the data rate is for demonstration purposes only)

.. code-block:: console

	[I] (37717) sc_central: rx data rate:240 B/s
	[I] (38717) sc_central: rx data rate:4560 B/s
	[I] (39768) sc_central: rx data rate:4080 B/s
	[I] (40817) sc_central: rx data rate:3840 B/s
	[I] (41817) sc_central: rx data rate:4080 B/s
	[I] (42818) sc_central: rx data rate:4320 B/s
	[I] (43842) sc_central: rx data rate:4080 B/s
	
.. note::

   This example need to work together with :ref:`periphal_sc_general example <ble_peripheral_sc_general_sample_en>` 