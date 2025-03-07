
.. |Equipment-Name| replace:: w800

.. _Bluetooth_AT:
.. _cmd-EINIT:

****************************
Bluetooth AT Command Set
****************************


-  :ref:`Introduction <cmd-bluetooth-intro>`      
-  :ref:`AT+BLEINIT <cmd-EINIT>`                     : Bluetooth LE initialization
-  :ref:`AT+BLEVERS <cmd-EVERS>`                     : Query Bluetooth LE version information
-  :ref:`AT+BLEADDR <cmd-EADDR>`                     : Set Bluetooth LE device address
-  :ref:`AT+BLENAME <cmd-ENAME>`                     : Query/Set Bluetooth LE Device Name
-  :ref:`AT+BLESCANPARAM <cmd-ESCANPARAM>`           : Query/Set Bluetooth LE Scan Parameters
-  :ref:`AT+BLESCAN <cmd-ESCAN>`                     : Enable Bluetooth LE scanning
-  :ref:`AT+BLBLESCANRSPDATA <cmd-ESCANRSPDATA>`     : Set Bluetooth LE scan response
-  :ref:`AT+BLEADVPARAM <cmd-EADVPARAM>`             : Query/Set Bluetooth LE Broadcast Parameters
-  :ref:`AT+BLEADVDATA <cmd-EADVDATA>`               : Set Bluetooth LE broadcast data
-  :ref:`AT+BLEADVDATAEX <cmd-EADVDATAEX>`           : Automatically set Bluetooth LE broadcast data
-  :ref:`AT+BLEADVSTART <cmd-EADVSTART>`             : Start Bluetooth LE broadcast
-  :ref:`AT+BLEADVSTOP <cmd-EADVSTOP>`               : Stop Bluetooth LE broadcasting
-  :ref:`AT+BLECONN <cmd-ECONN>`                     : Establish Bluetooth LE connection
-  :ref:`AT+BLECONNPARAM <cmd-ECONNPARAM>`           : Query/Update Bluetooth LE connection parameters
-  :ref:`AT+BLEDISCONN <cmd-EDISCONN>`               : Disconnect Bluetooth LE connection
-  :ref:`AT+BLEDATALEN <cmd-EDATALEN>`               : Set Bluetooth LE packet length
-  :ref:`AT+BLECFGMTU <cmd-ECFGMTU>`                 : Set Bluetooth LE MTU length
-  :ref:`AT+BLEGATTSSVC <cmd-EGATTSSVC>`             : GATTS creates/queries local services
-  :ref:`AT+BLEGATTSSVCRMV <cmd-EGATTSSVCRMV>`       : GATTS Delete Local Service
-  :ref:`AT+BLEGATTSNTFY <cmd-EGATTSNTFY>`           : Server notifies service feature values to client
-  :ref:`AT+BLEGATTSIND <cmd-EGATTSIND>`             : Server indicates service characteristic values to clients
-  :ref:`AT+BLEGATTSSETATTR <cmd-EGATTSSETATTR>`     : GATTS sets service characteristic values
-  :ref:`AT+BLEGATTCSVC <cmd-EGATTCSVC>`             : GATTC Discovery Service List 
-  :ref:`AT+BLEGATTCRD <cmd-EGATTCRD>`               : GATTC reads service characteristic values
-  :ref:`AT+BLEGATTCWR <cmd-EGATTCWR>`               : GATTC writes service characteristic values
-  :ref:`AT+BLEGATTCSUBS <cmd-EGATTCSUBS>`           : GATTC subscription service
-  :ref:`AT+BLESECPARAM <cmd-ESECPARAM>`             : Query/Set Bluetooth LE Encryption Parameters
-  :ref:`AT+BLEENC <cmd-EENC>`                       : Initiate Bluetooth LE encryption request
-  :ref:`AT+BLEENCRSP <cmd-EENCRSP>`                 : Reply to pairing requests initiated by the peer device
-  :ref:`AT+BLEKEYREPLY <cmd-EKEYREPLY>`             : Reply the key to the other party's device
-  :ref:`AT+BLECONFREPLY <cmd-ECONFREPLY>`           : Reply with confirmation result to peer device (classic connection stage)
-  :ref:`AT+BLEENCDEV <cmd-EENCDEV>`                 : Query list of Bonded Bluetooth LE encryption devices
-  :ref:`AT+BLEENCCLEAR <cmd-EENCCLEAR>`             : Clear List of Bluetooth LE encrypted devices
-  :ref:`AT+BLEVERS <cmd-EVERS>`                     : Query Bluetooth LE host and controller the firmware version numbers
-  :ref:`AT+BLESPPSCFG <cmd-ESPPSCFG>`               : Configure Bluetooth LE SPP server transparent transmission parameters
-  :ref:`AT+BLESPPCCFG <cmd-ESPPCCFG>`               : Configure Bluetooth LE SPP client transparent transmission parameters
-  :ref:`AT+BLESPPSTART <cmd-ESPPSTART>`             : Enable Bluetooth LE SPP transparent transmission function
-  :ref:`AT+BLESPPSTOP <cmd-ESPPSTOP>`               : Disable Bluetooth LE SPP transparent transmission function
-  :ref:`AT+BLEWIFIPROV <cmd-EWIFIPROV>`             : Query/Set Bluetooth LE WIFI network connection function
-  :ref:`AT+BTTEST <cmd-EBTTEST>`                    ：Enable/Exit Bluetooth Test Mode

.. _cmd-bluetooth-intro:

Introduction
------------------

Currently, the | Equipment Name | series AT firmware supports the Bluetooth Core Specification version 4.2.

.. important::

  	The default AT firmware supports all AT commands listed on this page. If you need to modify the default supported commands of | Equipment Name | , please compile the BLUETOOTH-AT project yourself and select it in the fifth step of configuring the project (each item below is independent, choose according to your needs):

 -  Disable Bluetooth SPP command
 -  Disable WIFIPROV command
 -  Disable Bluetooth LE command

.. _cmd-BLEINIT:

:ref:`AT+BLEINIT <Bluetooth_AT>`: Bluetooth LE initialization
------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

Check the initialization status of Bluetooth LE.

**Command:**

::

   AT+BLEINIT?
    
**Response:**

If initialized, AT returns:

::

   ++BLEINIT:<role>
   OK

If not initialized, AT returns:

::

    +BLEINIT:0
    OK

Set Command
^^^^^^^^^^^^

**Fucntion:**

Initialize the role of the Bluetooth LE.

**Command:**

::

   AT+BLEINIT=<init>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<Initialization>**: 

-  0: Log out of Bluetooth LE protocol stack
-  1: Initialize the Bluetooth LE protocol stack

Note
^^^^^^^^^^^^

- When using Bluetooth LE function, if the SoftAP mode is not required, it is recommended to use AT+CWMOD to set the Wi-Fi mode to NULL or station mode.
- Before using the Bluetooth LE AT command, you should first run this command to trigger the initialization process.
- Before canceling the initialization of the Bluetooth stack, it is necessary to stop broadcasting, stop scanning, and disconnect all existing connections.
- If Bluetooth LE is initialized, AT+CIPODE cannot be set to 1.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1

.. _cmd-EADDR:

:ref:`AT+BLEADDR<Bluetooth_AT>`: Query Bluetooth LE Version Information
---------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

