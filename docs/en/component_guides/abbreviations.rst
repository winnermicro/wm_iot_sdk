:orphan:

.. |Equipment-Name| replace:: W800

Index of Abbreviations
=======================

.. glossary::

    A2DP
      Advanced Audio Distribution Profile
    
    ADC
      Analog-to-Digital Converter
    
    ALPN
      Application Layer Protocol Negotiation
    
    AT
      AT stands for "attention". 
    
    AT command port
      The port that is used to send AT commands and receive responses. More details are in the :term:`AT port` introduction.

    AT log port
      The port that is used to output log. More details are in the :term:`AT port` introduction.

    AT port
      AT port is the general name of AT log port (that is used to output log) and AT command port (that is used to send AT commands and receive responses). Please refer to for default AT port pins and for how to customize them.

    Bluetooth LE
      Bluetooth Low Energy
    
    BluFi
      Wi-Fi network configuration function via Bluetooth channel

    Command Mode
      Default operating mode of AT. In the command mode, any character received by the AT command port will be treated as an AT command, and AT returns the command execution result to the AT command port.
      AT enters :term:`Data Mode` from :term:`Command Mode` in the following cases.

      .. parsed-literal::

        * After sending the :ref:`AT+CIPSEND <cmd-SEND>` set command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSEND <cmd-SEND>` execute command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSENDL <cmd-SENDL>` set command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSENDEX <cmd-SENDEX>` set command successfully and returns `OK`.
        |Equipment-Name| * After sending the :ref:`AT+BTSPPSEND <cmd-BTSPPSEND>` execute command successfully and returns `OK`.
        |Equipment-Name| * After sending the :ref:`AT+BLESPP <cmd-BLESPP>` execute command successfully and returns `OK`.

      In the data mode, send the :ref:`+++ <cmd-PLUS>` command, AT will exit from :term:`Data Mode` and enter the :term:`Command Mode`.

    Data Mode
      In the data mode, any character received by the AT command port will be treated as data (except for special :ref:`+++ <cmd-PLUS>`) instead of the AT command, and these data will be sent to the opposite end without modification.
      AT enters :term:`Data Mode` from :term:`Command Mode` in the following cases.

      .. parsed-literal::

        * After sending the :ref:`AT+CIPSEND <cmd-SEND>` set command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSEND <cmd-SEND>` execute command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSENDL <cmd-SENDL>` set command successfully and returns `OK`.
        * After sending the :ref:`AT+CIPSENDEX <cmd-SENDEX>` set command successfully and returns `OK`.
        |Equipment-Name| * After sending the :ref:`AT+BTSPPSEND <cmd-BTSPPSEND>` execute command successfully and returns `OK`.
        |Equipment-Name| * After sending the :ref:`AT+BLESPP <cmd-BLESPP>` execute command successfully and returns `OK`.

      In the data mode, send the :ref:`+++ <cmd-PLUS>` command, AT will exit from :term:`Data Mode` and enter the :term:`Command Mode`.

    DHCP
      Dynamic Host Configuration Protocol

    DNS
      Domain Name System
    
    DTIM
      Delivery Traffic Indication Map
    
    GATTC
      Generic Attributes client
    
    GATTS
      Generic Attributes server

    HID
      Human Interface Device

    I2C
      Inter-Integrated Circuit

    ICMP
      Intemet Control Message Protocol

    LwIP
      A Lightweight TCP/IP stack

    LWT
      Last Will and Testament

    MAC
      Media Access Control

    mDNS
      Multicast Domain Name System

    manufacturing nvs
      Manufacturing Non-Volatile Storage. ``manufacturing nvs`` stores all certificates, private keys, GATTS data, module information, Wi-Fi configurations, UART configurations, etc. These configurations are finally built into the ``mfg_nvs.bin`` file and downloaded to the flash at the address defined in ``at_customize.csv``.

    MSB
      Most Significant Bit

    MTU
      maximum transmission unit
      
    NVS
      Non-Volatile Storage

    Normal Transmission Mode
      Default Transmission Mode

      In normal transmission mode, users can send AT commands. For examples, users can send MCU data received by AT command port to the opposite end of transmission by :ref:`AT+CIPSEND <cmd-SEND>`; and the data received from the opposite end of transmission will also be returned to MCU through AT command port with additional prompt: `+IPD`.

      During a normal transmission, if the connection breaks, |Equipment-Name| will give a prompt and will not attempt to reconnect.

      More details are in :term:`Transmission Mode Shift Diagram`.

    OWE
      Opportunistic Wireless Encryption. OWE is a Wi-Fi standard which ensures that the communication between each pair of endpoints is protected from other endpoints.

      More details are in `Wikipedia <https://en.wikipedia.org/wiki/Opportunistic_Wireless_Encryption>`__.

    Passthrough Mode
      
	  Also called as "Passthrough Sending & Receiving Mode".
      
      In passthrough mode, users cannot send AT commands except special :ref:`+++ <cmd-PLUS>` command. All MCU data received by AT command port will be sent to the opposite end of transmission without any modification; and the data received from the opposite end of transmission will also be returned to MCU through AT command port without any modification.

      During the Wi-Fi passthrough transmission, if the connection breaks, |Equipment-Name| (as client) will keep trying to reconnect until :ref:`+++ <cmd-PLUS>` is input to exit the passthrough transmission; |Equipment-Name| (as server) will shutdown the old connection and listen new connection until :ref:`+++ <cmd-PLUS>` is input to exit the passthrough transmission.

      More details are in :term:`Transmission Mode Shift Diagram`.

    Transmission Mode Shift Diagram
      .. figure:: ../../_static/abbreviations/intro_tt_mode.png
        :align: center
        :alt: Transmission Mode Shift Diagram
        :figclass: align-center
    
        Transmission Mode Shift Diagram

      More details are in the following introduction.

      * :term:`Normal Transmission Mode`
      * :term:`Passthrough Receiving Mode`
      * :term:`Passthrough Mode`
      * :ref:`AT+CIPMODE <cmd-IPMODE>`
      * :ref:`AT+CIPSEND <cmd-SEND>`
      * :ref:`+++ <cmd-PLUS>`

    Passthrough Receiving Mode
      The temporary mode between :term:`Normal Transmission Mode` and :term:`Passthrough Mode`.

      In passthrough receiving mode, AT cannot send any data to the opposite end of transmission; but the data received from the opposite end of transmission can be returned to MCU through AT command port without any modification. More details are in :term:`Transmission Mode Shift Diagram`.

    PBC
      Push Button Configuration
    
    PCI Authentication
      Payment Card Industry Authentication. In WinnerMicro-AT project, it refers to all Wi-Fi authentication modes except OPEN and WEP.

    PKI
      A public key infrastructure (PKI) is a set of roles, policies, hardware, software and procedures needed to create, manage, distribute, use, store and revoke digital certificates and manage public-key encryption.

      More details are in `Public Key Infrastructure <https://en.wikipedia.org/wiki/Public_key_infrastructure>`__.

    PLCP
      Physical Layer Convergence Procedure

    PMF
      protected management frame

    PSK
      Pre-shared Key

    PWM
      Pulse-Width Modulation

    QoS
      Quality of Service

    RTC
      Real Time Controller. A group of circuits in SoC that keeps working in any chip mode and at any time.

    SMP
      Security Manager Protocol

    SNI
      Server Name Indication

    SNTP
      Simple Network Time Protocol

    SPI
      Serial Peripheral Interface
      
    SPP
      Serial Port Profile

    SSL
      Secure Sockets Layer

    system message
      Data sent via AT command port to MCU. Each system message usually ends with ``\r\n``. Detailed system message descriptions are available at :ref:`AT Messages <at-messages>`.

    TLS
      Transport Layer Security

    URC
      Unsolicited Result Code

    UTC
      Coordinated Universal Time

    UUID
      universally unique identifier

    WEP
      Wired-Equivalent Privacy

    WPA
      Wi-Fi Protected Access

    WPA2
      Wi-Fi Protected Access II

    WPS
      Wi-Fi Protected Setup
