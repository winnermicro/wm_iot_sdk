.. _bluetooth_central_multilink_en:

central_multilink
###############################

Overview
********

This example demonstrates the functionality of BLE as a central device to connect to multiple peripheral devices. 
After the example is run, the scanning function is activated. If the signal strength of a scanned peripheral device is greater than -70dBm, a connection attempt is initiated. Once the connection is successful, 
scanning continues and another connection request is made.
This process repeats until the number of connected peripheral devices reaches ``CONFIG_BT_MAX_CONN`` , at which point the connections are sequentially disconnected. The operations are then repeated, and the program exits after the number of repetitions reaches ``CONFIG_SAMPLE_CONN_ITERATIONS`` .

- ``CONFIG_BT_MAX_CONN`` configures the maximum number of peripheral devices that can be connected simultaneously. The setting path is ``menuconfig -> Components configuration -> Bluetooth -> Maximum number of simultaneous connections``

- ``CONFIG_SAMPLE_CONN_ITERATIONS`` configures the number of iterations for repeated connections. The setting path is ``menuconfig -> Connection iterations``

Requirements
************

* A board with BLE support
* Multiple devices running BLE peripheral functionality

Building and Running
********************

Example Location：``examples/bluetooth/central_multilink``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	I/bt_hci_core     [0.078] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.084] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.092] LMP: version 4.2 (0x08) subver 0x0709
	I/multilink       [0.098] Bluetooth initialized, CONFIG_BT_MAX_CONN is 3 , loop is 10
	I/multilink       [0.108] Scanning successfully started