::

   Search for Bluetooth LE public address

::

   AT+BLEADDR?

**Response:**

::

   +BLEADDR:<addr_type>,<addr>
   OK 
                        
Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Function:**

::

  Set the Bluetooth LE address type.

**Command:**

::

   AT+BLEADDR=<addr_type>[,<random_addr>]

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

    -  **<addr_type>**  ：  
    - 0: Public Broadcasting
    - 1: Random address

Note
^^^^^^^^^^^^

- Static addresses should meet the following requirements:
- The two most significant bits of the address should be equal to 1.
- The random part of the address should have at least one digit as 0.
- The random part of the address should have at least one digit as 1.
- Configuration changes will be saved in the NVS area.

Example
^^^^^^^^^^^^   

::

   AT+BLEADDR=1,"f8:7f:24:87:1c:7b"    // Set the random device to use a static address, with the specific address being: "f8:7f:24:87:1c:7b"
   AT+BLEADDR=1                        // Set the random device to use a static address, which is automatically generated by the host protocol stack
   AT+BLEADDR=0                        // Set random devices to use public device addresses

.. _cmd-EVERS:

:ref:`AT+BLEVERS <Bluetooth_AT>`: Query Bluetooth LE Version Information
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

::

   Check the Bluetooth LE firmware version number.

**Command:**

::
 
   AT+BLEVERS?

**Response:**

::

   +BLEVERS:<h-major>,<h-minor>,<h-patch>.<c-major>,<c-minor>,<c-patch>
   OK

    
Parameter
^^^^^^^^^^^^^
 
-  **<"h-major">**: Bluetooth LE host protocol stack major version number.
-  **<"h-minor">**: Bluetooth LE host protocol stack sub version number.
-  **<"h-patch">**: Bluetooth LE host protocol stack patch version number.
-  **<"c-major">**: Bluetooth LE controller protocol stack major version number.
-  **<"c-major">**: Bluetooth LE controller protocol stack version number.
-  **<"c-major">**: Bluetooth LE controller protocol stack patch version number.

Example
^^^^^^^^^^^^

::

   AT+BLEVERS?

.. _cmd-ENAME:

:ref:`AT+BLENAME <Bluetooth_AT>`: Query/Set Bluetooth LE Device Name
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

::

   Query the Bluetooth LE device name.

**Command:**

::
 
   AT+BLENAME?

**Response:**

::

   +BLENAME:<device_name>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

::

   Set the Bluetooth LE device name.

**Command:**

::
 
   AT+BLENAME=<device_name>

**Response:**

::

   OK
    
Parameter
^^^^^^^^^^^^^
 
-  **<"device_name">**: Bluetooth LE device name. The maximum length is 32. Default value: "WM-AT".

Note
^^^^^^^^^^^^
   
- The configuration changes will be saved in the NVS area.
- After using this command to set the device name, it is recommended that you execute the AT+LEADVDATA command to add the device name to the advertising data.

Example
^^^^^^^^^^^^

::

   AT+BLENAME="wm-iot-dev"
 
.. _cmd-ESCANPARAM:   
  
:ref:`AT+BLESCANPARAM <Bluetooth_AT>`: Query/Set Bluetooth LE Scan Parameters
---------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**

::

   Query Bluetooth LE scanning parameters

**Command:**

::

   AT+BLESCANPARAM?

**Response:**

::

   +BLESCANPARAM:<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>
   OK

Set command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set Bluetooth LE scanning parameters

**Command:**

::

   AT+BLESCANPARAM=<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>

**Response:**

::

   OK

Parameters
^^^^^^^^^^^^

-  **<"scan_type">**: Scan type
- 0: Passive scanning
- 1: Active scanning
-  **<"own_addr_type">**: Address type
- 0: Public Address
- 1: Random address
- 2: RPA public address
- 3: RPA Random Address
-  **<"filter_policy">**: Scanning and filtering methods
- 0: BLE_SCAN_FILTER_ALLOW_ALL
- 1: BLE_SCAN_FILTER_ALLOW_ONLY_WLST
-  **<"scan_interva">**: Scan interval, the value of this parameter should be greater than or equal to the value of the<scanw_window>parameter. Parameter range: [0x0004, 0x4000]. The scanning interval is the parameter multiplied by 0.625 milliseconds, so the actual scanning interval range is [2.510240] milliseconds.
-  **<"scan_window">**: Scan the window, the value of this parameter should be less than or equal to the value of the<scan_interval>parameter. Parameter range: [0x0004, 0x4000]. The scanning window is multiplied by this parameter by 0.625 milliseconds, so the actual scanning window range is [2.510240] milliseconds.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   // Initialize Bluetooth LE protocol stack
   AT+BLESCANPARAM=0,0,0,100,50

.. _cmd-ESCAN:   
 
:ref:`AT+BLESCAN <Bluetooth_AT>`: Enable Bluetooth LE scanning
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Start/Stop Bluetooth LE Scan
 
**Command:**

::

   AT+BLESCAN=<enable>[,<duration>][,<filter_type>,<filter_param>]

**Response:**

::

   +BLESCAN:<addr>,<rssi>,<data_type>,<data>
   OK
   +BLESCANDONE

Parameters
^^^^^^^^^^^^
   
-  **<"enable">**: 
-  1: Start continuous scanning
-  0: Stop continuous scanning
-  **[<"duration">]**: Scan duration, in seconds.
-  If the stop scan is set, there is no need to set this parameter;
-  If you want to start scanning, you need to set this parameter:
-  When this parameter is set to 0, it indicates the start of continuous scanning;
-  When this parameter is set to a non-zero value, for example, AT+BLESCAN=1,3, it means that the scan will automatically end after 3 seconds.
-  **[<"ilter_type">]**: Filter options
-  1: “MAC”
-  2: “NAME”
-  3: “RSSI”
-  **<"filter_param">**: Filter parameter, representing the MAC address or name of the other device
-  **<"addr">**: Bluetooth LE address
-  **<"<rssi">**: Signal strength
-  **<"<data_type">**: Data type
-  0: “Scannable and connectable advertising”
-  1: “Directed connectable advertising”
-  2: “Non-connectable and scannable advertising”
-  3: “Non-connectable and non-scannable advertising”
-  4: “Additional advertising data requested by an active scanner”
-  5: “Extended advertising, see advertising properties”
-  **<"data">**: Data content

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1    // Initialize Bluetooth LE protocol stack
   AT+BLESCAN=1    // Start scanning
   AT+BLESCAN=0    // Stop scanning
   AT+BLESCAN=1,3,1,"28:6D:C5:96:E6:68"  // Start scanning, filtering type is MAC address
   AT+BLESCAN=1,3,2,"wm-iot-dev"  // Start scanning, filtering type is device name
   AT+BLESCAN=1,3,3,-70 // Start scanning, filtering type is signal strength greater than -70dBm

.. _cmd-ESCANRSPDATA:   
    
:ref:`AT+BLESCANRSPDATA <Bluetooth_AT>`: Set Bluetooth LE Scan Response
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set Bluetooth LE scan response
 
**Command:**

::

   AT+BLESCANRSPDATA=<scan_rsp_data>

**Response:**

::

   OK
  
Parameter
^^^^^^^^^^^^

