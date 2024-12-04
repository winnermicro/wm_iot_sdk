
.. |Equipment-Name| replace:: w800
.. _WiFi-AT:

****************************
Wi-Fi AT Command Set
****************************


-  :ref:`AT+CWINIT <cmd-INIT>`                  : Initialize/Deinitialize Wi-Fi driver
-  :ref:`AT+CWMODE <cmd-MODE>`                  : Query/Set the Wi-Fi mode (Station/SoftAP/Station+SoftAP)
-  :ref:`AT+CWSTATE <cmd-WSTATE>`               : Query the Wi-Fi status and Wi-Fi information
-  :ref:`AT+CWJAP <cmd-JAP>`                    : Connect to an AP
-  :ref:`AT+CWRECONNCFG <cmd-RECONNCFG>`        : Query/Set the Wi-Fi reconnection configuration
-  :ref:`AT+CWLAPOPT <cmd-LAPOPT>`              : Set properties of :ref:`AT+CWLAP <cmd-LAP>` command scan results
-  :ref:`AT+CWLAP <cmd-LAP>`                    : Scan the currently available APs
-  :ref:`AT+CWQAP <cmd-QAP>`                    : Disconnect from AP
-  :ref:`AT+CWSAP <cmd-SAP>`                    : Configure |Equipment-Name| SoftAP parameters
-  :ref:`AT+CWLIF <cmd-LIF>`                    : Query Station information connected to |Equipment-Name| SoftAP
-  :ref:`AT+CWQIF <cmd-QIF>`                    : Disconnect Station from |Equipment-Name| SoftAP
-  :ref:`AT+CWDHCP <cmd-DHCP>`                  : Enable/Disable DHCP
-  :ref:`AT+CWDHCPS <cmd-DHCPS>`                : Query/Set |Equipment-Name| SoftAP DHCP Assigned IPv4 Address Range
-  :ref:`AT+CWAUTOCONN <cmd-AUTOC>`             : Does it automatically connect to the AP when powered on
-  :ref:`AT+CWAPPROTO <cmd-APPROTO>`            : Query/Set the 802.11 b/g/n protocol standard of SoftAP mode
-  :ref:`AT+CWSTAPROTO <cmd-STAPROTO>`          : Query/Set the 802.11 b/g/n protocol standard of Station mode
-  :ref:`AT+CIPSTAMAC <cmd-STAMAC>`             : Query/Set the MAC Address of the |Equipment-Name| station 
-  :ref:`AT+CIPAPMAC <cmd-APMAC>`               : Query/Set the MAC Address of the |Equipment-Name| SoftAP 
-  :ref:`AT+CIPSTA <cmd-IPSTA>`                 : Query/Set the IP address of |Equipment-Name| station
-  :ref:`AT+CIPAP <cmd-IPAP>`                   : Query/Set the IP address of |Equipment-Name| SoftAP 
-  :ref:`AT+CWSTARTSMART(Not supported) <cmd-STARTS>`: Enable SmartConfig
-  :ref:`AT+CWSTOPSMART(Not supported) <cmd-STOPS>`  : Stop SmartConfig
-  :ref:`AT+WPS(Not supported) <cmd-WPS>`            : Set the WPS function
-  :ref:`AT+MDNS(Not supported) <cmd-MDNS>`          : Set the mDNS function
-  :ref:`AT+CWJEAP(Not supported) <cmd-JEAP>`        : Connect to a WPA2 Enterprise AP
-  :ref:`AT+CWHOSTNAME <cmd-HOSTNAME>`          : Query/Set the host name of the |Equipment-Name| station
-  :ref:`AT+CWCOUNTRY <cmd-COUNTRY>`            : Query/Set the Wi-Fi Country Code



.. _cmd-INIT:

:ref:`AT+CWINIT <WiFi-AT>`: Initialize/Deinitialize Wi-Fi Driver
-------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the Wi-Fi initialization status of the |Equipment-Name| device

**Command:**

::

    AT+CWINIT?

**Response:**

::

    +CWINIT:<init>
    OK

Set Command
^^^^^^^^^^^^^^^^^^^

**Function:**

Initialize or deinitialize the Wi-Fi driver of |Equipment-Name| device

**Command:**

::

    AT+CWINIT=<init>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^^^^^^^

-  **<init>**: 

   -  0: Deinitialize Wi-Fi drivers
   -  1: Initialize Wi-Fi driver (default value)

Note
^^^^^

- This setting will not be saved to flash, and will be restored to the default value of 1 after restarting.
- When you run out of RAM resources, you can use this command to clean up Wi-Fi drivers and free up RAM resources without using Wi-Fi.

