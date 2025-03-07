.. _bluetooth_broadcaster_sample_en:

Broadcaster
#############

Overview
********

This example demonstrates how to utilize the broadcasting function of BLE. After the program is executed, it periodically sends out non-connectable advertisements. The content of these advertisement packets includes the name **WM BROADCASTER** and a manufacturer-specific field. Each time an advertisement is broadcast, the counter in the manufacturer-specific field increments by 1. 
The counter is 1 Byte in size, meaning that once the count reaches 255, it will roll back to 0.


Requirements
************

* A board with BLE support
* nRF Connect APP

Building and Flashing
*********************

Example Location：``examples/bluetooth/broadcaster``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	I/broadcaster     [0.110] Starting Broadcaster
	I/bt_hci_core     [0.134] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.140] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.146] LMP: version 4.2 (0x08) subver 0x0709
	I/broadcaster     [0.152] Bluetooth initialized
	I/broadcaster     [1.156] Sending advertising data: 0x00
	I/broadcaster     [3.162] Sending advertising data: 0x01
	I/broadcaster     [5.168] Sending advertising data: 0x02
	I/broadcaster     [7.174] Sending advertising data: 0x03
	I/broadcaster     [9.180] Sending advertising data: 0x04

2. Run nRF Connect APP to perform a scan,you will find **WM BROADCASTER**

.. figure:: assert/broadcaster.svg
    :align: center 

.. tip::

   This demos send non-connected advertising, the advertising packet can be displayed by refresing scan periodically