- **<"scan_rsp_data">**: Scan response data as HEX string. For example, if you want to set the scan response data to “0x04 0x09 0x31 0x32 0x33”, the command is AT+BLESCAN RSPDATA="0409313233".

Note
^^^^^^^^^^^^

- The maximum length of scanned data is 31 bytes
- Scanning response data must comply with the LTV format defined by Bluetooth SIG

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   // Initialize Bluetooth LE protocol stack
   AT+BLESCANRSPDATA="0409313233"

.. _cmd-EADVPARAM:    
   
:ref:`AT+BLEADVPARAM <Bluetooth_AT>`: Query/Set Bluetooth LE Broadcast Parameters
------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query broadcast parameters
 
**Command:**

::

   AT+BLEADVPARAM?

**Response:**

::

   +BLEADVPARAM:<adv_int_min>,<adv_int_max>,<adv_type>,<own_addr_type>,<channel_map>,<adv_filter_policy>,<peer_addr_type>,<peer_addr>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set broadcast parameters
 
**Command:**

::

   AT+BLEADVPARAM=<adv_int_min>,<adv_int_max>,<adv_type>,<own_addr_type>,<channel_map>[,<adv_filter_policy>][,<peer_addr_type>,<peer_addr>]

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<"adv_int_min">**: Minimum broadcast interval. Parameter range: [0x0020,0x4000]. The broadcast interval is equal to multiplying this parameter by 0.625 milliseconds, so the actual minimum broadcast interval range is [201240] milliseconds. The value of this parameter should be less than or equal to the value of the <adv_int_max> parameter.
-  **<"adv_int_max">**: Maximum broadcast interval. Parameter range: [0x0020,0x4000]. The broadcast interval is equal to multiplying this parameter by 0.625 milliseconds, so the actual maximum broadcast interval range is [201240] milliseconds. The value of this parameter should be greater than or equal to the value of the<adv_int_min>parameter.
-  **<"adv_type">**:
-  0: ADV_TYPE_IND
-  1: ADV_TYPE_DIRECT_IND_HIGH
-  2: ADV_TYPE_SCAN_IND
-  3: ADV_TYPE_NONCONN_IND
-  4: ADV_TYPE_DIRECT_IND_LOW

-  **<"own_addr_type">**: Bluetooth LE Address Type
-  0: BLE_ADDR_TYPE_PUBLIC
-  1: BLE_ADDR_TYPE_RANDOM
-  **<"channel_map">**: Broadcast channel
-  1: ADV_CHNL_37
-  2: ADV_CHNL_38
-  4: ADV_CHNL_39
-  7: ADV_CHNL_ALL
-  **<"adv_filter_policy">**: Broadcast Filter Rules
-  0: ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
-  1: ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY
-  2: ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST
-  3:
   ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST
-  **<"peer_addr_type">**: The other party's Bluetooth LE address type
-  0: PUBLIC
-  1: RANDOM
-  **<"peer_addr">**: The other party's Bluetooth LE address

Explanation
^^^^^^^^^^^^

- If peer_addr has never been set, the query result will be all zeros.
- For non-connectable broadcast types, the minimum interval for broadcast parameters should be greater than or equal to 0xA0, which is 160 multiplied by 0.625 equals 100 milliseconds.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   // Initialize Bluetooth LE protocol stack
   AT+BLEADDR=1,"c2:34:45:78:66:89"
   AT+BLEADVPARAM=50,50,0,1,4,0,1,"12:34:45:78:66:88"
   // At this time, the MAC address of the wm iot dev device scanned by the Bluetooth LE client is "c2:34:45:78:66:89"

.. _cmd-EADVDATA:   
 
:ref:`AT+BLEADVDATA <Bluetooth_AT>`: Set Bluetooth LE Broadcast Data
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

  Set broadcast data
 
**Command:**

::

   AT+BLEADVDATA=<adv_data>

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<"adv_data">**: Broadcast data, which is a HEX string. For example, if you want to set the broadcast data to "0x04 0x09 0x31 0x32 0x33", the command is AT+BLEADVDATA="0409313233".


Explanation
^^^^^^^^^^^^

- If the broadcast data has already been set using the command AT+BELEDVDATAEX=<dev_name>,<uuid>,<manufacturer.data>,<inclie_power>, it will be overwritten by the broadcast data set by this command.
- If you want to use this command to modify the device name, it is recommended to execute the AT+BLE command after executing the command to set the device name to the same name.
- If longer broadcast data needs to be set, please call the AT+BLESCAN RSPDATA command to set it.
- Broadcast data must comply with the LTV format defined by Bluetooth SIG.
- The maximum length of broadcast data is 31 bytes.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEADVDATA="0409313233"

.. _cmd-EADVDATAEX:   
 
:ref:`AT+BLEADVDATAEX <Bluetooth_AT>`: Automatically Set Bluetooth LE Broadcast Data
-------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query the parameters of broadcast data
  
**Command:**

::

   AT+BLEADVDATAEX?

**Response:**

::

   +BLEADVDATAEX:<dev_name>,<uuid>,<manufacturer_data>,<include_power>
   OK
   
Set Command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set up broadcast data and start broadcasting
  
**Command:**

::

   AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power>

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<"dev_name">**: String parameter representing device name
-  **<"uuid">**: String parameter.
-  **<"manufacturer_data">**: Manufacturer data, which is a HEX string.
-  **<"include_power">**: If the broadcast data needs to include TX power, this parameter should be set to 1; Otherwise, it is 0.

Explanation
^^^^^^^^^^^^

- If the broadcast data has already been set using the command AT+LEADVDATA=<adv_data>, it will be overwritten by the broadcast data set by this command.
- This command will automatically change the broadcast type previously set using the AT+BLEDVPARAM command to 0.
- The total length of the broadcast data configuration is 31 bytes.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEADVDATAEX="wm-iot-dev","1802","0102030405",1

.. _cmd-EADVSTART:

:ref:`AT+BLEADVSTART <Bluetooth_AT>`: Start Bluetooth LE Broadcast
-----------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Start broadcasting
  
**Command:**

::

   AT+BLEADVSTART

**Response:**

::

    OK

Explanation
^^^^^^^^^^^^

- If the command AT+BLEADVPARAM=<adv_parameter> is not used to set broadcast parameters, the default broadcast parameters will be used.
- If the command AT+LEADVDATA=<adv_data>is not used to set broadcast data, send a packet with a name.
- If the broadcast data has already been set using the command AT+LEADVDATA=<adv_data>, it will be overwritten by the broadcast data set by AT+LEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<inclie_power>. Conversely, if AT+LEADVDATAEX is used first, it will be overwritten by the broadcast data set by AT+LEADVDATA.
- After enabling Bluetooth LE broadcasting, if no Bluetooth LE connection is established, the broadcasting will continue; If a connection is established, the broadcast will automatically end. If the connection is disconnected, it will automatically start broadcasting.
- After enabling connectionable broadcasting, if the main device initiates a connection operation. At this point, the device automatically selects an idle conn index as the connected conn index. When sending AT instructions with conn index from devices (peripherals), use this index value.

Example
^^^^^^^^^^^^^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEADVSTART

.. _cmd-EADVSTOP:
    
:ref:`AT+BLEADVSTOP <Bluetooth_AT>`: Stop Bluetooth LE broadcasting
-----------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Stop broadcasting
     