Example
^^^^^^^^^^^^^^

::

    // Clean Wi-Fi drivers
    AT+CWINIT=0

.. _cmd-MODE:

:ref:`AT+CWMODE <WiFi-AT>`: Query/Set the Wi-Fi Mode (Station/SoftAP/Station+SoftAP)
--------------------------------------------------------------------------------------------


Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the Wi-Fi mode of  |Equipment-Name| device

**Command:**

::

    AT+CWMODE?

**Response:**

::

    +CWMODE:<mode>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the Wi-Fi mode of  |Equipment-Name| device

**Command:**

::

    AT+CWMODE=<mode>[,<auto_connect>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<mode>**: Mode

   -  0: No Wi-Fi mode and Wi-Fi RF turned off
   -  1: Station mode
   -  2: SoftAP mode
   -  3: SoftAP+Station mode

-  **<auto_connect>**: When switching the Wi-Fi mode of the |Equipment-Name| device (for example, switching from SoftAP or No Wi-Fi mode to Station mode or SoftAP+Station mode) whether to enable the automatic connection function to the AP, default value: 1. When the parameter is set to omitted, the default value is used, which means it can automatically connect.

   -  0: Disable the automatic connection to the AP function
   -  1: Enable the automatic connection function to the AP. If the configuration for automatic connection to the AP has been saved to flash before, the |Equipment-Name| device will automatically connect to the AP

Note
^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, this setting will be saved in the NVS partition
-  When switching back to Station mode with AT+CWMODE=1 , it will default to automatically connecting to Wi-Fi.

Example
^^^^^^^^^^^^^^^

::

    AT+CWMODE=1 
  
.. _cmd-WSTATE:

:ref:`AT+CWSTATE <WiFi-AT>`: Query the Wi-Fi Status and Information
------------------------------------------------------------------------------


Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query  the Wi-Fi status and information  of the |Equipment-Name| device

**Command:**

::

    AT+CWSTATE?

**Response:**

::

    +CWSTATE:<state>,<"ssid">
    OK

Parameters
^^^^^^^^^^^^

-  **<state>**: Current Wi-Fi status

   - 0: |Equipment-Name| Station has not yet established any Wi-Fi connection
   - 1: |Equipment-Name| Station has connected to the AP, but has not yet obtained an IPv4 address
   - 2: |Equipment-Name| Station has connected to the AP and obtained an IPv4 address
   - 3: |Equipment-Name| Station is currently undergoing Wi-Fi connection or Wi-Fi reconnection
   - 4: |Equipment-Name| Station is in Wi-Fi disconnected state

-  **<"ssid">**: SSID of the target AP

Note
^^^^^

- When the |Equipment-Name| Station is not connected to the AP, it is recommended to use this command to query Wi-Fi information; After the |Equipment-Name| Station is connected to the AP, it is recommended to use the :ref:`AT+CWJAP <cmd-JAP>` command to query Wi-Fi information

.. _cmd-JAP:

:ref:`AT+CWJAP <WiFi-AT>`: Connect to an AP
---------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query information about the AP connected to the |Equipment-Name| station

**Command:**

::

    AT+CWJAP?

**Response:**

::

    +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<pci_en>,<reconn_interval>,<listen_interval>,<scan_mode>,<pmf>

    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the AP that |Equipment-Name| Station needs to connect to

**Command:**

::

    AT+CWJAP=<ssid>,<pwd>[,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]

**Response:**

::

    WIFI CONNECTED
    OK
    WIFI GOT IP
    [WIFI GOT IPv6 LL]
    [WIFI GOT IPv6 GL]

or

::

    ERROR

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function:**

Connect the |Equipment-Name| Station to the AP from the previous Wi-Fi configuration

**Command:**

::

    AT+CWJAP

**Response:**

::

    WIFI CONNECTED
    OK
    WIFI GOT IP
    [WIFI GOT IPv6 LL]
    [WIFI GOT IPv6 GL]

or

::

    ERROR

Parameters
^^^^^^^^^^^^

-  **<ssid>**: SSID of the target AP

   -  If there are special characters such as ``,`` , ``"`` and ``\\`` in the SSID and password, they need to be escaped
   -  AT supports connecting to APs with Chinese SSID, but some routers or hotspots have Chinese SSID that are not in UTF-8 encoding format. You can scan the SSID first, and then use the scanned SSID to connect.

-  **<pwd>**            : A string parameter representing the password, ranging from 8 to 63 bytes in ASCII (or 64 bytes in HEX).
-  **<bssid>**          : The MAC address of the target AP. This parameter cannot be omitted when multiple APs have the same SSID.

-  **<channel>**        : The channel number
-  **<rssi>**           : The signal strength.
-  **<pci_en>**         : PCI certification

   - 0: |Equipment-Name| Station can connect to AP with any type of encryption method, including OPEN and WEP
   - 1: |Equipment-Name| Station can connect to AP with any type of encryption method except OPEN and WEP

-  **<reconn_interval>**: Wi-Fi reconnection interval, in seconds, default value: 1, maximum value: 7200

   -  0       : |Equipment-Name| Station will not reconnect to AP after disconnection
   -  [1,7200]: |Equipment-Name| Station will reconnect to the AP at the specified interval after disconnection

-  **<listen_interval>**: Interval for listening to AP beacons, in units of AP beacon intervals, default value: 3, range: [1,100]
-  **<scan_mode>**      : Scan mode

   -  0: Fast scan, The scan terminates after finding the target AP, |Equipment-Name| Station connects to the first detected AP
   -  1: Full channel scan, The scan terminates only after all channels have been scanned, |Equipment-Name| Station connects to the AP with the strongest signal

-  **<jap_timeout>**    : The Maximum timeout value for the :ref:`AT+CWJAP <cmd-JAP>` command, in seconds, default value: 15, range: [3,600]
-  **<pmf>**: PMF (Protected Management Frames), default value: 1

   - 0 : PMF disabled
   - bit 0: Indicates support for PMF, |Equipment-Name| device will prefer to connect in PMF mode if other devices also support PMF
   - bit 1:  Indicates requirement for PMF, device will not associate with devices that do not support PMF

Note
^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, the configuration changes will be saved to the NVS partition
-  This command requires station mode to be enabled.
-  When  the |Equipment-Name| station  has connected to an AP, it is recommended to use this command to query Wi-Fi information; when |Equipment-Name| Station is not connected to an AP, it is recommended to use :ref:`AT+CWSTATE <cmd-WSTATE>` command to query Wi-Fi information
-  The ``<reconn_interval>`` parameter in this command is the same as the ``<interval_second>``parameter in the :ref:`AT+CWRECONNCFG <cmd-RECONNCFG>` command. If the ``<reconn_interval>`` parameter is not set when this command is executed, the default Wi-Fi reconnection interval of 1 will be used
-  If both ``<ssid>`` and  ``<password>`` parameters are omitted, the last set values will be used
-  The timeout for executing the command and setting the command is the same, with a default value of 15 seconds, which can be set by the ``<jap_timeout>`` parameter
-  To obtain an IPv6 address,:ref:`AT+CIPV6=1 <cmd-IPV6>` must be set first
-  The response  ``OK``  indicates that the IPv4 network is ready, but does not indicate that the IPv6 network is ready. Currently, WinnerMicro-AT prioritizes the IPv4 network, with the IPv6 network as a secondary option
-  ``WIFI GOT IPv6 LL``  indicates that a local link IPv6 address has been obtained, which is calculated locally using EUI-64 and does not require router involvement. Due to parallel timing,this print may appear before or after ``OK``
-  ``WIFI GOT IPv6 GL`` indicates that a global IPv6 address has been obtained, which is a combination of the prefix issued by the AP and the suffix calculated internally, requiring router involvement. Due to parallel timing, this print may appear before or after ``OK`` ; it may also not be printed if the AP does not support IPv6

Examples
^^^^^^^^^

::

    // If the SSID of the target AP is "WinnerMicro_WiFi" and the password is "12345678", the command is:
    AT+CWJAP="WinnerMicro_WiFi","12345678"

    // If multiple APs have the same SSID "WinnerMicro-WiFi", the target AP can be found through BSSID:
    AT+CWJAP="WinnerMicro_WiFi","12345678","28:6d:cd:55:e5:7b" 

.. _cmd-RECONNCFG:

:ref:`AT+CWRECONNCFG <WiFi-AT>`: Query/Set the Wi-Fi reconnection configuration
-------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query Wi-Fi reconnection configuration

**Command:**

::

    AT+CWRECONNCFG?

**Response:**

::

    +CWRECONNCFG:<interval_second>,<repeat_count>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set Wi-Fi reconnection configuration

**Command:**

::

    AT+CWRECONNCFG=<interval_second>,<repeat_count>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<interval_second>**: Wi-Fi reconnection interval, unit: seconds, default value: 0, maximum value 7200

   -  0       : After disconnecting, the |Equipment-Name| Station does not reconnect to AP
   -  [1,7200]: After disconnecting, the |Equipment-Name| Station reconnects with AP every specified time

-  **<repeat_count>**   : The number of times the |Equipment-Name| device attempts to reconnect to the AP, this parameter is valid when ``<interval_second>`` is not 0, default value: 0, maximum value: 1000

   -  0       : the |Equipment-Name| Station always tries to connect to AP
   -  [1,1000]: the |Equipment-Name| Station will reconnect to the AP according to the specified number of times in this parameter

Example
^^^^^^^^^

.. parsed-literal::

    // |Equipment-Name| Station attempts to reconnect to the AP every 1 second for a total of 100 attempts
    AT+CWRECONNCFG=1,100

    // |Equipment-Name| Station does not reconnect to AP after disconnecting
    AT+CWRECONNCFG=0,0

Note
^^^^^
-  If AT+SYSSTORE=1, the configuration changes will be saved to the NVS partition.
-  The ``<interval_second>`` parameter in this command is the same as the ``[<reconn_interval>]`` parameter in: :ref:`AT+CWJAP <cmd-JAP>` 
-  This command is suitable for passive disconnection of AP, Wi Fi mode switching, and automatic Wi Fi connection after power on

.. _cmd-LAPOPT:

:ref:`AT+CWLAPOPT <WiFi-AT>`: Set Properties of :ref:`AT+CWLAP <cmd-LAP>` Command scan results
----------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^

**Command:**

::

    AT+CWLAPOPT=<print mask>[,<rssi filter>][,<authmode mask>]

**Response:**

::

    OK

or

::

    ERROR

Parameters
^^^^^^^^^^^^

-  **<print mask>**: :ref:`AT+CWLAP <cmd-LAP>`  Whether the scanning result displays the following parameters, default value: 0x1FF. If the bit is set to 1, the corresponding parameter is displayed. If set to 0, the corresponding parameter is not displayed

   -  bit 0: Whether to display <ecn>
   -  bit 1: Whether to display <ssid>
   -  bit 2: Whether to display <rssi>
   -  bit 3: Whether to display <mac>
   -  bit 4: Whether to display <channel>
   -  bit 5: Whether to display <pairwise_cipher>
   -  bit 6: Whether to display <group_cipher>
   -  bit 7: Whether to display <bgn>
   -  bit 8: Whether to display <wps>

-  **[<rssi filter>]**  : Whether the scanning results of :ref:`AT+CWLAP <cmd-LAP>` are filtered according to this parameter, that is, whether APs with signal strength lower than the ``rssi filter`` parameter value are filtered out, unit: dBm, default value: -100, range: [-100,40]
-  **[<authmode mask>]**: The scanning result of :ref:`AT+CWLAP <cmd-LAP>` will be displayed with the following authentication method, default value: 0xFFFF.  If bit x is set to 1, then APs with the corresponding authentication method will be displayed. If it is set to 0, then APs with that authentication method will not be displayed.

   -  bit 0: Whether to display the AP with ``OPEN`` authentication method
   -  bit 1: Whether to display the AP with ``WEP`` authentication method
   -  bit 2: Whether to display the AP with ``WPA_PSK`` authentication method
   -  bit 3: Whether to display the AP with ``WPA2_PSK`` authentication method
   -  bit 4: Whether to display the AP with ``WPA_WPA2_PSK`` authentication method
   -  bit 5: Whether to display the AP with ``WPA2_ENTERPRISE`` authentication method
   -  bit 6: Whether to display the AP with ``WPA3_PSK`` authentication method
   -  bit 7: Whether to display the AP with ``WAPI_PSK`` authentication method
   -  bit 8: Whether to display the AP with ``WPA2_WPA3_PSK`` authentication method
   -  bit 9: Whether to display the AP with ``OWE`` authentication method

Examples
^^^^^^^^^^^^

::

    // With the first parameter set to 31, which is 0x1F in hexadecimal, it indicates that all parameters corresponding to bits with a value of 1 will be displayed.
    AT+CWLAPOPT=31
    AT+CWLAP

    // Only display APs with OPEN authentication method.
    AT+CWLAPOPT=31,-100,1
    AT+CWLAP

.. _cmd-LAP:

:ref:`AT+CWLAP <WiFi-AT>`: Scan the Currently Available APs
--------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^

**Function:**

List APs that meet specific criteria, such as specifying SSID, MAC address, or channel number

**Command:**

::

    AT+CWLAP=[<ssid>,<mac>,<channel>,<scan_type>,<scan_time_min>,<scan_time_max>]

Execute command
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function:**

List the currently available APs

**Command:**

::

    AT+CWLAP

**Response:**

::

    +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<pairwise_cipher>,<group_cipher>,<bgn>,<wps>
    OK

Parameters
^^^^^^^^^^^^

-  **<ecn>**: Encryption method

   -  0: OPEN
   -  1: WEP
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA_WPA2_PSK
   -  5: WPA2_ENTERPRISE
   -  6: WPA3_PSK
   -  7: WAPI_PSK
   -  8: WPA2_WPA3_PSK
   -  9: OWE

-  **<ssid>**     : String parameter, SSID of AP
-  **<rssi>**     : Signal strength
-  **<mac>**      : String parameter, MAC address of AP
-  **<channel>**  : Channel number
-  **<scan_type>**: Wi Fi scan type, default value: 0

   -  0: Active scanning
   -  1: Passive scanning
-  **<scan_time_min>**: Minimum Scan Time for each channel, unit: milliseconds, range: [0,1500]. If the scanning type is passive scanning, this parameter is invalid
-  **<scan_time_max>**: Maximum Scan time for each channel, unit: milliseconds, range: [0,1500], if set to 0, firmware uses default parameter values, active scanning is 120 ms, passive scanning is 360 ms

-  **<pairwise_cipher>**: Paired encryption type

   - 0: None
   - 1: WEP40
   - 2: WEP104
   - 3: TKIP
   - 4: CCMP
   - 5: TKIP and CCMP
   - 6: AES-CMAC-128
   - 7: Unknown

-  **<group_cipher>**: Group encryption type, with the same enumeration value as the ``<pairwise_cipher>`` parameter
-  **<bgn>**         : 802.11 b/g/n, if the bit is set to 1, it means the corresponding mode is enabled, if set to 0, it means the corresponding mode is disabled

   -  bit 0: Enable 802.11b mode or not
   -  bit 1: Is 802.11g mode enabled
   -  bit 2: Is 802.11n mode enabled

-  **<wps>**: wps flag

   - 0: Does not support WPS
   - 1: Support WPS

Examples
^^^^^^^^^

::

    AT+CWLAP="WinnerMicro_WiFi","42:c1:82:80:2a:15",6,0
    
    // Find the AP with the specified SSID
    AT+CWLAP="WinnerMicro_WiFi"

.. _cmd-QAP:

:ref:`AT+CWQAP <WiFi-AT>`: Disconnect from AP
--------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWQAP

**Response:**

::

    OK

.. _cmd-SAP:

:ref:`AT+CWSAP <WiFi-AT>`: Configure |Equipment-Name| SoftAP parameters
-----------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the configuration parameters of |Equipment-Name| SoftAP

**Command:**

::

    AT+CWSAP?

**Response:**

::

    +CWSAP:<ssid>,<pwd>,<channel>,<ecn>,<max conn>,<ssid hidden>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the configuration parameters for|Equipment-Name| SoftAP 

**Command:**

::

    AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>[,<max conn>][,<ssid hidden>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<ssid>**   : String parameter, access point name
-  **<pwd>**    : String parameter, password, range: 8~63 bytes ASCII
-  **<channel>**: Channel number
-  **<ecn>**    : Encryption method, does not support WEP

   -  0: OPEN
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA_WPA2_PSK

-  **[<max conn>]**   : The maximum number of stations allowed to connect to |Equipment-Name| SoftAP .
-  **[<ssid hidden>]**: 

   -  0: Broadcast SSID (default)
   -  1: Do not broadcast SSID

Note
^^^^^

- If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved in the NVS partition
- This instruction is only valid when :ref:`AT+CWMODE=2 <cmd-MODE>` or :ref:`AT+CWMODE=3 <cmd-MODE>`
- The default SSID varies by device as it is composed of the device's MAC address. You can use :ref:`AT+CWSAP? <cmd-SAP>` command to query the default SSID.

Examples
^^^^^^^^^

::

    AT+CWSAP="WinnerMicro_WiFi","12345678",5,3

.. _cmd-LIF:

:ref:`AT+CWLIF <WiFi-AT>`: Query Station Information Connected to |Equipment-Name| SoftAP
---------------------------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWLIF

**Response:**

::

    +CWLIF:<ip addr>,<mac>
    OK

Parameters
^^^^^^^^^^^^

-  **<ip addr>**: IP address of the Station connected to the |Equipment-Name| SoftAP
-  **<mac>**    : MAC address of the Station connected to the |Equipment-Name| SoftAP

Note
^^^^^^

- This command cannot query static IP addresses and is only valid when both |Equipment-Name| SoftAP and the connected station DHCP are enabled

.. _cmd-QIF:

:ref:`AT+CWQIF <WiFi-AT>`: Disconnect Station from |Equipment-Name| SoftAP
------------------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function:**

Disconnect all stations connected to the |Equipment-Name| SoftAP

**Command:**

::

    AT+CWQIF

**Response:**

::

    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Disconnect a station connected to the |Equipment-Name| SoftAP

**Command:**

::

    AT+CWQIF=<mac>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<mac>**: The MAC address of the station that needs to be disconnected

.. _cmd-DHCP:

:ref:`AT+CWDHCP <WiFi-AT>`: Enable/Disable DHCP
-----------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWDHCP?

**Response:**

::

  +CWDHCP:<state>
  OK

Set Command
^^^^^^^^^^^^^^

**Function:**
 
Enable/Disable DHCP

**Command:**

::

    AT+CWDHCP=<operate>,<mode>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<operate>**: 

   -  0: Disable
   -  1: Enable

-  **<mode>**: 

   -  Bit0: DHCP for Station
   -  Bit1: DHCP for SoftAP

-  **<state>**: DHCP status
   
   - Bit0:

     - 0: Disable DHCP for Station
     - 1: Enable DHCP for Station
   
   - Bit1:

     - 0: Disable DHCP for SoftAP
     - 1: Enable DHCP for SoftAP

   - Bit2:

     - 0: Disable DHCP for Ethernet
     - 1: Enable DHCP for Ethernet

Note
^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved to the NVS partition
-  This setting command and the command to set a static IPv4 address will interact with each other, for example: :ref:`AT+CIPSTA <cmd-IPSTA>`
  
  -  If static IPv4 address is enabled, DHCP will be automatically disabled
  -  If DHCP is enabled, static IPv4 addresses will be automatically disabled
  -  The last configuration will overwrite the previous configuration
  
Example
^^^^^^^^^^^^^^^^^^^^^

::

    // Enable Station DHCP. If the original DHCP mode was 2, the current DHCP mode is 3
    AT+CWDHCP=1,1

    // Disable SoftAP DHCP. If the original DHCP mode was 3, the current DHCP mode is 1
    AT+CWDHCP=0,2


.. _cmd-DHCPS:

:ref:`AT+CWDHCPS <WiFi-AT>`: Query/Set |Equipment-Name| SoftAP DHCP Assigned IPv4 Address Range
-----------------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWDHCPS?

**Response:**

::

    +CWDHCPS=<lease time>,<begin IP>,<end IP>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**
 
Set the IPv4 Address Range Assigned by the SoftAP DHCP Server

**Command:**
 
::
 
 AT+CWDHCPS=<enable>,<lease time>,<begin IP>,<end IP>

**Response:**

::

    OK


Parameters
^^^^^^^^^^^^

-  **<enable>**：
   
   - 0: Clearing DHCP server information to restore default settings; subsequent parameters do not need to be filled in.
   - 1: Setting DHCP server information, subsequent parameters must be filled in.
   
-  **<lease time>**: lease time, unit: minute, value range:[1,7200]
-  **<begin IP>**  : Start IP of IPv4 address pool for SoftAP DHCP server of |Equipment-Name|
-  **<end IP>**    : End IP of IPv4 address pool for SoftAP DHCP server of |Equipment-Name|

Note
^^^^^

-  When :ref:AT+SYSSTORE=1 <cmd-SYSSTORE> is executed, the configuration changes will be saved in the NVS (Non-Volatile Storage) area.
-  This command must be enabled in |Equipment-Name| SoftAP mode and used with DHCP server enabled
-  The IPv4 address range set must be in the same network segment as the |Equipment-Name| SoftAP 


.. tip:: 
    
    If you want to change the DHCP range of SoftAP, you can use AT+CIPAP=<"ip">[,<"gateway">,<"netmask">]
    To change the IP address of SoftAP, and then open SoftAP to change the range of DHCP

.. _cmd-AUTOC:

:ref:`AT+CWAUTOCONN <WiFi-AT>`: Does It Automatically Connect to the AP When Powered on
-------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^

**Command:**

::

    AT+CWAUTOCONN=<enable>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<enable>**: 

   -  1: Power on automatically connect to AP (default)
   -  0: Power on does not automatically connect to AP

Note
^^^^^

-  Save this setting to the NVS area

Example
^^^^^^^^^

::

    AT+CWAUTOCONN=1

.. _cmd-APPROTO:

:ref:`AT+CWAPPROTO <WiFi-AT>`: Query/Set the 802.11 b/g/n Protocol Standard in SoftAP Mode
---------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWAPPROTO?

**Response:**

::

    +CWAPPROTO=<protocol>
    OK

Set Command
^^^^^^^^^^^^^^

**Command:**

::

    AT+CWAPPROTO=<protocol>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<protocol>**: 

   -  bit0: 802.11b protocol standard
   -  bit1: 802.11g protocol standard
   -  bit2: 802.11n protocol standard
   -  bit3: 802.11ax protocol standard

Note
^^^^^

-  Currently, the PHY modes supported by |Equipment-Name| device are as follows:  `Pending Wi-Fi protocol mode <https://#>`_
-  By default, the PHY mode of the |Equipment-Name| device is 802.11bgn mode

.. _cmd-STAPROTO:

:ref:`AT+CWSTAPROTO <WiFi-AT>`: Set the 802.11 b/g/n Protocol Standard in Station Mode
--------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CWSTAPROTO?

**Response:**

::

    +CWSTAPROTO=<protocol>
    OK

Set Command
^^^^^^^^^^^^^^

**Command:**

::

    AT+CWSTAPROTO=<protocol>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<protocol>**: 

.. only:: w800

   -  bit0: 802.11b protocol standard
   -  bit1: 802.11b/g protocol standard
   -  bit2: 802.11b/g/n protocol standard


Note
^^^^^

-  Currently, the PHY modes supported by |Equipment-Name| device are as follows:  `Pending Wi-Fi protocol mode <https://#>`_
-  By default, the PHY mode of the |Equipment-Name| device is 802.11bgn mode

.. _cmd-STAMAC:

:ref:`AT+CIPSTAMAC <WiFi-AT>`: Query/Set |Equipment-Name| Station's MAC Address
--------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the MAC address of the |Equipment-Name| Station

**Command:**

::

    AT+CIPSTAMAC?

**Response:**

::

    +CIPSTAMAC:<mac>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the MAC address of |Equipment-Name| Station

**Command:**

::

    AT+CIPSTAMAC=<mac>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<mac>**: String parameter representing the MAC address of |Equipment-Name| Station

Note
^^^^^

- If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved to the NVS partition.
- The MAC address of the  |Equipment-Name| Station is different from |Equipment-Name| SoftAP, do not set the same MAC address for both.
- Bit 0 of the MAC address cannot be 1. For example, the MAC address can be "18:…" but not "15:…".
- FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00 are invalid addresses and cannot be set.

Example
^^^^^^^^^

::

    AT+CIPSTAMAC="18:6d:cd:55:e5:7b"

.. _cmd-APMAC:

:ref:`AT+CIPAPMAC <WiFi-AT>`: Query/Set the MAC Address of |Equipment-Name| SoftAP
------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^
**Function:**

Query the MAC address of the |Equipment-Name| SoftAP.

**Command:**

::

    AT+CIPAPMAC?

**Response:**

::

    +CIPAPMAC:<mac>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the MAC address of the |Equipment-Name| SoftAP.

**Command:**

::

    AT+CIPAPMAC=<mac>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<mac>**: String parameter representing the MAC address of |Equipment-Name| SoftAP

Note
^^^^^

- If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved to the NVS partition.
- The MAC address of |Equipment-Name| is different from |Equipment-Name| Station, do not set the same MAC address for both.
- Bit 0 of the MAC address cannot be 1. For example, the MAC address can be "18:…", but not "15:…".
- FF:FF:FF:FF:FF:FF and 00:00:00:00:00:00 are invalid addresses and cannot be set.

Examples
^^^^^^^^^

::

    AT+CIPAPMAC="18:fe:35:98:d3:7b" 

.. _cmd-IPSTA:

:ref:`AT+CIPSTA <WiFi-AT>`: Query/Set the IP address of |Equipment-Name| Station
-----------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the IP address of |Equipment-Name| Station.

**Command:**

::

    AT+CIPSTA?

**Response:**

::

    +CIPSTA:ip:<"ip">
    +CIPSTA:gateway:<"gateway">
    +CIPSTA:netmask:<"netmask">
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the IPv4 address of the |Equipment-Name| Station.

**Command:**

::

    AT+CIPSTA=<"ip">[,<"gateway">,<"netmask">]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<"ip">**     : String parameter representing the IPv4 address of |Equipment-Name| Station.
-  **<"gateway">**: Gateway.
-  **<"netmask">**: Subnet mask.

Note
^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved to the NVS partition.
-  When using the query command, the IP address of |Equipment-Name| Station can only be queried if it is connected to the AP or if the static IP address has been configured.
-  This setting command interacts with the command to set DHCP, such as :ref:`AT+CWDHCP <cmd-DHCP>`
  
  -  If a static IPv4 address is enabled, DHCP is automatically disabled.
  -  If DHCP is enabled, the static IPv4 address is automatically disabled..
  -  The last configuration overrides the previous configuration.
  
Example
^^^^^^^^^

::

    AT+CIPSTA="192.168.6.100","192.168.6.1","255.255.255.0" 

.. _cmd-IPAP:

:ref:`AT+CIPAP <WiFi-AT>`: Query/Set the SoftAP IP Address of the  |Equipment-Name| 
-------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the IP address of the |Equipment-Name| SoftAP.

**Command:**

::

    AT+CIPAP?

**Response:**

::

    +CIPAP:ip:<"ip">
    +CIPAP:gateway:<"gateway">
    +CIPAP:netmask:<"netmask">
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the IPv4 address of |Equipment-Name| SoftAP.

**Command:**

::

    AT+CIPAP=<"ip">[,<"gateway">,<"netmask">]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<"ip">**     : String parameter representing the IPv4 address of |Equipment-Name| SoftAP.
-  **<"gateway">**: Gateway.
-  **<"netmask">**: Subnet Mask.

Note
^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved to the NVS partition.
-  This setting command applies only to IPv4 networks, not to IPv6 networks.
  
Example
^^^^^^^^^

::

    AT+CIPAP="192.168.5.1","192.168.5.1","255.255.255.0"

.. _cmd-STARTS:

:ref:`AT+CWSTARTSMART(Not supported) <cmd-STARTS>`: Enable SmartConfig
-------------------------------------------------------------------------------

.. _cmd-STOPS:

:ref:`AT+CWSTOPSMART(Not supported) <cmd-STOPS>`: Stop SmartConfig
-------------------------------------------------------------------------------

.. _cmd-WPS:

:ref:`AT+WPS(Not supported) <cmd-WPS>`    : Set the WPS function
-------------------------------------------------------------------------------

.. _cmd-MDNS:

:ref:`AT+MDNS(Not supported) <cmd-MDNS>`  : Set the mDNS function
-------------------------------------------------------------------------------

.. _cmd-JEAP:

:ref:`AT+CWJEAP(Not supported) <cmd-JEAP>`: Connect to WPA2 Enterprise AP
-------------------------------------------------------------------------------

.. _cmd-HOSTNAME:

:ref:`AT+CWHOSTNAME <WiFi-AT>`: Query/Set the host name of the |Equipment-Name| Station
--------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the Station host name of |Equipment-Name| 

**Command:**

::

    AT+CWHOSTNAME?

**Response:**

::

    +CWHOSTNAME:<hostname>

    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the host name of the |Equipment-Name| Station

**Command:**

::

    AT+CWHOSTNAME=<hostname>

**Response:**

::

    OK

If Station mode is not enabled, returns:

::

    CMD PARSE ERR

Parameter
^^^^^^^^^^^^

-  **<hostname>**: Host name of |Equipment-Name| Station, maximum length: 32 bytes.

Note
^^^^^

-  Configuration changes are not saved to flash.

Example
^^^^^^^^^

::

    AT+CWMODE=3
    AT+CWHOSTNAME="WinnerMicro_WiFi"

.. _cmd-COUNTRY:

:ref:`AT+CWCOUNTRY <WiFi-AT>`: Query/Set the Wi-Fi Country Code
--------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^

**Function:**

Query the Wi-Fi country code

**Command:**

::

    AT+CWCOUNTRY?

**Response:**

::

    +CWCOUNTRY:<country_code>,<start_channel>,<total_channel_count>
    OK

Set Command
^^^^^^^^^^^^^^

**Function:**

Set the Wi-Fi country code

**Command:**

::

    AT+CWCOUNTRY=<country_code>,<start_channel>,<total_channel_count>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<country_code>**       : Country code, maximum length: 3 characters, 
   The current country codes only support CN, JP, US, CA, AL, AM, AT, AZ, BA, BE, BG, BY, CH, CY, CZ, DE, DK, EE, EL, ES, FI, FR,
   GE, HR, HU, IE, IS, IT, LI, LT, LU, LV, MD, ME, MK, MT, NL, NO, PL, PT, RO, RS, RU, SE, SI, SK, TR, UA, and UK.
-  **<start_channel>**      : Starting channel, range: [1,14].
-  **<total_channel_count>**: Total number of channels.

Note
^^^^^

-  For detailed information, refer to:  `Wi-Fi Country/Region Codes  <https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2>`_.
-  Configuration changes are not saved to flash.

Example
^^^^^^^^^

::

    AT+CWMODE=3
    AT+CWCOUNTRY="CN",1,13
