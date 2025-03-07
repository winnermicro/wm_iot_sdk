.. _bluetooth_bthome_sensor_template_sample_en:

bthome_sensor_template
#################################

Overview
********

Template for a `BTHome <https://bthome.io/>`_ sensor.

Requirements
************

* A board with BLE support
* A BTHome compatible listener, for example `Home Assistant <https://www.home-assistant.io/>`_ with the BTHome integration running

Building and Running
********************

Example Location：``examples/bluetooth/bthome_sensor_template``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	I/bthome_sensor   [0.056] Starting BTHome sensor template
	I/bt_hci_core     [0.082] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.088] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.096] LMP: version 4.2 (0x08) subver 0x0709
	I/bthome_sensor   [0.102] Bluetooth initialized
	
2. Run APP, navigate to Devices & Services under settings in Home
Assistant. There you will be asked to configure the BTHome sensor if everything
went well.