**Command:**

::

   AT+BLEADVSTOP

**Response:**

::

    OK

Explanation
^^^^^^^^^^^^

- If a Bluetooth LE connection is successfully established after starting the broadcast, the Bluetooth LE broadcast will automatically end without calling this command.


Example
^^^^^^^^^^^^

::
    
   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEADVSTART
   AT+BLEADVSTOP

.. _cmd-ECONN:

:ref:`AT+BLECONN <Bluetooth_AT>`: Establish Bluetooth LE Connection
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query Bluetooth LE connection
  
**Command:**

::

   AT+BLECONN?

**Response:**

::

   +BLECONN:<conn_index>,<remote_address>,<role>
   OK

   If no connection is established, the response will not display the <conn_idex> and <remote_dedress> parameters.

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Establish Bluetooth LE connection
  
**Command:**

::

   AT+BLECONN=<conn_index>,<remote_address>[,<addr_type>,<timeout>]

**Response:**

If the connection is successfully established, it will prompt:

::

   +BLECONN:<conn_index>,<remote_address>
   OK

If the connection fails to establish, it will prompt:

::

   +BLECONN:<conn_index>,-1
   ERROR
   
If the connection fails due to parameter errors or other reasons, it will prompt:

::

   ERROR

Parameter
^^^^^^^^^^^^

-  **<"conn_index">**: Bluetooth LE connection number, range: [0,5].
-  **<"remote_address">**: The address of the other party's Bluetooth LE device.
-  **<"addr_type">**: Broadcast device address type:
-  0: Public Address
-  1: Random Address
-  **<"timeout">**: Connection timeout time, unit: seconds. Range: [3,30], default value is 3 seconds.
-  **<"role">**: Connect roles, CENTRAL or PERIPHERAL.

Explanation
^^^^^^^^^^^^

-  It is recommended to run the AT+BLESCAN command to scan the device before establishing a new connection, ensuring that the target device is in broadcast mode.
-  The maximum connection timeout is 30 seconds.


Attention
^^^^^^^^^^^^^^^^^^
-  For peripheral devices using random addresses, and successfully BONDed with the peripheral device. After the connection is broken, it is **not supported** to initiate a connection operation to this peripheral device again.


Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:0a:c4:09:34:23",0,10

.. _cmd-ECONNPARAM:

:ref:`AT+BLECONNPARAM <Bluetooth_AT>`: Query/Update Bluetooth LE Connection Parameters
-------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query Bluetooth LE connection parameters
  
**Command:**

::

   AT+BLECONNPARAM?

**Response:**

::

   +BLECONNPARAM:<conn_index>,<min_interval>,<max_interval>,<cur_interval>,<latency>,<timeout>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Update Bluetooth LE connection parameters
    
**Command:**

::

   AT+BLECONNPARAM=<conn_index>,<min_interval>,<max_interval>,<latency>,<timeout>

**Response:**

::

   OK

If the setting fails, the following message will be prompted:

::

   +BLECONNPARAM: <conn_index>,-1

Parameter
^^^^^^^^^^^^

-  **<"conn_index">**: Bluetooth LE connection number, range: [0,5].
-  **<"min_interval">**: Minimum connection interval. The value of this parameter should be less than or equal to the value of the<max_interval>parameter. Parameter range: [0x0006,0x0C80]. The connection interval is equal to multiplying this parameter by 1.25 milliseconds, so the actual minimum connection interval range is [7.5,4000] milliseconds.
-  **<"max_interval">**: Maximum connection interval. The value of this parameter should be greater than or equal to the value of the<min_interval>parameter. Parameter range: [0x0006,0x0C80]. The connection interval is equal to multiplying this parameter by 1.25 milliseconds, so the actual maximum connection interval range is [7.5,4000] milliseconds.
-  **<"cur_interval">**: Current connection interval.
-  **<"latency">**: Delay. Parameter range: [0x0000, 0x01F3].
-  **<"timeout">**: Timeout. Parameter range: [0x000A, 0x0C80]. The timeout is equal to multiplying this parameter by 10 milliseconds, so the actual timeout range is [10032000] milliseconds.

Note
^^^^^^^^^^^^

-  For **<"timeout">**, **<"latency">** and **<"max_interval">**, the following conditions must also be met:
	    (timeout x 4)>= (1+latency) x max_interval


Explanation
^^^^^^^^^^^^

- This command requires establishing a connection first, and both the client and server roles support updating connection parameters.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLECONNPARAM=0,12,14,1,500

.. _cmd-EDISCONN:
 
:ref:`AT+BLEDISCONN <Bluetooth_AT>`: Disconnect Bluetooth LE connection
-----------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Disconnect Bluetooth LE connection
  
**Command:**

::

   AT+BLEDISCONN=<conn_index>

**Response:**

::
  
   OK  // Received AT+LEDISCONN command
   +BLEDISCONN:<conn_index>,<remote_address>  // Successfully executed command

Parameter
^^^^^^^^^^^^

-  **<"conn_index">**: Bluetooth LE connection number, range: [0,5].
-  **<"remote_address">**: The address of the other party's Bluetooth LE device.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLEDISCONN=0

.. _cmd-EDATALEN:

:ref:`AT+BLEDATALEN <Bluetooth_AT>`: Set Bluetooth LE Packet Length
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set Bluetooth LE packet length
  
**Command:**

::

  AT+BLEDATALEN=<conn_index>,<pkt_data_len>

**Response:**

::
 
   OK

Parameter
^^^^^^^^^^^^

-  **<"conn_index">**: Bluetooth LE connection number, range: [0,5].
-  **<"pkt_data_len">**: Packet length, range: [0x0001B, 0x00FB].

Explanation
^^^^^^^^^^^^

- A Bluetooth LE connection needs to be established before setting the packet length.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLEDATALEN=0,30

.. _cmd-ECFGMTU:

:ref:`AT+BLECFGMTU <Bluetooth_AT>`: Set Bluetooth LE MTU Length
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query MTU (maximum transmission unit) length
  
**Command:**

::

  AT+BLECFGMTU?

**Response:**

::

   +BLECFGMTU:<conn_index>,<mtu_size>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set the length of MTU
  
**Command:**

::

   AT+BLECFGMTU=<conn_index>,<mtu_size>

**Response:**

::

   OK  // Received this command

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<mtu_size>**: MTU length, unit: bytes, range: [23,517].

Explanation
^^^^^^^^^^^^

.. parsed-literal::

   - This command requires the establishment of a Bluetooth LE connection first.
   - Only supports the client to run this command to set the MTU length.
   - After the device is connected, the firmware automatically attempts to negotiate the MTU value with the peer device using the maximum value of 517.
   - The actual length of MTU needs to be negotiated, and responding with ``OK`` only indicates an attempt to negotiate the MTU length. Therefore, setting the length may not necessarily take effect. It is recommended to call:  :ref:`AT+BLECFGMTU? <cmd-ECFGMTU>` Query the actual MTU length.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLECFGMTU=0,244

.. _cmd-EGATTSSVC:
 
:ref:`AT+BLEGATTSSVC <Bluetooth_AT>`: GATTS Creates/Queries Local Services
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTS (Generic Attributes Server) queries the local service list
  
**Command:**

