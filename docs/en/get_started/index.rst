**********************
Quick Start Guide
**********************

This document aims to guide users in setting up a software environment for hardware development, demonstrating through a simple example how to configure menus using the WM IoT SDK, as well as the steps for compiling and downloading firmware to the development board.

Overview
============

The WM IoT SDK supports the following functions for SoC chips:

.. only:: w800

    * 2.4 GHz Wi-Fi
    * Bluetooth Low Energy
    * High-performance 32-bit CK804 Single-Core Processor
    * Various Peripherals
    * Built-in Security Hardware

The SoC adopts a 40 nm process technology, offering optimal power consumption, RF performance, stability, versatility, and reliability. It is suitable for various application scenarios and different power consumption needs.

WinnerMicro provides users with complete software and hardware resources for the development of hardware devices. Among them, the WM IoT SDK is designed to assist users in rapidly developing IoT applications, meeting their requirements for Wi-Fi, Bluetooth, low power consumption, and other aspects.

Preparation
=============

Hardware
--------------

* A development board
* USB cable
* Computer (Windows, Linux, or macOS)

.. only:: w800

    Below are the W800 official development boards. Click on the links to learn more about the hardware information.

    .. toctree::
       :maxdepth: 1

       W800-Arduino <w800_arduino>

       W803-Pico <w803_pico>

       HLK-W800 <hlk_w800>

	   HLK-W806 <hlk_w806>



Software
---------

To use the WM IoT SDK, please install the following software:

    * Set up a compilation environment for compiling firmware code;
    * Obtain the source code of the `WM IoT SDK <https://doc.winnermicro.net/download/version>`_ software development framework.

.. _compiling-environment:

Set Up the Compilation Environment
==========================================

**Currently, two compilation environments are available:：**

- Command Line Compilation ( :ref:`SHELL<shell-compiling-environment>` Compilation)
- Integrated Tool Compilation(:ref:`IDE<ide>` Compilation)

.. hint::

    Since different users may use different operating systems, we have provided steps for setting up the environments for several commonly used operating systems
    (:ref:`Windows system<windows-system>`、:ref:`Linux system<linux-system>` and :ref:`macOS system<macos-system>`).Users can choose one according to their actual situation.


.. _shell-compiling-environment:

Set up a Command-Line Compilation Environment
-----------------------------------------------

.. toctree::
   :maxdepth: 1

   windows
   linux
   macos

Build a Compilation Environment Using IDE
-----------------------------------------------

.. toctree::
   :maxdepth: 1

   ide

.. _bulid-function:

Compile the First Project
====================================

**Select a Project**

If you have already installed the WM IoT SDK, please switch to the project directory in the command prompt, for example,

::

    cd examples
    cd hello_world

Note that this is an example of operation without using an Integrated Development Environment (IDE). The path for WM IoT SDK and the project should not contain spaces or other special characters.

The first time you use the project, please check if there is already a build folder in the project directory.If it exists, delete it before compiling, otherwise, the compilation may fail due to the existing build information in the build folder.For projects copied from other places, please be sure to check this.


**Set Chip Type**

::

    wm.py set-soc <type>

The default chip type is currently W800, and if you do not perform the setting of the chip type, the configuration for W800 will be used by default.

You can execute ``wm.py set-soc --list-socs`` to view all the supported chip types currently. You can also select the chip type in the ``SOC configuration`` menu of ``menuconfig`` .

Note that this operation will clear and initialize the project's previous compilation and configuration.


**Configure the Project**

::

    wm.py menuconfig

All configurations in the project have default values, so you can skip the project configuration step without affecting subsequent compilation and use.


**Compile the Project**

::

	wm.py build


**Flash to the Device**

Run the following command to flash, where PORT is the serial port to which the development board is currently connected.

::

    wm.py flash -p PORT

Replace PORT with the actual serial port name in your system (for example, COM1 on Windows, /dev/ttyUSB0 on Linux, or /dev/cu.usbserial-1401 on macOS).
If you omit the -p option, wm.py flash will try to use the first available serial port for flashing. If you want to erase the entire flash and then flash again, please run ``wm.py flash -e -p PORT`` .
By default, the device will automatically restart after flashing. If you want to prevent the device from automatically restarting after flashing, please run ``wm.py flash -m -p PORT`` .

If you want to use a graphical flashing tool, please choose to use the `Upgrade Tools <http://isme.fun/?log=blog&id=34>`_ to flash the firmware to the device.
The compiled firmware is in the `build` folder of the project directory. For more information on the firmware, please refer to :ref:`Firmware Format<firmware_format>`.


**Observe Serial Port Runtime Information**

You can use the command ``wm.py monitor -p PORT`` to display real-time serial port output of the project runtime information.
Note, do not forget to replace PORT with your serial port name, the serial port name rules are the same as described in the previous section `<Flash to the Device>`.

``wm.py monitor`` only provides the most basic serial port information display function. After starting, you can use the shortcut key ``Ctrl+C`` to terminate the capture of device serial port information.

Of course, you can also choose other serial port tools to observe the device output information. WM IoT SDK does not limit the serial port tools used.


.. tip::
    You can also run the following command to perform compilation, flashing, and monitor serial port information all at once:

    ``wm.py flash monitor -p PORT``



Uninstall the Compilation Environment
=====================================================

If you are a Windows user and have set up the compilation environment using the ``WM IoT SDK integrated package``, you can in  ``Settings - Apps - features``（or ``Control Panel - Programs - Uninstall a Program``）run the uninstall wizard to uninstall the WM IoT SDK compilation environment.

If you have set up the compilation environment using ``VS Code`` , you can select the installed WM-IDE plugin in the plugin list on the left side of VS Code. There will be an uninstall option on the page that opens, click to uninstall.