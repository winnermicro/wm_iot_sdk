.. _bluetooth-dev:

Developing Bluetooth Applications
#########################################

Bluetooth applications are developed using the general architecture and
methods described described in the “Application Development” section of the
documentation.

Additional information related solely to Bluetooth applications can be found on this page.

Thread Safety
****************

Unless otherwise specified in the documentation of the API functions, calling the Bluetooth API is thread-safe.



Initialization
**************

The Bluetooth subsystem is initialized using the :c:func:`bt_enable`
function. The caller should ensure that function succeeds by checking
the return code for errors. If a function pointer is passed to
:c:func:`bt_enable`, the initialization proceeds asynchronously, and the
completion is notified through the given function.

Bluetooth Application Example
********************************

Below is a demonstration of a simple Bluetooth beacon application. This application initializes the Bluetooth subsystem and enables non-connectable advertising.

.. code:: C

    
    #include <bluetooth/bluetooth.h>
	#include <bluetooth/hci.h>

	#ifndef IBEACON_RSSI
	#define IBEACON_RSSI 0xc8
	#endif

	/*
	 * Set iBeacon demo advertisement data. These values are for
	 * demonstration only and must be changed for production environments!
	 *
	 * UUID:  18ee1516-016b-4bec-ad96-bcb96d166e97
	 * Major: 0
	 * Minor: 0
	 * RSSI:  -56 dBm
	 */
	static const struct bt_data ad[] = {
		BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
		BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
				  0x4c, 0x00, /* Apple */
				  0x02, 0x15, /* iBeacon */
				  0x18, 0xee, 0x15, 0x16, /* UUID[15..12] */
				  0x01, 0x6b, /* UUID[11..10] */
				  0x4b, 0xec, /* UUID[9..8] */
				  0xad, 0x96, /* UUID[7..6] */
				  0xbc, 0xb9, 0x6d, 0x16, 0x6e, 0x97, /* UUID[5..0] */
				  0x00, 0x00, /* Major */
				  0x00, 0x00, /* Minor */
				  IBEACON_RSSI) /* Calibrated RSSI @ 1m */
	};

	static void bt_ready(int err)
	{
		if (err) {
			printk("Bluetooth init failed (err %d)\n", err);
			return;
		}

		printk("Bluetooth initialized\n");

		/* Start advertising */
		err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad),
					  NULL, 0);
		if (err) {
			printk("Advertising failed to start (err %d)\n", err);
			return;
		}

		printk("iBeacon started\n");
	}

	int main(void)
	{
		int err;

		printk("Starting iBeacon Demo\n");

		/* Initialize the Bluetooth Subsystem */
		err = bt_enable(bt_ready);
		if (err) {
			printk("Bluetooth init failed (err %d)\n", err);
		}
		return 0;
	}  
  


The key APIs employed by the beacon sample are :c:func:`bt_enable` for initializing Bluetooth, followed by :c:func:`bt_le_adv_start` for starting the advertisement with a specific combination of advertising and scan response data.


Compilation and Download
********************************

Run the ``wm.py build`` command

::

    cd examples/bluetooth/ibeacon
    wm.py build

After successful building, the generated firmware ibeacon.fls will be located in:

::

    examples/bluetooth/ibeacon/build/ibeacon.fls
	
Provide the location or refer to the :ref:`Quick Start Guide<compiling-environment>` section for downloading the firmware.Upon successful downloading, the console will print information as follows:

::

     	I/bt_hci_core     [0.090] Identity: 38:65:BD:12:21:66 (public)
	I/bt_hci_core     [0.096] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.104] LMP: version 4.2 (0x08) subver 0x0709
	Bluetooth initialized
	iBeacon started	



More Application Examples
*****************************

For more application examples, please refer to :ref:`examples/bluetooth<bluetooth_example>`