::

   AT+BLEGATTSSVC?

**Response:**

::

   +BLESVCLIST(local):
   <description>,<uuid>,<handle>,<prop>,<perm>
   ......
   <description>,<uuid>,<handle>,<prop>,<perm>
   OK

Parameter
^^^^^^^^^^^^

-  **<description>**: Description, which may include service, characteristics, or descriptor.
-  **<uuid>**: uuid, a standard uuid defined by Bluetooth SIG or a user-defined uuid, displayed in hexadecimal.
-  **<handle>**: Handle value, displayed in decimal.
-  **<prop>**: Corresponds to the characteristic property. The specific property bit is defined as:

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Bit 7
     - Bit 6
     - Bit 5
     - Bit 4
     - Bit 3
     - Bit 2
     - Bit 1
     - Bit 0

   * - Expansion
     - Certification
     - Indicate
     - Notify
     - Write with response
     - Write without response
     - Read it
     - Broadcasting

-  **<perm>**：The permission corresponding to **<description>** , the specific bit definition of the permission is:

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Bit 8
     - Bit 7
     - Bit 6
     - Bit 5
     - Bit 4
     - Bit 3
     - Bit 2
     - Bit 1
     - Bit 0

   * - Le Security Write
     - Le Security Read
     - Prepare Write
     - Authentication Write
     - Authentication Read
     - Encrypted Write
     - Encrypted Read
     - Write
     - Read

Note
^^^^^^^^^^^^

-   If **<prop>** or **<perm>** is ----, it is invalid. In particular, the **<perm>**	of characteristic is mapped to the subsequence **<perm>** of descriptor. 

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTS (Generic Attributes Server) creates a local service list
  
**Command:**

::

   AT+BLEGATTSSVC=<prim_service_uuid>,<character_uuid>,<property>,<permission>
					   [,<character_uuid>,<property>,<permission>]
					   […]
					   [,<character_uuid>,<property>,<permission>]


**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-   **<prim_service_uuid>**: Create a UUID for the Primary server, input in hexadecimal format.
-   **<character_uuid>**: The characteristic uuid is entered in hexadecimal format.
-   **<property>**: The attribute value of characterization is entered in hexadecimal format. For details, please refer to the table above for the bit definition of :ref:`property <cmd-EGATTSSVC>` .
-   **<permission>**: The permission value for characterization, entered in hexadecimal format. Specifically, for the bit definition of  :ref:`permission <cmd-EGATTSSVC>` , please refer to the table above.


Note
^^^^^^^^^^^^

-   When a characteristic's property is configured for write operations without acknowledgment, the permission cannot be configured as encrypted write, authorized write, or Le security write (unless the connection is already in encryption mode).
-   Firmware does not verify the property and permission, user should ensure to input the reasonable values. 

Explanation
^^^^^^^^^^^^

-   The instruction only supports creating a basic list of services, and a maximum list of 12 services can be dynamically created.
-   The instruction allows for the creation of a list of characteristics, with each service supporting a maximum of 6 characteristics.
-   When creating via the instruction, if a characteristic's property is set to indicate or notify, a descriptor with UUID 2902 is automatically appended; otherwise, a descriptor with UUID 2901 is added.
-   The created list of services takes effect immediately. If a connection has already been established with the other device during creation, the other device can obtain this list of services by refreshing it.
-   The correspondence between the property and permission configurations of characteristics and common rules are described as follows:
-   1) If a characteristic property is read, the corresponding permission must include any read permission.
-   2) If a characteristic property is write, the corresponding permission must include any write permission.
-   3) If a characteristic property is indicate, the corresponding permission must include both read and write permissions.
-   4) When the setting permission to Le security read and write, authorized read and write or encrypted read and write must also be configured.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEGATTSSVC=FFF0,FFF1,02,01 //Create FFF1 characteristic with read-only property and read permission
   AT+BLEGATTSSVC=FFF0,FFF1,06,03 //Create FFF1 characteristic with read and write (no acknowledgment) properties, and read and write permissions
   AT+BLEGATTSSVC=FFF1,FFF2,10,03 //Create FFF4 characteristic with Notify property, and read and write permissions
   AT+BLEGATTSSVC=FFF3,FFF4,10,0C //Create FFF4 characteristic with Notify property, and encrypted read and encrypted write permissions
   AT+BLEGATTSSVC=FFF5,FFF6,02,01,FFF7,10,18C //Create FFF4 characteristic with Notify property, encrypted read and encrypted write permissions, and Le security encryption level

.. _cmd-EGATTSSVCRMV:
 
:ref:`AT+BLEGATTSSVCRMV <Bluetooth_AT>`: GATTS Delete Local Service
-----------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTS Delete Local Dynamic Creation Service
  
**Command:**

::

   AT+BLEGATTSSVCRMV=<uuid>

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-   **<uuid>**: Primary service UUID when creating a service.

Explanation
^^^^^^^^^^^^

-   After the service is deleted, it takes effect in real time. If a connection has been created, the main device can view it by refreshing the service list.

Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEGATTSSVC=FFF0,FFF1,10,01 //Dynamically create a service list for FFF0
   AT+BLEGATTSSVCRMV=FFF0 //Delete the dynamically created FFF0 service list

.. _cmd-EGATTSNTFY:

:ref:`AT+BLEGATTSNTFY <Bluetooth_AT>`: Server Notifies Service Feature Values to Client
---------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Server notifies service feature values to client
  
**Command:**

::

   AT+BLEGATTSNTFY=<conn_index>,<handle>,<length>

**Response:**

::

   >
  
The symbol > indicates that the AT is ready to receive serial data. At this point, you can input data, and when the data length reaches the value of the parameter <length> , perform the notify operation.

If the data transmission is successful, it will prompt:

::

   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<handle>**: The handle value of the service feature, which can be queried using AT+BLEGATTSSVC? .
-  **<length>**: Data length, maximum length: (:ref:`MTU <cmd-ECFGMTU>` - 3) .

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1      //Initialize Bluetooth LE protocol stack 
   AT+BLEGATTSSVC=FFF0,FFF1,10,03 //Create characteristics that support notify
   AT+BLEADVSTART    //To start broadcasting, after the client connects, it is necessary to configure :ref: `Subscribe to <cmd-EGATTCSUBS>`
   AT+BLEGATTSSVC?   //Query the handle for sending notify
   // For example, using handle value 21 to send a 4-byte data through notify, use the following command:  AT+BLEGATTSNTFY=0,21,4. After the prompt for the ">" symbol后, enter the 4-byte data, such as"1234", and the data will be automatically transmitted


.. _cmd-EGATTSIND:

:ref:`AT+BLEGATTSIND <Bluetooth_AT>`: Server indicates Service Characteristic Values to Clients
-------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Server indicates service characteristic values to clients
  
**Command:**

::

   AT+BLEGATTSIND=<conn_index>,<handle>,<length>

**Response:**

::

   >

The symbol > indicates that the AT is ready to receive serial data. At this point, you can input data, and when the data length reaches the value of the parameter <length> , perform the indicate operation.

If the data transmission is successful, it will prompt:

