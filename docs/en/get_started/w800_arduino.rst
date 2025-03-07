
W800-Arduino Getting Started Guide
=======================================


Preparation
--------------

- W800-Arduino Development Board

- Micro USB Cable or Type-C Cable

- USB Serial Driver （`Click to download the CH34X driver <https://doc.winnermicro.net/download/tools/all/CH341SER.zip>`_）

- PC (Windows, or Linux)

You can skip the introduction and proceed directly to the :ref:`Application development<application-program>` section.

Overview
--------------------------

W800-Arduino is a development board from WinnerMicro based on the W800 chip.

.. attention::
    There are currently two different hardware versions of the development board: v4 and v3. When using them, please observe the silkscreen marking on the board to distinguish between them. 
    The following introduction will be based on the W800-Arduino development board with hardware version v3 as an example.

W800-Arduino development board integrates the following peripherals:

- External Antenna Interface and Onboard Antenna
- Integrated 5V to 3.3V LDO chip
- Provided with BOOT, RESET, and USER Buttons
- Built-in Trichromatic LED Light

This significantly reduces the number and cost of additional components that users need to purchase and install, as well as the complexity of extra assembly and testing, while increasing usability.

The USER button on the W800-Arduino is an input signal detection button provided for customers, facilitating the debugging of related functional programs.

To make it easier for customers to connect more peripherals, all IO signals and system power pins on the W800-Arduino are routed out through the pins on the bottom and top of the board. This allows customers to directly plug the W800-Arduino onto other development boards or use jumper wires to connect it to other peripherals, greatly enhancing the diversity of user applications.

Functional Description
--------------------------

The main components, interfaces, and control methods of the W800-Arduino development board are as follows.

.. figure:: ../../_static/get_started/hardware/w800_arduino.png
    :align: center
    :alt: W800-Arduino Development Board Layout

The main components of the W800-Arduino development board are described in the table below (clockwise from the top left corner).

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Main Component
     - Basic Description

   * - RST-Key
     - System reset button.

   * - USER-Key
     - Input signal detection button provided for customers

   * - BOOT-Key
     - Download button. The user presses the BOOT button, then connects the USB cable to the UART0 port. After releasing the BOOT button, the system enters download mode."

   * - Micro-USB1
     - This is an additional USB port on the development board that functions similarly to the default USB port.

   * - Micro-USB0
     - The default USB port of the development board can be used as a power supply port for the development board or and a communication interface for connecting the PC and the development board.

   * - I/O
     - All pins of the W800 are led out to the pin headers on the development board. Users can program the W800 to achieve functions such as PWM, ADC, DAC, I2C, and I2S. For details, see :ref:`Pin Descriptions <base-pin>`.

   * - LED
     - Three-color LED for user debugging of PWM. You need to switch the K1 toggle to the ON position when using it, and to the OFF position when not in use.

   * - Antenna
     - The onboard antenna of the development board also supports external antennas.

   * - W800
     - The main control chip of the development board features WiFi, Bluetooth, and other functionalities.

   * - LDO
     - LDO chip for converting 5V to 3.3V.

Power Options
--------------------------

The development board can use any of the following three methods:

- Powered by Micro USB (default), both USB ports can provide power.
- Power supply via 5V / GND pins.
- Power supply via 3.3V / GND pins.

.. warning:: 

    The above power supply modes **cannot be connected simultaneously**, otherwise the development board or power supply may be damaged.

.. _base-pin:

Pin Descriptions
--------------------------

