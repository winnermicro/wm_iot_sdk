.. _ble_peripheral_general_sample_en:

peripheral_general
##############################

Overview
********

This example demonstrates enabling the BLE (Bluetooth Low Energy) peripheral role. It provides three custom service characteristic values that support read and write functionality, as well as subscription services initiated by a central device.It also shows how to exchange mtu function as peripheral role.

.. csv-table:: Service table
    :header: "description", "UUID", "property and permission"
    :widths: 35, 20, 45

    "Primary service", "0xFFF0", " "
    "Characteristic ", "0xFFF1", "BT_GATT_CHRC_READ|BT_GATT_PERM_READ|BT_GATT_CHRC_WRITE|BT_GATT_PERM_WRITE"
    "Characteristic ", "0xFFF2", "BT_GATT_CHRC_NOTIFY（ ``menuconfig -> Example configuration -> selectable item peripheral example`` ）"
	"Characteristic ", "0xFFF3", "BT_GATT_CHRC_INDICATE（ ``menuconfig -> Example configuration -> selectable item peripheral example`` ）default"

Requirements
************

* Development board B with BLE capability is used to run the firmware for this example.
* nRF Connect app or development board A with BLE capability is used to run the  :ref:`central_general <ble_central_general_sample_en>` example firmware

Building and Running
********************

Example Location：``examples/bluetooth/peripheral_general``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running Result (Using development board A and development board B as examples)
**************************************************************************************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (7) main: ver: 2.0.6dev build at Oct 18 2024 07:59:44
	[I] (29) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (30) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (30) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (32) peripheral: peripheral indicate service ready

2. Wait for development board A to be programmed with the firmware for the :ref:`central_general <ble_central_general_sample_en>` example

3. Development Board B, upon receiving a connection request from Development Board A, establishes a successful connection. It automatically performs MTU updates and connection parameter updates.

.. code-block:: console

	[I] (1318) peripheral: Updated MTU: TX: 23 RX: 23 bytes
	[I] (1319) peripheral: Connected 28:6D:CD:68:D7:99 (public)
	[I] (1366) peripheral: Updated MTU: TX: 247 RX: 247 bytes
	[I] (1965) peripheral: Turn on 2902 indication
	[I] (1966) peripheral: mtu_exchange: Current MTU = 247
	[I] (1966) peripheral: mtu_exchange: Exchange MTU...
	[E] (1966) peripheral: mtu_exchange: MTU exchange failed (err -120)
	[I] (2316) peripheral: LE conn param updated: 28:6D:CD:68:D7:99 (public) int 0x0014 lat 0 to 500


4. Upon receiving a subscription request from Development Board A, Development Board B sends random values to Development Board A using either Indicate or Notify.

5. Development Board B also receives data sent by Development Board A in a write operation

.. code-block:: console

	[I] (2416) peripheral: recv(244):
	[I] (3392) peripheral: recv(244):
	[I] (4392) peripheral: recv(244):
	[I] (5393) peripheral: recv(244):
	[I] (6392) peripheral: recv(244):
	[I] (7441) peripheral: recv(244):
	[I] (8393) peripheral: recv(244):
	[I] (9393) peripheral: recv(244):


Running Result ( nRF Connect APP 和 development board B as examples)
******************************************************************************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (7) main: ver: 2.0.6dev build at Oct 18 2024 07:59:44
	[I] (29) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (30) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (30) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (32) peripheral: peripheral indicate service ready

2. Run the nRF Connect APP, perform a scan, discover the **WM peripheral** device, and establish a connection.

.. figure:: assert/peripheral_general_scan_connect.svg
    :align: center
	
3. Upon receiving a connection request, development board B outputs the following logs.

.. code-block:: console

	[I] (15074) peripheral: Connected 7F:6B:49:51:23:CD (random)
	[I] (15781) peripheral: LE conn param updated: 7F:6B:49:51:23:CD (random) int 0x0006 lat 0 to 500
	[I] (16086) peripheral: LE conn param updated: 7F:6B:49:51:23:CD (random) int 0x0014 lat 0 to 500
	
4. Perform ``write`` and ``subscribe`` on nRF Connect APP after successful connection

.. figure:: assert/peripheral_general_write_subscribe.svg
    :align: center
	
5. Development Board B will output the following logs

.. code-block:: console

	[I] (46618) peripheral: recv(2):
	[I] (48568) peripheral: Turn on 2902 indication
	[I] (48569) peripheral: mtu_exchange: Current MTU = 23
	[I] (48569) peripheral: mtu_exchange: Exchange MTU...
	[I] (48618) peripheral: Updated MTU: TX: 247 RX: 247 bytes
	[I] (48619) peripheral: mtu_exchange_cb: MTU exchange successful (247)