::

   OK
   
Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<handle>**: The handle value of the service feature, which can be queried using AT+BLEGATTSSVC? .
-  **<length>**: Data length, maximum length:( :ref:`MTU <cmd-ECFGMTU>` - 3).

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEGATTSSRV=FFF0,FFF1,20,03 //Create a characteristic that supports indication, with uuid FFF1
   AT+BLEADVSTART //Start broadcasting, when the client connects, it is necessary to configure the receive indication
   AT+BLEGATTSSVC? // Query the feature handle values that the client can receive for indication
   // For example, using handle value 3 to send 4-byte data, the command is as follows:
   AT+BLEGATTSIND=0,3,4
   // After the ">" symbol is prompted, enter 4-byte data, such as "1234". Then the data is automatically transmitted


.. _cmd-EGATTSSETATTR:

:ref:`AT+BLEGATTSSETATTR <Bluetooth_AT>`: GATTS Sets Service Characteristic Values
-------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTS sets service feature values or descriptor values
    
**Command:**

::

   AT+BLEGATTSSETATTR=<handle>,<length>
    
**Response:**

::

    >

The symbol > indicates that the AT is ready to receive serial data. At this point, you can input the data, and when the data length reaches the value of the parameter <length> , perform the setting operation.

If the data transmission is successful, it will prompt:

::

   OK

Parameter
^^^^^^^^^^^^

-  **<handle>**: Handle value, can run AT+BLEGATTSSVC? query.
-  **<length>**: Data length, range [1,512].

Explanation
^^^^^^^^^^^^

-  If the value of the <length> parameter is greater than the maximum supported length, the setting will fail.

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLEGATTSSVC=FFF0,FFF1,06,03 //Create characteristics that support reading and writing
   AT+BLEGATTSSVC?                //Query handle value
   //For example, to write 3-byte data to handle value 5, the command is as follows:
   AT+BLEGATTSSETATTR=5,3
   //After the ">" symbol is prompted, enter 3-byte data, such as "123", and then set the start


.. _cmd-EGATTCSVC:

:ref:`AT+BLEGATTCSVC <Bluetooth_AT>`: GATTC Discovery Service List
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTC (Generic Attributes Client) discovers server-side service list
    
**Command:**

::

   AT+BLEGATTCSVC=<conn_index>
    
**Response:**

::

   +BLESVCLIST(remote):<conn_index>,
    <attribute description>,<uuid>,<handle>,<prop>
    ……
    <attribute description>,<uuid>,<handle>,<prop>
   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<attribute description>**: Attribute description, which may include primary service, secondary service, characteristic, descriptor.
-  **<uuid>**: UUID of attribute, displayed in hexadecimal.
-  **<handle>**: The handle value of the attribute, displayed in hexadecimal.
-  **<prop>**: Attribute:  :ref:`property <cmd-EGATTSSVC>` value, displayed in hexadecimal.

Explanation
^^^^^^^^^^^^

- To use this command, a Bluetooth LE connection needs to be established first.
- This command will sequentially query the service, characteristic, and descriptor information of the server. If there are many services provided by the server, it will take a longer time.

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0


.. _cmd-EGATTCRD:

:ref:`AT+BLEGATTCRD <Bluetooth_AT>`: GATTC Reads Service Characteristic Values
---------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTC reads service feature values or descriptor values
    
**Command:**

::

   AT+BLEGATTCRD=<conn_index>,<handle>
    
**Response:**

::

   +BLEGATTCRD:<conn_index>,<len>,<value>
   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<handle>**: Service feature handle value, which can be queried by AT+BLEGATTCSVC=<conn_idex>.
-  **<len>**: Data length.
-  **<value>**: Data content. For example, if the response is +BLEGATTCRD:0,25,2,0123, it means that the data length is 2 and the content is 0x01 0x23.

Explanation
^^^^^^^^^^^^^

- To use this command, a Bluetooth LE connection needs to be established first.
- If the target service feature does not support read operations, return 'ERROR'.

Example
^^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCRD=0,25 // Based on the query result of the previous command, specify the handle to query

.. _cmd-EGATTCWR:

:ref:`AT+BLEGATTCWR <Bluetooth_AT>`:GATTC Writes Service Characteristic Values
---------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTC writes service feature values or descriptor values
  
**Command:**

::

   AT+BLEGATTCWR=<conn_index>,<handle>,<length>
   
**Response：**

::

  >  

The symbol > indicates that the AT is ready to receive serial data. At this point, you can input data and perform a write operation when the data length reaches the value of the parameter <length> .

If the data transmission is successful, it will prompt:

::

   OK
    
Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<handle>**: Service handle, can run AT+BLEGATTCSVC=<conn_index> query.
-  **<char_index>**: Service feature number, can be queried by AT+BLEGATTCCHAR=<conn_index>,<srv_index> .
-  **<length>**: Data length. Maximum length: ( :ref:`MTU <cmd-ECFGMTU>` - 3) .

Explanation
^^^^^^^^^^^^

- To use this command, a Bluetooth LE connection needs to be established first.
- If the target service feature does not support write operations, return 'ERROR'.

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCWR=0,25,6
   // After the ">" symbol is prompted, enter 6 bytes of data, such as "123456". Then start writing

.. _cmd-EGATTCSUBS:

:ref:`AT+BLEGATTCSUBS <Bluetooth_AT>`: GATTC Subscription Service
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   GATTC subscription or cancellation of server's Indication or Notification function
  
**Command:**

::

   AT+BLEGATTCSUBS=<conn_index>,<handle>,<value>
   
**Response：**

::

  OK 
    
Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<handle>**: Service handle, can run AT+BLEGATTCSVC=<conn_index> query.
-  **<value>** 0 Cancel; 1 subscription.

Explanation
^^^^^^^^^^^^

- To use this command, a Bluetooth LE connection needs to be established first.
- If the target service feature does not support indication or notification operations, return "ERROR".
- After successful subscription, receive and display the data format sent by the other party as follows:

::

  +BLEGATTCNTFY:0,25,4,12232123 //The connection with index 0 received 4-byte data from the other party on handle 25. The data content is displayed in hexadecimal format as 12232123, which is 0x12, 0x23, 0x21, 0x23

Example
^^^^^^^^^^^^

:: 

   AT+BLEINIT=1   //Initialize Bluetooth LE protocol stack
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCSUBS=0,25,1

.. _cmd-ESECPARAM:

:ref:`AT+BLESECPARAM <Bluetooth_AT>`: Query/Set Bluetooth LE Encryption Parameters
---------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query about Bluetooth LE SMP encryption parameters
  
**Command:**

::

   AT+BLESECPARAM?
    
**Response:**

::

   +BLESECPARAM:<auth_req>,<iocap>,<enc_key_size>,<init_key>,<rsp_key>,<auth_option>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set Bluetooth LE SMP encryption parameters
  
**Command:**

::

   AT+BLESECPARAM=<auth_req>,<iocap>,<enc_key_size>,<init_key>,<rsp_key>[,<auth_option>]
    
**Response:**

::

 OK

Parameter
^^^^^^^^^^^^

-  **<auth_req>**: Authentication request.
-  0: NO_BOND
-  1: BOND
-  4: MITM
-  8: SC_ONLY
-  9: SC_BOND
-  12: SC_MITM
-  13: SC_MITM_BOND
-  **<iocap>**: Input/output capability.
-  0: DisplayOnly
-  1: DisplayYesNo
-  2: KeyboardOnly
-  3: NoInputNoOutput
-  4: Keyboard display
-  **<enc_key_size>**: length of encryption key. Parameter range: [7,16]. Unit: Byte.。
-  **<init_key>**: An initial key composed of multiple bits.
-  **<rsp_key>**: A response key composed of multiple bits.
-  **<auth_option>**: Security authentication options:
-  0: Automatically select security level;
-  1: If the previously set security level cannot be met, the connection will be disconnected.

