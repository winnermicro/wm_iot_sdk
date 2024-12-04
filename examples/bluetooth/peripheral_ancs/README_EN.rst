.. _peripheral_ancs_client_sample_en:

peripheral_ancs
#################################


Overview
********

The Apple Notification Center Service(ANCS) client sample implements an Apple Notification Center Service client.
This client receives iOS notifications and is therefore a Notification Consumer.

- It can be connected with a Notification Provider, typically an iPhone or some other Apple device, which functions as ANCS server.

- When the sample is connected to a Notification Provider, it receives and prints incoming notifications on the CONSOLE.

- Notifications can have positive and negative actions associated with them, depending on the app that is sending the notification.
  For example, a notification for an incoming call is usually associated with the positive action to answer the call and the negative action to reject it.
  After receiving a notification, the available actions are indicated by flags on CONSOLE.

- The sample can perform the positive or negative action on console (cli command) as a response to the notification.

- More about ancs, see: `ANCS Specification <https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Specification/Specification.html>`_

Requirements
************

* A board with Bluetooth Low Energy support

* The sample also requires a device running an ANCS Server to connect with (for example, an iPhone which runs iOS)

.. caution::

   This example only supports working in conjunction with iOS devices.


User interface (cli commands)
*****************************

read_notify:
   * Request current iOS notification attributes (content)

read_notify_next:
   * Request next iOS notification attributes (content)

read_appl:
   * Request iOS app attributes

action_positive:
   * Perform a positive action as a response to the last received notification

action_negative:
   * Perform a negative action as a response to the last received notification

Building and Running
********************

Example Location：``examples/bluetooth/peripheral_ancs``

compile, burn, and more, see：`Quick Start Guide <https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html>`_


Running result
**************

1. Successfully running will output the following logs

.. code-block:: console

	[I] (35) main: ver: 2.0.6dev build at Oct 25 2024 08:33:22
	[I] (61) ancs: Starting Apple Notification Center Service client
	[I] (85) bt: LE SC enabled
	[I] (214) bt: No ID address. App must call settings_load()
	[I] (216) bt: Identity: 28:6D:CE:11:22:33 (public)
	[I] (216) bt: HCI: version 4.2 (0x08) revision 0x0709, manufacturer 0x070c
	[I] (217) bt: LMP: version 4.2 (0x08) subver 0x0709
	[I] (232) ancs: Advertising successfully started

#. Select the device **WM ANCS** in the iOS settings Bluetooth menu and connect
#. Observe that notifications that are displayed in the iOS notification tab also show up on the CONSOLE from the sample
#. Input ``read_notify`` to retrieve the notification attributes and observe that you receive, among other information, the app identifier for the last received notification
   For example, if you got a notification from the Calendar app and request the app identifier, it is "com.apple.mobilecal"
#. Input ``read_appl`` to retrieve the app attributes and observe that you receive the display name for the app identifier from the previous step.
   For example, requesting the app attributes for "com.apple.mobilecal" yields "Calendar"
#. If the notification has a flag for a positive or negative action, Input the notification action with ``action_positive`` or ``action_negative``, respectively.

