.. _ble_wifi_provision_en:

Bluetooth: Wi-Fi Provision
##########################

Overview
************

This example demonstrates the BLE-assisted rapid connection to a Wi-Fi Access Point (AP) function. After running the example, it starts a custom service and begins broadcasting, waiting for a connection.
On the mobile phone, install and run the `WMBleWiFi <https://www.winnermicro.com/upload/1/editor/1715418917695.rar>`_ networking tool. By scanning, you can discover the **wm-iot-dev** device. After connecting, input the username and password required for the device to connect to the AP. Click on
"Configure", and the example program will connect to the AP and report the connection result.It should be noted that the user information transmitted through BLE is encrypted. For specific details, please refer to :ref:`Wi-Fi Networking Protocol <bluetooth-wifiprov-proto>` 。


Requirements
************

* A board with BLE support
* `WMBleWiFi APP <https://www.winnermicro.com/upload/1/editor/1715418917695.rar>`_ 

Building and Running
********************

Example Location：``examples/bluetooth/wifi_prov``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/latest/get_started/index.html>`_

Running Result
***************

1. Successfully running will output the following logs

.. code-block:: console

	I/bt_hci_core     [0.110] Identity: 28:6D:CE:01:02:03 (public)
	I/bt_hci_core     [0.116] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.126] LMP: version 4.2 (0x08) subver 0x0709
	I/wifi_prov       [0.134] WiFi provision by BLE ready
	I/wifi_prov       [224.184] Connected 5E:C0:7C:15:1F:10 (random)
	I/wifi_prov       [224.190] wps service connected
	D/wifi_prov       [225.168] Received data from: 5E:C0:7C:15:1F:10 (random)
	D/wifi_prov       [225.482] Received data from: 5E:C0:7C:15:1F:10 (random)
	D/HEX SSID:: 0000-000F: 54 50 2D 4C 49 4E 4B 5F  43 30 36 32                TP-LINK_C062
	D/HEX PASS:: 0000-000F: 31 32 33 34 35 36 37 38  39 30                      1234567890
	I/wifi_prov       [225.506] connecting to ap SSID:TP-LINK_C062 password:1234567890 (225504)
	I/wifi_prov       [226.340] wifi_init_sta finished (226340).
	D/wifi            [227.998] distribution event 1
	D/wifi            [228.002] sta connected 64-6E-97-07-C0-62 channel 6 authmode 3
	D/wifi            [231.470] distribution event 0
	D/wifi            [231.474] sta got ip 192.168.1.131
	I/wifi_prov       [231.478] got ip: 192.168.1.131

2. The operation process of WMBleWiFi APP is shown in the following figure

.. figure:: assert/wifi_prov.svg
    :align: center