Explanation
^^^^^^^^^^^^

-  The bit combination pattern of the <init_key> and <rsp_key> parameters is as follows:
-  Bit0: Encryption key used for exchanging initial keys and response keys;
-  Bit1: IRK key used for exchanging initial and response keys;
-  Bit2: CSRK key used for exchanging initial and response keys;
-  Bit3: Link key used for exchanging initial and response keys (only for Bluetooth LE and BR/EDR coexistence modes).

Example
^^^^^^^^^^^^

:: 

   AT+BLESECPARAM=1,4,16,3,3,0

.. _cmd-EENC:

:ref:`AT+BLEENC <Bluetooth_AT>`: Initiate Bluetooth LE Encryption Request
-----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

  Initiate pairing request
  
**Command:**

::

  AT+BLEENC=<conn_index>,<sec_act>
    
**Response:**

::

  OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<sec_act>**: 
-  0: SEC_NONE;
-  1: SEC_ENCRYPT_NO_MITM;
-  2: SEC_ENCRYPT_MITM;
-  3: SEC_ENCRYPT_SC。
 
Explanation
^^^^^^^^^^^^

-  Before using this command, please set security parameters and establish a connection with the other device.

Example
^^^^^^^^^^^^

:: 

   AT+RESTORE
   AT+BLEINIT=1
   AT+BLESECPARAM=1,0,16,3,3
   AT+BLESETKEY=123456
   AT+BLEADVSTART
   // Using Bluetooth LE debugging app as client to establish Bluetooth LE connection with  wm-iot-dev device
   AT+BLEENC=0,3

.. _cmd-EENCRSP:

:ref:`AT+BLEENCRSP <Bluetooth_AT>`: Reply to Pairing Requests Initiated by the Peer Device
---------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Reply to pairing requests initiated by the peer device
  
**Command:**

::

   AT+BLEENCRSP=<conn_index>,<accept>
    
**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<accept>**:
-  0: Refuse;
-  1: Accept.

Explanation
^^^^^^^^^^^^

-  After using this command, AT will output the pairing result after the pairing request process is completed.


::

   +BLEAUTHCMPL:<conn_index>,<enc_result>
   
-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<enc_result>**:
-  0: Encryption pairing successful;
-  1: Encryption pairing failed.


Example
^^^^^^^^^^^^

:: 

   AT+BLEENCRSP=0,1

.. _cmd-EKEYREPLY:

:ref:`AT+BLEKEYREPLY <Bluetooth_AT>`: Reply the Key to the Other Party's Device
---------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Reply pairing key
  
**Command:**

::

   AT+BLEKEYREPLY=<conn_index>,<key>
    
**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<key>**: Pairing key.

Example
^^^^^^^^^^^^

:: 

   AT+BLEKEYREPLY=0,649784

.. _cmd-ECONFREPLY:

:ref:`AT+BLECONFREPLY <Bluetooth_AT>`: Reply to The Other Party's Device with A Confirmation Result (Traditional Connection Stage)
--------------------------------------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Reply to pairing results
  
**Command:**

::

   AT+BLECONFREPLY=<conn_index>,<confirm>
    
**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<conn_index>**: Bluetooth LE connection number, range: [0,5].
-  **<confirm>**:
-  0: No
-  1: Yes

Example
^^^^^^^^^^^^

:: 

   AT+BLECONFREPLY=0,1

.. _cmd-EENCDEV:

:ref:`AT+BLEENCDEV <Bluetooth_AT>`: Query List Of Bound Bluetooth LE Encryption devices
----------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query about the list of bound Bluetooth LE Encryption Devices
  
**Command:**

::

   AT+BLEENCDEV?
    
**Response:**

::

   +BLEENCDEV:<enc_dev_index>,<mac_address>
   OK

Parameter
^^^^^^^^^^^^

-  **<enc_dev_index>**: The connection number of the bound device. This parameter may not necessarily be equal to the conn_index parameter in the Bluetooth LE connection list queried by the command AT+BLECONN. Scope: [0,7].
-  **<mac_address>**: MAC address.

Explanation
^^^^^^^^^^^^

-  wm-iot-dev allows binding up to 8 devices. If the number of bound devices exceeds 8, the newly bound device information will overwrite the previous device information in order of binding from 0 to 7.

Example
^^^^^^^^^^^^

::

   AT+BLEENCDEV?

.. _cmd-EENCCLEAR:

:ref:`AT+BLEENCCLEAR <Bluetooth_AT>`:  Clear List of Bluetooth LE Encrypted Devices
-------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Remove a device with a certain connection number from the security database list
  
**Command:**

::

   AT+BLEENCCLEAR=<enc_dev_index>
    
**Response:**

::

  OK

Execute command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Delete all devices from the security database
  
**Command:**

::

   AT+BLEENCCLEAR

**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<enc_dev_index>**: The connection number of the device that has been bound.

Example
^^^^^^^^^^^^

::

   AT+BLEENCCLEAR

.. _cmd-ESETKEY:

:ref:`AT+BLESETKEY <Bluetooth_AT>`: Set Bluetooth LE Static Pairing Key
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Query the static pairing key of Bluetooth LE, if not set, AT returns -1
  
**Command:**

::

   AT+BLESETKEY?
    
**Response:**

::

   +BLESETKEY:<static_key>
   OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

  Set a Bluetooth LE static pairing key for all Bluetooth LE connections
  
**Command:**

::

   AT+BLESETKEY=<static_key>
    
**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<static_key>**: Bluetooth LE static pairing key.

Example
^^^^^^^^^^^^

::

   AT+BLESETKEY=123456

.. _cmd-ESPPSCFG:   
 
:ref:`AT+BLESPPSCFG <Bluetooth_AT>`: Configure Bluetooth LE SPP Server Transparent transmission parameters
---------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set parameter information for Bluetooth LE SPP transmission function on the server side
 
**Command:**

::

   AT+BLESPPSCFG=<svc_uuid>,<tx_uuid>,<tx_prop>,<tx_perm>,<rx_uuid>,<rx_prop>,<rx_perm>


**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<"svc_uuid">**: Transparent transmission of basic service uuid.
-  **<"tx_uuid">**: The uuid of the data characteristic sent by the transparent transmission service.
-  **<"tx_prop">**: The attribute of the characteristic of the data sent by the transparent transmission service is read in hexadecimal format.
-  **<"tx_perm">**: The permission for the transparent transmission service to send data characteristics, read in hexadecimal format.
-  **<"rx_uuid">**: The uuid of the data characteristic received by the transparent transmission service.
-  **<"rx_prop">**: The transparent transmission service receives the characteristic attribute of the data and reads it in hexadecimal format.
-  **<"rx_perm">**: The transparent transmission service has the permission to receive data characteristics and read them in hexadecimal format.

Explanation
^^^^^^^^^^^^

