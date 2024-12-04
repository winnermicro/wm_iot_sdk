.. _ble_peripheral_sample_en:

peripheral
#####################

Overview
********

Application demonstrating the BLE Peripheral role. It has several well-known and
vendor-specific GATT services that it exposes.

.. csv-table:: 1 Standard GATT Service
    :header: "description", "UUID", "NOTE"
    :widths: 15, 70, 15

    "Battery service", "0x180F", "Periodically report the battery level"
    "Current Time service", "0x1805", " "
    "Device Information", "0x180A", "Read Device Information"
	"Heart Rate", "0x180D", "Periodically report the heart rate"

.. csv-table:: 2 vendor-specific GATT services
    :header: "description", "UUID", "property and permission"
    :widths: 15, 70, 15

    "Primary service", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef0", " "
    "Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef1", "Read | Write | INDICATE, Encrypted Read | Encrypted Write"
    "Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef2", "Read | Write， Authenticated Read | Authenticated Write"
    "Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef3", "Read | Write | Extended, Read | Write | Prepare Write"
	"Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef4", "Read | Write | Authenticated ，Read | Write "
	"Characteristic ", "0x12345678-0x1234-0x5678-0x1234-0x56789abcdef5", "Write Without Response ， Write "

Requirements
************

* Board A with BLE support running this application
* nRF Connect APP

Building and Running
********************

Example Location：``examples/bluetooth/peripheral``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running Result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (84) bt: LE SC enabled
	[I] (217) bt: No ID address. App must call settings_load()
	[I] (217) peripheral: Bluetooth initialized
	[I] (233) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (234) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (234) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (280) peripheral: Advertising successfully started
	[I] (281) peripheral: Indicate VND attr 0x803dea8 (UUID 12345678-1234-5678-1234-56789abcdef1)
	
2. Run the nRF Connect APP, perform a scanning operation, and you will be able to discover devices labeled as **WM Peripheral** . Execute the connection operation, and once successfully connected, subscribe to the Battery Service and Heart Rate Service. On the APP, you will be able to view the reported battery level and heart rate values.

.. figure:: assert/peripheral_scan_connect_info.svg
    :align: center
	
3. Log output on board A after connection

.. code-block:: console

	[I] (11454) peripheral: Connected
	
4. When performing a write operation to the characteristic value **0x12345678-0x1234-0x5678-0x1234-0x56789abcdef1** in the nRF Connect APP, it will trigger the encryption process, and the APP will prompt a pairing request.

.. figure:: assert/peripheral_write_pairing_en.svg
    :align: center
	
5. After the development board receives the pairing request, it will output the pairing code **867435**

.. code-block:: console

	[I] (80939) peripheral: Passkey for 60:58:28:93:AC:16 (random): 867435
	
6. At this point, enter the pairing code 867435 in the nRF Connect APP and confirm. The APP will then indicate that the pairing is successful.

.. figure:: assert/peripheral_paired_write_cmplt.svg
    :align: center