The table below introduces the **name** and **functions** of the I/O pins on the development board.  For the specific layout, see the :ref:`Development Board Schematic and Layout Diagram<schematic-diagram>` section.

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Number.
     - Name
     - Type
     - Pin Function After Reset
     - Multiplexing Function
     - Maximum Frequency
     - Pull-up/Pull-down Capability
     - Drive Capability

   * - 1
     - PB_20
     - I/O
     - UART_RX
     - UART0_RX/PWM1/UART1_CTS/I²C_SCL
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 2
     - PB_19
     - I/O
     - UART_TX
     - UART0_TX/PWM0/UART1_RTS/I²C_SDA
     - 10MHz
     - UP/DOWN
     - 12mA

   * - 3
     - WAKEUP
     - I
     - WAKEUP wake-up function
     - 
     - 
     - DOWN
     - 

   * - 4
     - RESET
     - I
     - RESET reset
     - 
     - 
     - UP
     - 

   * - 5
     - XTAL_OUT
     - O
     - External crystal oscillator output
     - 
     - 
     - 
     - 

   * - 6
     - XTAL_IN
     - I
     - External crystal oscillator input
     - 
     - 
     - 
     - 

   * - 7
     - AVDD33
     - P
     - Chip power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 8
     - ANT
     - I/O
     - RF antenna
     - 
     - 
     - 
     - 

   * - 9
     - AVDD33
     - P
     - Chip power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 10
     - AVDD33
     - P
     - Chip power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 11
     - AVDD33_AUX
     - P
     - Chip power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 12
     - TEST
     - I
     - Test function configuration pin
     - 
     - 
     - 
     -

   * - 13
     - BOOTMODE
     - I/O
     - BOOTMODE
     - I²S_MCLK/LSPI_CS/PWM2/I²S_DO
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 14
     - PA_1
     - I/O
     - JTAG_CK
     - JTAG_CK/I²C_SCL/PWM3/I²S_LRCK/ADC0
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 15
     - PA_4
     - I/O
     - JTAG_SWO
     - JTAG_SWO/I²C_SDA/PWM4/I²S_BCK/ADC1
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 16
     - PA_7
     - I/O
     - GPIO, input, high impedance
     - PWM4/LSPI_MOSI/I²S_MCK/I²S_DI /Touch0
     - 20MHz
     - UP/DOWN
     - 12mA

   * - 17
     - VDD33IO
     - P
     - IO power, 3.3V
     - 
     - 
     - 
     -

   * - 18
     - PB_0
     - I/O
     - GPIO, input, high impedance
     - PWM0/LSPI_MISO/UART3_TX/PSRAM_CK/Touch3
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 19
     - PB_1
     - I/O
     - GPIO, input, high impedance
     - PWM1/LSPI_CK/UART3_RX/PSRAM_CS/Touch4
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 20
     - PB_2
     - I/O
     - GPIO, input, high impedance
     - PWM2/LSPI_CK/UART2_TX/PSRAM_D0/Touch5
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 21
     - PB_3
     - I/O
     - GPIO, input, high impedance
     - PWM3/LSPI_MISO/UART2_RX/PSRAM_D1/Touch6
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 22
     - PB_4
     - I/O
     - GPIO, input, high impedance
     - LSPI_CS/UART2_RTS/UART4_TX/PSRAM_D2/Touch7
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 23
     - PB_5
     - I/O
     - GPIO, input, high impedance
     - LSPI_MOSI/UART2_CTS/UART4_RX/PSARM_D3/Touch8
     - 80MHz
     - UP/DOWN
     - 12mA

   * - 24
     - VDD33IO
     - P
     - IO power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 25
     - CAP
     - I
     - External capacitor, 4.7µF
     - 
     - 
     - 
     - 

   * - 26
     - PB_6
     - I/O
     - GPIO, input, high impedance
     - UART1_TX/MMC_CLK/HSPI_CK/SDIO_CK/Touch9
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 27
     - PB_7
     - I/O
     - GPIO, input, high impedance
     - UART1_RX/MMC_CMD/HSPI_INT/SDIO_CMD/Touch10
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 28
     - PB_8
     - I/O
     - GPIO, input, high impedance
     - I²S_BCK/MMC_D0/PWM_BREAK/SDIO_D0/Touch11
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 29
     - PB_9
     - I/O
     - GPIO, input, high impedance
     - I²S_LRCK/MMC_D1/HSPI_CS/SDIO_D1/Touch12
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 30
     - PB_10
     - I/O
     - GPIO, input, high impedance
     - I²S_DI/MMC_D2/HSPI_DI/SDIO_D2
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 31
     - VDD33IO
     - P
     - IO power supply, 3.3V
     - 
     - 
     - 
     - 

   * - 32
     - PB_11
     - I/O
     - GPIO, input, high impedance
     - I²S_DO/MMC_D3/HSPI_DO/SDIO_D3
     - 50MHz
     - UP/DOWN
     - 12mA

   * - 33
     - GND
     - P
     - Grounding
     - 
     - 
     - 
     - 

.. _application-program:

Application Development
--------------------------

Before powering on the W800-Arduino, please first confirm that the development board is intact.

Now, please proceed to the Quick Start Guide section on :ref:`Set Up the Compilation Environment<compiling-environment>` to see how to set up the development environment and try flashing the sample project to your development board.

.. _schematic-diagram:

Development Board Schematic and Layout Diagram
-------------------------------------------------

The schematic diagram of the W800-Arduino development board is shown below.

.. figure:: ../../_static/get_started/hardware/w800_arduino_schematic_diagram.png
    :align: center
    :alt: W800-Arduino Schematic Diagram of the Development Board

The layout diagram of the W800-Arduino development board is shown below.

.. figure:: ../../_static/get_started/hardware/w800_arduino_layout_diagram.png
    :align: center
    :alt: W800-Arduino Layout Diagram of the Development Board
	
	
Download Reference Materials
-------------------------------

- `W800-Arduino Development Board v3 <../../../../download/board/w800_arduino_board_v3.1_20240808.zip>`__
- `W800-Arduino Development Board v4 <../../../../download/board/w800_arduino_board_v4_20240826.zip>`__

The following table outlines the differences between the W800-Arduino Development Board v4 and v3:

.. list-table::
  :header-rows: 0
  :align: center
  
  * - Hardware Versions
    - USB to Serial Port
    - Pins
    - I2S
    - Auto-Reset
	
  * - v3
    - Micro USB
    - Silkscreen Without Pin Numbering
    - No MCLK Pin
    - No
	
  * - v4
    - Type-C
    - Silkscreen With Pin Numbering

      Provided more power and ground pins

      Reduced by 1 RESET pin

      Added 1 set of SPI pins

      Reduced by 1 set of I2S pins
    - With MCLK pin
    - Yes

      The serial port RTS pin is connected to the development board's RESET pin.
	  