-  The transparent transmission service sends data using notify or indicate methods, so tx_prop must include a configuration of 10 or 20, i.e. 0x10, notify method or 0x20, indicate method.
-  The transparent transmission service receives data, that is, the client sends data to the server, using response writing or writing without response. Therefore, rx_prop must be configured as 04 or 08, i.e. 0x04, without response writing or 0x08, with response writing.
-  After the instruction is executed, the broadcast function will be enabled, and the broadcast data fields include: device name and service UUID field svc_uuid. The client can scan and initiate connection operations based on this field.
-  After the client connects to the SPP server and subscribes to the indicate or notify function for sending feature values, the console interface will print +BLESPP READY and wait for the user to enable transparent transmission. 


Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1    // Initialize Bluetooth LE protocol stack
   AT+BLESPPSCFG=FFF0,FFF1,10,3,FFF2,4,2 //Configure SPP server with service UUID FFF0,, use FFF1 feature value to send data, and send by notify. Using FFF2 feature values to receive data, the other party sends data without response
   
.. _cmd-ESPPCCFG:   
 
:ref:`AT+BLESPPCCFG <Bluetooth_AT>`: Configure Bluetooth LE SPP Client Transparent Transmission Parameters
---------------------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Set parameter information for Bluetooth LE SPP transmission function client
 
**Command:**

::

   AT+BLESPPCCFG=<filter_type>,<filter_value>,<svc_uuid>,<tx_uuid>,<rx_uuid>


**Response:**

::

   OK

Parameters
^^^^^^^^^^^^

-  **<"filter_type">**: Range [0,2], specify scan filter type
-   1) "NAME"
-   2) "MAC"
-   3) "UUID"
-  **<"filter_value">**: The content corresponding to the filter type.
-  **<"svc_uuid">**: The server enables the transparent transmission service uuid.
-  **<"tx_uuid">**: The server opens the transparent transmission service to send the characteristic value UUID of the data.
-  **<"rx_uuid">**: The server opens the transparent transmission service to receive the characteristic value UUID of the data.

Explanation
^^^^^^^^^^^^

-  After executing this command, enable the scanning function. When the scanned device meets the filtering criteria, initiate the connection operation. After a successful connection, check whether the other party provides basic transparent transmission services for svc_uuid, and whether the transparent transmission service provides the special value sending function for tx_uuid and the special value receiving function for rx_uuid. If all the above conditions are met, the client will subscribe to the notify or indicate function for sending tx_uuid feature values. After successful subscription, the console will print +BLESPP READY and wait for the user to start transparent transmission.


Example
^^^^^^^^^^^^

::

   AT+BLEINIT=1    // Initialize Bluetooth LE protocol stack
   AT+BLESPPCCFG=0,”WM-AT”,FFF0,FFF2,FFF1 //Configure SPP client parameters, filter type as name, name as "WM-AT", server-side SPP service UUID as FFF0, FFF2 client sends data UUID, corresponding to server-side received data UUID, receive data special value as FFF1, corresponding to server-side sent data UUID
   
.. _cmd-ESPPSTART:   
 
:ref:`AT+BLESPPSTART <Bluetooth_AT>`: Enable Bluetooth LE SPP Transparent Transmission Function
--------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Activate Bluetooth LE SPP transmission function
 
**Command:**

::

   AT+BLESPPSTART


**Response:**

::

   >


Explanation
^^^^^^^^^^^^

-   For SPP clients or servers, when the system prompts +BLESPP READY, use this command to enable the transparent transmission function. Otherwise, it will fail to enable and return ERROR
-   In transparent mode, when the system receives a packet containing only++++, the device returns to normal command mode and the console prints+BLESPP EXIT MANUAL. Please wait at least one second before sending the next AT command

Note
^^^^^^^^^^^^

-   If the transparent transmission server is configured with encryption operations, the pairing operation must be completed using the AT+BLEENC command before enabling the transparent transmission function(after entering transparent transmission mode, the console does not respond to AT commands, resulting in the inability to enter the pairing password and pairing confirmation message during the pairing process)

Example
^^^^^^^^^^^^

::

   AT+BLESPPSTART
   
.. _cmd-ESPPSTOP:   
 
:ref:`AT+BLESPPSTOP <Bluetooth_AT>`: Disable Bluetooth LE SPP Transparent Transmission Function
--------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Disable Bluetooth LE SPP transmission function
 
**Command:**

::

   AT+BLESPPSTOP


**Response:**

::

   OK


Explanation
^^^^^^^^^^^^

-   Note that during the transparent transmission period, the system does not respond to AT commands. Before enabling this command, the user must exit transparent mode.
-   After the instruction is executed, if a connection has already been established with the client, the other party's connection will be disconnected;If not established, the scanning function will be turned off for the client and the broadcasting function will be turned off for the server.

Note
^^^^^^^^^^^^

-   Turning off transparent commands will not log out of dynamically created SPP services on the server side. If you want to cancel the SPP service, you can use the command :ref:`Delete Local Service <cmd-EGATTSSVCRMV>` to delete it.   

Example
^^^^^^^^^^^^

::

   AT+BLESPPSTOP
   
.. _cmd-EWIFIPROV:   
 
:ref:`AT+BLEWIFIPROV <Bluetooth_AT>`: Query/Set Bluetooth LE WIFI Network Connection Function
-------------------------------------------------------------------------------------------------
Query Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

   Check if the WiFi function assisted by Bluetooth LE is enabled
 
**Command:**

::

   AT+BLEWIFIPROV?


**Response:**

::

   +BLEWIFIPROV=<enable>

Parameter
^^^^^^^^^^^^

-  **<"enable">**: Enable or disable WiFi Prov function


Set Command
^^^^^^^^^^^^^^^^^^^^^^^^

**Fucntion:**
 
::

  Enable/Disable the function of connecting to a router or soft AP via Bluetooth LE-assisted WiFi
 
**Command:**

::

   AT+BLEWIFIPROV=<enalbe>


**Response:**

::

   OK

Parameter
^^^^^^^^^^^^

-  **<"enable">**：Enable or Disable WiFi Provisioning (Prov) Function
-   1) Enable WiFi Prov function
-   2) Disable WiFi Prov function

Example
^^^^^^^^^^^^

::

   AT+BLEWIFIPROV=1
   
.. _cmd-EBTTEST:   
 
:ref:`AT+BTTEST <Bluetooth_AT>`：Enable/Exit Bluetooth Test Mode
-----------------------------------------------------------------------------
Exit Command
^^^^^^^^^^^^^^

**Fucntion：**
 
::

   Exit Bluetooth test mode
 
**Command：**

::

   AT+BTTEST=0


**Response：**

::

   OK


Enable Bluetooth Test Mode
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function：**
 
::

   Set Bluetooth test mode. After successful setting, the corresponding serial port will respond to HCI commands.
 
**Command：**

::

   AT+BTTEST=<1>,<uart_idx>


**Response：**

::

   OK

Parameter
^^^^^^^^^^

-  **<"uart_idx">**：Specifies which serial port will respond to HCI commands.
-   1) The serial port index ranges from 1 to 4, corresponding to Serial Port 1 to Serial Port 4.
-   2) Only default serial port configuration is supported, with a baud rate of 115200, no parity, 1 stop bit, and 8 data bits.

Example
^^^^^^^^

::

   AT+BTTEST=1,1   