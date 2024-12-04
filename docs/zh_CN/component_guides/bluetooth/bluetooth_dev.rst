.. _bluetooth-dev:

开发蓝牙应用程序
################################

蓝牙应用程序是使用文档的 “应用程序开发” 部分中描述的通用基础架构和方法开发的。

在此页面中可以找到仅与蓝牙应用程序相关的其他信息。

线程安全
*************

除非 API 函数的文档中另有说明，否则调用 Bluetooth API 是线程安全的。


初始化
**************

使用 :func:`bt_enable` 函数初始化蓝牙子系统。调用方应通过检查函数返回的错误码来确保函数成功。如果将函数指针传递给 :func:`bt_enable` ，则初始化以异步方式进行，并通过给定的函数通知完成。


蓝牙应用示例
*****************************

下面演示了一个简单的蓝牙信标应用程序。该应用程序初始化蓝牙子系统并启用不可连接的广播。

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



信标示例使用的关键 API 是用于初始化蓝牙的 :func:`bt_enable` ，然后是用于开始播发广告和扫描响应数据的特定组合的 :func:`bt_le_adv_start` 。

编译和下载
*****************************

运行 ``wm.py build`` 命令

::

    cd examples/bluetooth/ibeacon
    wm.py build

构建成功后，生成的固件 ibeacon.fls 位于:

::

    examples/bluetooth/ibeacon/build/ibeacon.fls

下载固件，具体参考 :ref:`快速入门<compiling-environment>` 章节 。下载成功后，console打印信息如下

::

	I/bt_hci_core     [0.090] Identity: 38:65:BD:12:21:66 (public)
	I/bt_hci_core     [0.096] HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	I/bt_hci_core     [0.104] LMP: version 4.2 (0x08) subver 0x0709
	Bluetooth initialized
	iBeacon started


更多应用示例
*****************************
 
更多应用示例请参考 :ref:`examples/bluetooth<bluetooth_example>`