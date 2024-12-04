.. _ble_scan_sample_en:

central_scan_only
###########################

Overview
********

A simple application demonstrating BLE Observer role functionality.The format for printing scan information is shown in the following table.

.. list-table:: format of scan result 
    :widths: 25 15 15 60
    :header-rows: 1

    * - Address
      - Signal Strength
      - Broadcast Type
      - Raw Broadcast Data
    * - 35:2A:3F:D6:32:60 (random)
      - -70
      - 0
      - 02 01 1a 02 0a 0c 0c ff  4c 00 10 07 72 1f e4 d3  45 18 58 	  

defintions of advertisement type
::

    #define ADV_TYPE_IND                0
    #define ADV_TYPE_DIRECT_IND_HIGH    1
    #define ADV_TYPE_SCAN_IND           2
    #define ADV_TYPE_NONCONNN_IND       3
    #define ADV_TYPE_DIRECT_IND_LOW     4

Requirements
************

* A board with BLE support

Building and Running
********************

Example Location：``examples/bluetooth/central_scan_only``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_

Running result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (2) main: ver: 2.0.6dev build at Oct 18 2024 16:33:29
	[I] (3) scan_adv: Starting Scanner Demo
	[I] (23) bt: Identity: 28:6D:CD:68:D7:99 (public)
	[I] (23) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (23) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (23) scan_adv: Bluetooth initialized
	[I] (54) scan_adv: dump [addr:35:2A:3F:D6:32:60 (random), rssi -88, adv_type 3, raw data:]
	0x0000   1e ff 06 00 01 0f 20 22  f0 83 fb 7d 6d 79 d2 52  56 5d cd 5f 48 1d 0a ef  0f 56 0b e5 77 b3 9d     ...... "...}my.RV]._H....V..w..
	[I] (64) scan_adv: dump [addr:1C:E4:91:9A:92:0D (random), rssi -87, adv_type 3, raw data:]
	0x0000   1e ff 06 00 01 09 20 22  fc 30 e8 1b 35 a5 06 af  9e 6d e8 0c 49 11 9a 98  e3 be ca e2 d2 78 5b     ...... ".0..5....m..I........x[
	[I] (104) scan_adv: dump [addr:58:71:68:0A:66:28 (random), rssi -88, adv_type 3, raw data:]
	0x0000   1e ff 06 00 01 09 20 02  4c 50 1a f9 a2 fe 32 58  9e 74 2e 6e 7e 06 14 5a  14 2b 86 57 6a a8 5e     ...... .LP....2X.t.n~..Z.+.Wj.^
	[I] (114) scan_adv: dump [addr:7E:EB:AE:E6:A2:05 (random), rssi -91, adv_type 0, raw data:]
	0x0000   02 01 1a 1b ff 4c 00 0c  0e 00 c3 35 df fa 04 a3  37 5e 06 ff be f6 e7 10  06 7e 1d 3e ee cb 88     .....L.....5....7^.......~.>...
	[I] (115) scan_adv: dump [addr:1A:DD:62:03:D2:74 (random), rssi -77, adv_type 3, raw data:]
	0x0000   1e ff 06 00 01 09 20 22  d3 79 62 87 b4 0d 77 d1  34 bd 09 e6 7c 9f e7 b3  00 00 00 00 85 31 d8     ...... ".yb...w.4...|........1.
	[I] (124) scan_adv: dump [addr:4D:9B:F1:17:ED:FF (random), rssi -73, adv_type 3, raw data:]
	0x0000   1e ff 06 00 01 09 20 02  c3 1d fc 5e 1a d8 9e b8  22 13 5e 00 a4 9b bf 5e  ee 65 bd f8 33 ef 10     ...... ....^....".^....^.e..3..
	[I] (134) scan_adv: dump [addr:48:48:1F:9D:68:47 (random), rssi -85, adv_type 0, raw data:]
	0x0000   02 01 1a 02 0a 0c 0c ff  4c 00 10 07 72 1f e4 d3  45 18 58                                          ........L...r...E.X
	[I] (134) scan_adv: dump [addr:08:F0:B0:C6:84:98 (public), rssi -83, adv_type 0, raw data:]
	0x0000   02 01 06 0b ff e0 07 08  f0 b6 c6 84 98 02 10 03  03 01 16 0a 08 45 44 49  46 49 45 52 20 42        .....................EDIFIER B
	[I] (146) scan_adv: dump [addr:10:D2:19:11:31:CE (random), rssi -59, adv_type 4, raw data:]
	0x0000   06 09 57 4d 2d 42 54                                                                                ..WM-BT