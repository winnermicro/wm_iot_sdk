.. _bluetooth-feature:


Supported Features
*********************

The WM IoT SDK  integrates a feature-rich and highly configurable Bluetooth protocol stack.

* Bluetooth v5.3 Compatible

  * Highly configurable

    * Features, buffer sizes/count, stack sizes, etc.
 
* Compliant with Bluetooth SIG standards

* Bluetooth Low Energy (BLE) Controller Support

  * Supports master, slave, and simultaneous master and slave roles.
  * Supports all v4.2 specification features.
  * Master can connect up to 6 peripherals.
  * Intelligent role scheduling to minimize overlap.
  * Portable design suitable for any open BLE antenna.
  * Controller can be built over different physical transports (HCI).

* Bluetooth Host Support

  * Generic Access Profile (GAP) with all possible LE roles:

    * Peripheral and central
    * Observer and broadcaster
    * Supports multiple PHYs (2Mbit/s, coded)
    * Extended advertising
    * Periodic advertising (including synchronization transfers)

  * GATT (Generic Attribute Profile)

    * Server (as peripheral role)
    * Client (as central role, to connect to peripherals)
    * Enhanced ATT (EATT)
    * GATT Database Hash
    * GATT Multiple Notifications

  * Pairing support, including secure connections feature of Bluetooth 4.2.

  * Non-volatile storage support for persistently storing Bluetooth-specific settings and data.

  * Bluetooth Mesh Network Support

    * Relay, friend node, low power node (LPN), and GATT proxy functionality.
    * Provisioning roles and bearer (PB-ADV & PB-GATT)
    * Includes foundation models
    * Highly configurable, suitable for devices with as little as 16k RAM.

  * Basic Bluetooth BR/EDR (Classic) Support

    * Generic Access Profile (GAP)
    * Logical Link Control and Adaptation Protocol (L2CAP)
    * Serial Port Emulation (RFCOMM Protocol)
    * Service Discovery Protocol (SDP)
