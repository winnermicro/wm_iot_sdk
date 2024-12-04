.. _ble_direct_adv_en:

direct_adv
#############################

Overview
********

Application demonstrating the BLE Direct Advertising capability. If no device is ``BONDED`` 
to the peripheral, casual advertising will be performed. Once ``BOND`` , on every subsequent
boot direct advertising to the bonded central will be performed. Additionally this sample
provides two BLE characteristics. To perform write, devices need to be bonded, while read
can be done just after connection (no bonding required).

.. warning::
   Please note that direct advertising towards iOS based devices is not allowed.
   For more information about designing BLE devices for Apple products refer to "Accessory Design Guidelines for Apple Devices" 

Requirements
************

* A board with BLE support
* Second BLE device acting as a central with enabled privacy. For example another board
  or any modern smartphone

Building and Running
********************

Example Location：``examples/bluetooth/direct_adv``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	I/bt_smp          [0.024] LE SC enabled
	I/bt_hci_core     [0.156] No ID address. App must call settings_load()
	I/direct_adv      [0.162] Bluetooth initialized
	I/bt_hci_core     [0.168] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.172] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.180] LMP: version 4.2 (0x08) subver 0x0709
	I/direct_adv      [0.190] Advertising successfully started
