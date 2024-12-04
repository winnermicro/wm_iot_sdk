
.. _TCPIP-AT:
.. |Equipment-Name| replace:: w800

****************************
TCP/IP AT Command set
****************************

 
-  :ref:`AT+CIPV6 <cmd-IPV6>`                      : Enable/Disable IPv6 Network (IPv6)
-  :ref:`AT+CIPSTATE <cmd-IPSTATE>`                : Query TCP/UDP/SSL connection information
-  :ref:`AT+CIPDOMAIN <cmd-DOMAIN>`                : Domain name Resolution
-  :ref:`AT+CIPSTART <cmd-START>`                  : Establish TCP connection, UDP Transmission, or SSL connection
-  :ref:`AT+CIPSTARTEX <cmd-STARTEX>`              : Establish TCP connection, UDP Transmission, or SSL connections with automatically assigned IDs
-  :ref:`[Data mode ony] +++ <cmd-PLUS>`           : Exit :term:`data mode`
-  :ref:`AT+CIPSEND <cmd-SEND>`                    : Send data in either :term:`Normal transmission mode` or Wi-Fi :term:`Passthrough Mode` 
-  :ref:`AT+CIPSENDL(Not supported)<cmd-SENDL>`    : Parallel transmission of long data in :term:`Normal transmission mode`
-  :ref:`AT+CIPSENDLCFG(Not supported) <cmd-SENDLCFG>` : Set :ref:`AT+CIPSENDL <cmd-SENDL>` Properties of command
-  :ref:`AT+CIPSENDEX <cmd-SENDEX>`                : Send data using an extended method in :term:`Normal transmission mode` 
-  :ref:`AT+CIPCLOSE <cmd-CLOSE>`                  : Close TCP/UDP/SSL connection
-  :ref:`AT+CIFSR <cmd-IFSR>`                      : Query local IP address and MAC address
-  :ref:`AT+CIPMUX <cmd-MUX>`                      : Enable/Disable multiple connection mode
-  :ref:`AT+CIPSERVER <cmd-SERVER>`                : Establish/Close TCP or SSL server
-  :ref:`AT+CIPSERVERMAXCONN <cmd-SERVERMAX>`      : Query/Set the maximum number of connections allowed by the server
-  :ref:`AT+CIPMODE <cmd-IPMODE>`                  : Query/Set transmission mode
-  :ref:`AT+CIPSTO <cmd-STO>`                      : Query/Set local TCP server timeout
-  :ref:`AT+CIPSNTPCFG <cmd-SNTPCFG>`              : Query/Set time zone and SNTP server
-  :ref:`AT+CIPSNTPTIME <cmd-SNTPT>`               : Query SNTP time
-  :ref:`AT+CIPSNTPINTV <cmd-SNTPINTV>`            : Query/set SNTP time synchronization interval
-  :ref:`AT+CIPFWVER (Not supported) <cmd-FWVER>`  : Query the AT firmware version already on the server
-  :ref:`AT+CIUPDATE (Not supported) <cmd-UPDATE>` : Upgrade firmware via Wi-Fi
-  :ref:`AT+CIPDINFO <cmd-IPDINFO>`                : Set +IPD message details
-  :ref:`AT+CIPSSLCCONF <cmd-SSLCCONF>`            : Query/Set SSL client configuration
-  :ref:`AT+CIPSSLCCN <cmd-SSLCCN>`                : Query/Set SSL client's common name
-  :ref:`AT+CIPSSLCSNI <cmd-SSLCSNI>`              : Query/Set SSL client's SNI
-  :ref:`AT+CIPSSLCALPN <cmd-SSLCALPN>`            : Query/Set SSL client's ALPN
-  :ref:`AT+CIPSSLCPSK <cmd-SSLCPSK>`              : Query/Set SSL client's PSK
-  :ref:`AT+CIPRECONNINTV <cmd-AUTOCONNINT>`       : Query/Set TCP/UDP/SSL reconnection interval under Wi-Fi :term:`Passthrough Mode`
-  :ref:`AT+CIPRECVMODE <cmd-CIPRECVMODE>`         : Query/Set socket receive mode
-  :ref:`AT+CIPRECVDATA <cmd-CIPRECVDATA>`         : Get socket data in passive receive mode
-  :ref:`AT+CIPRECVLEN <cmd-CIPRECVLEN>`           : Query the length of socket data in passive receive mode
-  :ref:`AT+PING <cmd-CIPPING>`                    : Ping the remote host
-  :ref:`AT+CIPDNS <cmd-DNS>`                      : Query/Set DNS server information
-  :ref:`AT+CIPTCPOPT <cmd-TCPOPT>`                : Query/Set socket options




.. _cmd-IPV6:

:ref:`AT+CIPV6 <TCPIP-AT>`: Enable/Disable IPv6 Network (IPv6)
------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^

**Function:**

Check if the IPv6 network is enabled

**Command:**

::

    AT+CIPV6?

**Response:**

::

    +CIPV6:<enable>

    OK

Parameters
^^^^^^^^^^^^

-  **<enable>**: IPv6 network enabled status. Default value: 1

   -  0: Disable IPv6 network
   -  1: Enable IPv6 network

Description
"""""""""""""""
- When :ref:AT+SYSSTORE=1 <cmd-SYSSTORE> is executed, the configuration changes will be saved in the NVS (Non-Volatile Storage) area.
- Before using upper layer applications based on IPv6 networks, it is necessary to enable the IPv6 network first. (For example, using TCP/UDP/SSL/PING/DNS over IPv6 networks, also known as TCP6/UDP6/SSL6/PING6/DNS6 or TCPv6/UDPv6/SSLv6/PINGv6/DNSv6)

.. _cmd-IPSTATE:

:ref:`AT+CIPSTATE <TCPIP-AT>`: Query TCP/UDP/SSL Connection Information
-----------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIPSTATE?

**Response:**

When there is a connection, AT returns:

::

    +CIPSTATE:<link ID>,<"type">,<"remote IP">,<remote port>,<local port>,<tetype>
    OK

When there is no connection, AT returns:

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**    : Network connection ID (0~4), used for multi connection.
-  **<"type">**     : String parameter representing the type of transmission: "TCP", "UDP", "SSL", "TCPv6", "UDPv6" or "SSLv6"
-  **<"remote IP">**: A string parameter showing the remote IPv4 or IPv6 address
-  **<remote port>**: Remote port value
-  **<local port>** : |Equipment-Name| Local port value
-  **<tetype>**     :

   -  0: |Equipment-Name| Equipment as a client
   -  1: |Equipment-Name| Equipment acts as a server


.. _cmd-DOMAIN:

:ref:`AT+CIPDOMAIN <TCPIP-AT>`: Domain Name Resolution
------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPDOMAIN=<"domain name">[,<ip network>]

**Response:**

::

    +CIPDOMAIN:<"IP address">
    OK

Parameters
^^^^^^^^^^^^

-  **<"domain name">**: Domain name to be resolved
-  **<ip network>**   : Preferred IP network. Default value: 1

   - 1: Preferred resolution to IPv4 address
   - 2: Only parse to IPv4 addresses
   - 3: Only parse to IPv6 address

-  **<"IP address">** IP address resolved

Example
^^^^^^^^

::

    AT+CWMODE=1                       // Set station mode
    AT+CWJAP="SSID","password"        // Connect to the network
    AT+CIPDOMAIN="www.winnermicro.com"   // Domain name analysis

    // Domain name analysis, preferably resolved to IPv4 address
    AT+CIPDOMAIN="ds.test-ipv6.com",1
    
    // Domain name analysis, only resolves to IPv4 addresses
    AT+CIPDOMAIN="www.winnermicro.com",2

    // Domain name analysis, only resolves to IPv6 addresses
    AT+CIPDOMAIN="ipv6.test-ipv6.com",3

.. _cmd-START:

:ref:`AT+CIPSTART <TCPIP-AT>`: Establish TCP Connection, UDP Transmission, or SSL Connection
------------------------------------------------------------------------------------------------------

Establish TCP connection
^^^^^^^^^^^^^^^^^^^^^^^^^^

Set Command
""""""""""""""

**Command:**

::

    // Single connection (AT+CIPMUX=0):
    AT+CIPSTART=<"type">,<"remote host">,<remote port>[,<keep_alive>][,<"local IP">]

    // Multi connection (AT+CIPMUX=1):
    AT+CIPSTART=<link ID>,<"type">,<"remote host">,<remote port>[,<keep_alive>][,<"local IP">]

**Response:**

In single connection mode, return:

::

    CONNECT
    OK

In multi connection mode, return:

::

    <link ID>,CONNECT
    OK

Parameters
""""""""""""""""

-  **<link ID>**      : Network connection ID (0~4), used for multi connection situations. The range of this parameter depends on two configuration items in ``menuconfig`` . One is the configuration item ``AT_SOCKET_MAX_CONN_NUM`` in the ``AT`` component, with a default value of 5. The other is the configuration item ``LWIP_MAX_SOCKETS`` in the ``LWIP`` component, with a default value of 10. To modify the range of this parameter, you need to adjust the value of the configuration item  ``AT_SOCKET_MAX_CONN_NUM`` and ensure that it is not greater than the value of ``LWIP_MAX_SOCKETS`` . (Please refer to: doc: for more information.)
-  **<"type">**       : A string parameter representing the network connection type, "TCP" or "TCPv6".
-  **<"remote host">**: A string parameter that represents a remote IPv4 address, IPv6 address, or domain name. The maximum length is 64 bytes. If you need to use a domain name with a length exceeding 64 bytes, please use the :ref:`AT+CIPDOMAIN<cmd-DOMAIN>` command to obtain the IP address corresponding to the domain name, and then use the IP address to establish a connection.
-  **<remote port>**  : Remote port value.
-  **<keep_alive>**   : Configure the ``SO_KEEPALIVE`` option for sockets (refer to `SO_KEEPALIVE introduce <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`_), unit: seconds.

  - Scope: [0,7200].

    - 0: Disable keep alive function; (Default)
    - 1 ~ 7200: Enable keep alive function. The value `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_  is  **<keep_alive>**,
    - The value `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_  is 1,  the value `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ is 3.

  -  The parameter ``<keep_alive>`` in this command is the same as the parameter ``<keep_alive>`` in the command :ref:`AT+CIPTCPOPT <cmd-TCPOPT>` , and the final value is determined by the command set later. If the ``<keep_alive>`` parameter is not set when running this command, the last configured value will be used by default.

-  **<"local IP">**: The local IPv4 address or IPv6 address bound to the connection. This parameter is very useful in local multi network interfaces and local multi IP addresses. Default is disabled. If you want to use it, you need to set it yourself, and null values are also valid values.

Description
"""""""""""""""
- If you want to establish a TCP connection based on an IPv6 network, please follow these steps:

  - Ensure that the AP supports IPv6
  - Setting :ref:`AT+CIPV6=1<cmd-IPV6>`
  - Obtain an IPv6 address through the command :ref:`AT+CWJAP<cmd-JAP>` 
  - (Optional) Check if the w800 has obtained an IPv6 address through the AT+CIPSTA? command.

Example
""""""""""""

::

    AT+CIPSTART="TCP","192.168.10.110",8080
    AT+CIPSTART="TCP","192.168.10.110",8080,60
    AT+CIPSTART="TCP","192.168.10.110",8080,,"192.168.10.100"
    AT+CIPSTART="TCPv6","fe80::860d:8eff:fe9d:cd90",1000

Establish UDP Transmission
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set Command
""""""""""""""

**Command:**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSTART=<"type">,<"remote host">,<remote port>[,<local port>,<mode>,<"local IP">]

    // Multi connection: (AT+CIPMUX=1)
    AT+CIPSTART=<link ID>,<"type">,<"remote host">,<remote port>[,<local port>,<mode>,<"local IP">]

**Response:**

In single connection mode, return:

::

    CONNECT
    OK

In multi connection mode, return:

::

    <link ID>,CONNECT
    OK

Parameters
"""""""""""""""""""""""

-  **<link ID>**      : Network connection ID (0~4), used for multi connection situations
-  **<"type">**       : String parameter, represent the type of network connection , "UDP" or "UDPv6".
-  **<"remote host">**: A string parameter that represents a remote IPv4 address, IPv6 address, or domain name. The maximum length is 64 bytes. If you need to use a domain name with a length exceeding 64 bytes, use the :ref:`AT+CIPDOMAIN<cmd-DOMAIN>` command to obtain the IP address corresponding to the domain name, and then establish a connection using the IP address.
-  **<remote port>**  : Remote port value
-  **<local port>**   : The UDP port value of the |Equipment-Name| device
-  **<mode>**         : Under UDP Wi-Fi transparent transmission, the value of this parameter must be set to 0

   -  0: After receiving UDP data, do not change the UDP address information of the remote end (default)
   -  1: Only when receiving UDP data from the remote end for the first time that is different from the initial settings, change the UDP address information of the remote end to the IP address and port of the sending data device.
   -  2: Every time UDP data is received, change the UDP address information of the remote end to the IP address and port of the device sending the data

-  **<"local IP">**: The bound local IPv4 or IPv6 address for connection. This parameter is particularly useful when there are multiple local network interfaces or multiple local IP addresses. By default, it is disabled. If you wish to use it, you need to set it manually. An empty value is also considered a valid input.

Description
"""""""""""""""
- If the remote IP address in the UDP connection is an IPv4 multicast address (224.0.0.0 ~ 239.255.255.255),the |Equipment-Name| will send and receive UDPv4 multicast.
- If the remote IP address in the UDP connection is an IPv4 broadcast address (255.255.255.255), the |Equipment-Name| will send and receive UDPv4 broadcast.
- If the remote IP address in the UDP connection is an IPv6 multicast address (FF00:0:0:0:0:0:0:0 ~ FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF),the |Equipment-Name| will send and receive UDP multicast over an IPv6 network.
- Before using the parameter ``<mode>``, you need to set the parameter ``<local port>``.
- If you want to establish a UDP transmission over an IPv6 network, follow these steps:

  - Ensure the AP supports IPv6.
  - Set :ref:`AT+CIPV6=1<cmd-IPV6>`.
  - Use the :ref:`AT+CWJAP<cmd-JAP>` command to obtain an IPv6 address.
  - (Optional) Use the :ref:`AT+CIPSTA?<cmd-IPSTA>` command to check if the |Equipment-Name| has obtained an IPv6 address.

- To receive the UDP packets larger than 1460 bytes, Please `compile the WinnerMicro-AT  project <https://#>`_yourself, and in step five,select: ``Component config`` -> ``LWIP`` -> ``Enable reassembly incoming fragmented IP4 packets``

Example
"""""""""""""""""

::

    // UDPv4 Unicast
    AT+CIPSTART="UDP","192.168.101.110",8080,1002,2
    AT+CIPSTART="UDP","192.168.101.110",8080,,,"192.168.101.100"
    // UDPv6 Unicast
    AT+CIPSTART="UDPv6","fe80::32ae:a4ff:fe80:65ac",1000

    // UDP Multicast over IPv4
    AT+CIPSTART="UDP","239.255.255.254",8080
    // UDP Multicast over IPv6
    AT+CIPSTART="UDPv6","FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFE",8080

Establishing an SSL Connection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Set Command
""""""""""""""

**Command:**

::

    // Single Connection Mode: (AT+CIPMUX=0)
    AT+CIPSTART=<"type">,<"remote host">,<remote port>[,<keep_alive>,<"local IP">]

    // Multiple Connection Mode:(AT+CIPMUX=1)
    AT+CIPSTART=<link ID>,<"type">,<"remote host">,<remote port>[,<keep_alive>,<"local IP">]

**Response:**

In single connection mode, return:

::

    CONNECT
    OK

In multi connection mode, return:

::

    <link ID>,CONNECT
    OK

Parameters
"""""""""""""""""""""""

- **<link ID>**: Network connection ID (0~4), used in multiple connection scenarios.
- **<"type">**: String parameter indicating the network connection type, "SSL" or "SSLv6".
- **<"remote host">**: String parameter indicating the remote IPv4 address, IPv6 address, or domain name. Maximum length is 64 bytes. If you need to use a domain name longer than 64 bytes, use the :ref:`AT+CIPDOMAIN<cmd-DOMAIN>` command to get the IP address corresponding to the domain name, then use the IP address to establish the connection.
- **<remote port>**: Remote port value.
-  **<keep_alive>**  : Configure the socket's ``SO_KEEPALIVE`` option (reference: `SO_KEEPALIVE Introduction <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`_), unit: seconds.

  - Range: [0,7200]。

    - 0: Disable keep-alive functionality; (default)
    - 1 ~ 7200: Enable keep-alive functionality. `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_ value is **<keep_alive>**,  `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_ value is 1,  `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ value is 3.

  -  The ``<keep_alive>`` parameter in this command is the same as the ``<keep_alive>`` parameter in the :ref:`AT+CIPTCPOPT <cmd-TCPOPT>` command. The final value is determined by the most recently set command. If the ``<keep_alive>`` parameter is not set when running this command, the default value from the last configuration is used.

-  **<"local IP">**   : Connect to the bound local IPv4 or IPv6 address, which is very useful for local multi network interfaces and local multi IP addresses. Default is disabled. If you want to use it, you need to set it yourself, and null values are also valid values

Description
""""""""""""""""

- The number of SSL connections depends on available memory and the maximum number of connections.
- SSL connections consume a lot of memory, and insufficient memory may cause system reboot.
- If the ``AT+CIPSTART`` command is based on SSL connection, and each packet's timeout is 10 seconds, the total timeout will be longer, depending on the number of handshake packets.
- To establish an SSL connection over an IPv6 network, follow these steps:

  - Ensure the AP supports IPv6.
  - Set :ref:`AT+CIPV6=1<cmd-IPV6>`.
  - Use the :ref:`AT+CWJAP<cmd-JAP>` command to obtain an IPv6 address.
  - (Optional) Use the :ref:`AT+CIPSTA?<cmd-IPSTA>` command to check if the |Equipment-Name| has obtained an IPv6 address.

Example
""""""""""""""""

::

    AT+CIPSTART="SSL","www.winnermicro.com",8443
    AT+CIPSTART="SSL","192.168.10.110",1000,,"192.168.101.100"
    AT+CIPSTART="SSLv6","FE80::5A9E:4687:702D:5D79",4433

.. _cmd-STARTEX:

:ref:`AT+CIPSTARTEX <TCPIP-AT>`: Establish TCP Connection, UDP Transmission, or SSL Connections with Automatically Assigned IDs
---------------------------------------------------------------------------------------------------------------------------------

This command is similar to :ref:`AT+CIPSTART <cmd-START>`, with the difference being that in multiple connection scenarios (:ref:`AT+CIPMUX=1 <cmd-MUX>`) you do not need to manually assign an ID, and the system will automatically assign an ID for the new connection.

.. _cmd-PLUS:

:ref:`[Applicable to Data Mode only] +++ <TCPIP-AT>`: Exit :term:`Data Mode`
----------------------------------------------------------------------------------------

Special Execution Command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Function:**

Exit :term:`Data Mode` and enter :term:`Command Mode`.

**Command:**

::

    // Applicable to data mode only
    +++

Description
""""""""""""""""

- This special execution command consists of three identical ``+`` characters (i.e., ASCII code: 0x2b), with no CR-LF characters at the end of the command.
- Ensure there is at least a 30 ms interval with no other input before the first ``+`` character, and at least a 30 ms interval with no other input after the third ``+`` character. There should be no more than a 30 ms interval with no other input between the three ``+`` characters. Otherwise, the ``+`` characters will be sent as regular data.
- This special execution command has no command reply.
- Wait at least 1 second before sending the next AT command.

.. _cmd-SEND:

:ref:`AT+CIPSEND <TCPIP-AT>`: Send Data in Either :term:`Normal Transmission mode` or Wi-Fi :term:`Passthrough Mode`
------------------------------------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Function:**

In :term:`Normal Transmission Mode` , specify the length to send data. If you need to send data longer than 5760 bytes, use the :ref:`AT+CIPSENDL <cmd-SENDL>` command.

**Command:**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSEND=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSEND=<link ID>,<length>

    // For UDP transmission, specify the remote host and port
    AT+CIPSEND=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response:**

::

    OK


This response indicates that the AT is ready to receive serial data. You can then input data, and once the AT receives data of the specified ``<length>``, data transmission will start.

If no connection is established or the connection is disconnected during data transmission, it returns:

::

    ERROR

If all data is successfully sent to the protocol stack (not necessarily to the remote end), it returns:

::

    SEND OK

Execute Command
^^^^^^^^^^^^^^^^^

**Function:**

Enter Wi-Fi :term:`Passthrough Mode`

**Command:**

::

    AT+CIPSEND

**Response:**

::

    OK

or

::

    ERROR

In Wi-Fi :term:`Passthrough Mode`, the |Equipment-Name| device can receive up to 512 bytes at a time. If the received data length is greater than or equal to 512 bytes, the data will be sent in blocks of 512 bytes each. Otherwise, the data will wait 30 milliseconds or until the received data length is greater than or equal to 512 bytes before sending. When inputting a single packet :ref:`+++ <cmd-PLUS>` , exit the data sending mode of  :term:`Passthrough Mode` . Please wait at least 1 second before sending the next AT command.

This command must be used in  :term:`Passthrough Mode` and in single connection mode. For Wi-Fi UDP transparent transmission, the parameter ``<mode>``  of the :ref:`AT+CIPSTART <cmd-START>` command must be set to 0.

Parameters
^^^^^^^^^^^^

-  **<link ID>**      : Network connection ID (0~4), used for multi connection situations
-  **<length>**       : Data length, maximum value: 5760 bytes
-  **<"remote host">**: For UDP transmission, specify the remote host: IPv4 address, IPv6 address, or domain name
-  **<remote port>**  : For UDP transmission, specify the remote port

Description
""""""""""""""""

- You can use the :ref:`AT+CIPTCPOPT <cmd-TCPOPT>` command to configure socket options for each TCP connection. For example, set `<so_sndtimeo>` to 5000 means that the TCP send operation will return a result within 5 seconds, regardless of whether it succeeds or fails. This can save the MCU's time waiting for the AT command response.

.. _cmd-SENDL:

:ref:`AT+CIPSENDL(Not Supported) <TCPIP-AT>`: Parallel Transmission of Long Data in :term:`Normal transmission mode`
----------------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Function:**

In :term:`Normal Transmission Mode` , specify the length to send data in parallel (the AT command port receives data and the AT sends data to the remote end in parallel). If the data length is less than 5760 bytes, you can also use the :ref:`AT+CIPSEND <cmd-SEND>` command to send data.

**Command:**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSENDL=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSENDL=<link ID>,<length>

    // For UDP transmission, specify the remote host and port
    AT+CIPSENDL=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response:**

::

    OK


This response indicates that the AT has entered :term:`Data Mode` and is ready to receive data from the AT command port. You can then input data, and as soon as the AT command port receives data, the data will be sent to the lower protocol, and data transmission will start.

::

    +CIPSENDL:<had sent len>,<port recv len>

If the transmission is canceled by the :ref:`+++ <cmd-PLUS>` command, the system returns:

::

    SEND CANCELLED

If all data is not completely sent, the system finally returns:

::

    SEND FAIL

If all data is successfully sent to the protocol stack (not necessarily to the remote end), the system finally returns:

::

    SEND OK 

When the connection is disconnected, you can send the :ref:`+++ <cmd-PLUS>` command to cancel the transmission, and the |Equipment-Name| device will exit :term:`data mode` . Otherwise, the AT command port will continue to receive data until the specified ``<length>`` of data is received, and then it will exit  :term:`data mode`。

Parameters
^^^^^^^^^^^^

-  **<link ID>**      : Network connection ID (0~4), used for multi connection situations
-  **<length>**       : Data length, maximum value: 2 :sup:`31` - 1  bytes
-  **<"remote host">**: For UDP Transmission, Specify the remote host: IPv4 address, IPv6 address, or domain name
-  **<remote port>**  : For UDP Transmission, Specify the remote port
-  **<had sent len>** : The length of data successfully sent to the lower protocol stack
-  **<port recv len>**: The total length of data received by the AT command port

Description
""""""""""""""""

- It is recommended to use UART flow control. Otherwise, if the UART reception speed is greater than the network transmission speed, data loss will occur.
- You can use the :ref:`AT+CIPTCPOPT <cmd-TCPOPT>` command to configure socket options for each TCP connection. For example, set <so_sndtimeo> to 5000 means that the TCP send operation will return a result within 5 seconds, regardless of whether it succeeds or fails. This can save the MCU's time waiting for the AT command response.

.. _cmd-SENDLCFG:

:ref:`AT+CIPSENDLCFG(Not supported) <cmd-SENDLCFG>`: Setting :ref:`AT+CIPSENDL <cmd-SENDL>` Properties of Command
------------------------------------------------------------------------------------------------------------------------------------

.. _cmd-SENDEX:

:ref:`AT+CIPSENDEX <TCPIP-AT>`: Sending Data Using An Extended Method in :term:`Normal Transmission Mode`
------------------------------------------------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Function:**

In :term:`Normal Transmission Mode` , specify the length of data to be sent or trigger data transmission using the string ``\0`` (0x5c, 0x30 ASCII).

**Command:**

::

    // Single connection: (AT+CIPMUX=0)
    AT+CIPSENDEX=<length>

    // Multiple connections: (AT+CIPMUX=1)
    AT+CIPSENDEX=<link ID>,<length>

    // For UDP transmission, you can specify the remote IP address and port:
    AT+CIPSENDEX=[<link ID>,]<length>[,<"remote host">,<remote port>]

**Response:**

::

    OK


This response indicates that the AT command is ready to receive serial data. You can then input the specified length of data, and data transmission will start once the AT command receives data of length ``<length>`` or encounters the ``\0`` character.

If no connection is established or the connection is disconnected during data transmission, it will return:

::

    ERROR

If all data is successfully sent to the protocol stack (which does not mean it has been sent to the endpoint), it will return:

::

    SEND OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**      : Network connection ID (0~4), used for multi connection situations
-  **<length>**       : Data length, maximum value: 5760  bytes
-  **<"remote host">**: For UDP transmission, you can specify the remote host: IPv4, IPv6 address, or domain name
-  **<remote port>**  : For UDP transmission, you can specify the remote port

Notes
^^^^^^^^^

-  Data transmission starts when the data length meets the requirement or when the  ``\0`` character (0x5c，0x30 ASCII)is encountered, and the system returns to normal command mode, waiting for the next AT command.
-  If the data contains ``\<any>``, the backslash will be removed, and only the  ``<any>`` symbol will be used.
-  If you need to send ``\0``, please escape it as ``\\0``
- You can use the :ref:`AT+CIPTCPOPT <cmd-TCPOPT>` command to configure socket options for each TCP connection. For setting <so_sndtimeo> to 5000 will make the TCP send operation return results within 5 seconds, whether successful or not. This can save MCU waiting time for AT command replies.

.. _cmd-CLOSE:

:ref:`AT+CIPCLOSE <TCPIP-AT>`: Close TCP/UDP/SSL Connection
----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^

**Function:**

Close TCP/UDP/SSL connections in multi-connection mode

**Command:**

::

    AT+CIPCLOSE=<link ID>

**Response:**

::

    <link ID>,CLOSED

    OK

Execute Command
^^^^^^^^^^^^^^^^^^^

**Function:**

Close TCP/UDP/SSL connections in single connection mode

::

    AT+CIPCLOSE

**Response:**

::

    CLOSED

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**: Network connection ID to be closed; if set to 5, it means closing all connections

.. _cmd-IFSR:

:ref:`AT+CIFSR <TCPIP-AT>`: Query Local IP Address and MAC Address
------------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIFSR

**Response:**

::

    +CIFSR:APIP,<"APIP">
    +CIFSR:APMAC,<"APMAC">
    +CIFSR:STAIP,<"STAIP">
    +CIFSR:STAIP6LL,<"STAIP6LL">
    +CIFSR:STAIP6GL,<"STAIP6GL">
    +CIFSR:STAMAC,<"STAMAC">
    OK

Parameters
^^^^^^^^^^^^

- **<"APIP">**     : IPv4 address of |Equipment-Name| SoftAP 
- **<"APMAC">**    : MAC address of|Equipment-Name| SoftAP 
- **<"STAIP">**    : IPv4 address of |Equipment-Name| Station
- **<"STAIP6LL">** : IPv6 link-local address of |Equipment-Name| Station
- **<"STAIP6GL">** : IPv6 global address of|Equipment-Name| Station
- **<"STAMAC">**   : MAC address of |Equipment-Name| Station 

Notes
^^^^^^

-  Only when the |Equipment-Name| has obtained valid interface information can its IP address and MAC address be queried.

.. _cmd-MUX:

:ref:`AT+CIPMUX <TCPIP-AT>`: Enable/disable Multiple Connection Mode
-----------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the connection mode

**Command:**

::

    AT+CIPMUX?

**Response:**

::

    +CIPMUX:<mode>
    OK

Set Command
^^^^^^^^^^^^^


**Function:**

Set the connection mode

**Command:**

::

    AT+CIPMUX=<mode>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<mode>**: Connection mode, default value: 0

   -  0: Single connection
   -  1: Multiple connections

Notes
^^^^^^

-  The connection mode can only be changed when all connections are closed.
-  Only in :term:`Normal Transmission Mode` (:ref:`AT+CIPMODE=0 <cmd-IPMODE>`), can it be set to multiple connections.
-  If a TCP/SSL server is established and you want to switch to single connection mode, you must close the server (:ref:`AT+CIPSERVER=0 <cmd-SERVER>`)

Example
^^^^^^^^

::

    AT+CIPMUX=1 

.. _cmd-SERVER:

:ref:`AT+CIPSERVER <TCPIP-AT>`: Establish/Close  TCP or SSL Server
------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the status of the TCP/SSL server

**Command:**

::

    AT+CIPSERVER?

**Response:**

::

    +CIPSERVER:<mode>[,<port>,<"type">][,<CA enable>]

    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPSERVER=<mode>[,<param2>][,<"type">][,<CA enable>]

**Response:**

::

    OK  

Parameters
^^^^^^^^^^^^

-  **<mode>**     ：

   -  0: Close the server
   -  1: Establish the server

-  **<param2>**    : The meaning of this parameter depends on the value of ``<mode>`` :

  - If ``<mode>`` is 1, ``<param2>`` represents the port number. Default value: 333
  - If ``<mode>`` is 0, ``<param2>`` represents whether to close all client connections when the server is shut down. Default value: 0

    - 0: Close the server and keep existing client connections
    - 1: Close the server and close all connections

-  **<"type">**   : Server type: "TCP", "TCPv6", "SSL", or "SSLv6". Default value: "TCP"
-  **<CA enable>**：Default value: 0

   -  0: No CA certification
   -  1: Use CA certification

Notes
^^^^^^

- A server can only be started in multiple connection mode (:ref:`AT+CIPMUX=1 <cmd-MUX>`).
- After creating the server, it will automatically start listening, and only one server can be created at most.
- When a client connects, it will automatically occupy a connection ID.
- If you want to create a TCP/SSL server over an IPv6 network, first set :ref:`AT+CIPV6=1<cmd-IPV6>`, and obtain an IPv6 address.
- When closing the server, parameters  ``<"type">`` and ``<CA enable>`` must be omitted.

Example
^^^^^^^^

::

    // Establish a TCP server
    AT+CIPMUX=1
    AT+CIPSERVER=1,80

    // Establish an SSL server
    AT+CIPMUX=1
    AT+CIPSERVER=1,443,"SSL",1

    // Create an SSL server over an IPv6 network
    AT+CIPMUX=1
    AT+CIPSERVER=1,443,"SSLv6",0

    // Close the server and close all connections
    AT+CIPSERVER=0,1

.. _cmd-SERVERMAX:

:ref:`AT+CIPSERVERMAXCONN <TCPIP-AT>`: Query/Set the Maximum Number of Connections Allowed by the Server
--------------------------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the maximum number of connections allowed by the TCP or SSL server

**Command:**

::

    AT+CIPSERVERMAXCONN?

**Response:**

::

    +CIPSERVERMAXCONN:<num>
    OK  

Set Command
^^^^^^^^^^^^^


**Function:**

Set the maximum number of connections allowed by the TCP or SSL server

**Command:**

::

    AT+CIPSERVERMAXCONN=<num>

**Response:**

::

    OK  

Parameters
^^^^^^^^^^^^

-  **<num>**: The maximum number of connections allowed by the TCP or SSL server, range: [1,5]. To modify the upper limit threshold of this parameter, refer to the description of the ``<link ID>`` parameter in the :ref:`AT+CIPSTART <cmd-START>` command.

Notes
^^^^^^

-  To set the maximum number of connections (``AT+CIPSERVERMAXCONN=<num>``), set it before creating the server.

Example
^^^^^^^^

::

    AT+CIPMUX=1
    AT+CIPSERVERMAXCONN=2
    AT+CIPSERVER=1,80

.. _cmd-IPMODE:

:ref:`AT+CIPMODE <TCPIP-AT>`: Query/set Transmission Mode
------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query transmission mode

**Command:**

::

    AT+CIPMODE?

**Response:**

::

    +CIPMODE:<mode>
    OK

Set Command
^^^^^^^^^^^^^


**Function:**

Set transmission mode

**Command:**

::

    AT+CIPMODE=<mode>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<mode>**:

   -  0: :term:`Normal Transmission Mode`
   -  1: Wi-Fi :term:`Passthrough Receiving Mode`, Only supports TCP single connection, UDP fixed communication with the remote end, and SSL single connection

Notes
^^^^^^

-  Configuration changes are not saved to flash.
-  After entering Wi-Fi :term:`Passthrough Receiving Mode` in |Equipment-Name|, any Bluetooth function will not be available.

Example
^^^^^^^^

::

    AT+CIPMODE=1

.. _cmd-STO:

:ref:`AT+CIPSTO <TCPIP-AT>`: Query/Set Local TCP Server Timeout
----------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query local TCP/SSL server timeout

**Command:**

::

    AT+CIPSTO?

**Response:**

::

    +CIPSTO:<time>
    OK

Set Command
^^^^^^^^^^^^^


**Function:**

Set local TCP/SSL server timeout

**Command:**

::

    AT+CIPSTO=<time>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<time>**: Local TCP/SSL server timeout, unit: seconds, value range:[0,7200]

Notes
^^^^^^

-  When there is no data communication between TCP/SSL clients within ``<time>`` , the |Equipment-Name| server will disconnect this connection.
-  If the parameter ``<time>`` is set to 0, the connection will never time out, which is not recommended.
-  When the client initiates communication with the server or the server initiates communication with the client within the set time, the timer will be reset. After timeout, the client was closed.

Example
^^^^^^^^

::

    AT+CIPMUX=1
    AT+CIPSERVER=1,1001
    AT+CIPSTO=10

.. _cmd-SNTPCFG:

:ref:`AT+CIPSNTPCFG <TCPIP-AT>`: Query/set Time Zone and SNTP Server
------------------------------------------------------------------------------

Query command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIPSNTPCFG?

**Response:**

::

    +CIPSNTPCFG:<enable>,<timezone>,<SNTP server1>[,<SNTP server2>,<SNTP server3>]
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPSNTPCFG=<enable>,<timezone>[,<SNTP server1>,<SNTP server2>,<SNTP server3>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<enable>**: Set the SNTP server.

   -  1: Set up SNTP server;
   -  0: Do not set up SNTP server.

-  **<timezone>**: Supports two formats:

   -  First Format: Ranges from: [-12,14]. (UTC) It marks most time zones（from  `UTC−12:00 <https://en.wikipedia.org/wiki/UTC%E2%88%9212:00>`_ to `UTC+14:00 <https://en.wikipedia.org/wiki/UTC%2B14:00>`_）in hours by offsetting from Coordinated Universal Time (UTC);
   -  The second format is ``UTC offset``. ``UTC offset``  specifies how much time you need to add to the UTC time to get the local time, usually displayed as ``[+|-][hh]mm``. If the local time zone is west of the Prime Meridian, it is negative; if it is east, it is positive. The hour (hh) must be between -12 and 14, and the minute (mm) must be between 0 and 59. For example, if you want to set the time zone to Chatham Islands, New Zealand, which is  ``UTC+12:45``, you should set the ``<timezone>`` parameter to ``1245`` . For more information, please refer to `UTC offset <https://en.wikipedia.org/wiki/Time_zone#List_of_UTC_offsets>`_。

-  **[<SNTP server1>]**: The first SNTP server.
-  **[<SNTP server2>]**: The second SNTP server.
-  **[<SNTP server3>]**: The third SNTP server.

Notes
^^^^^^

-  If the above three SNTP server parameters are not filled in the Set command, one of "cn.ntp.org.cn"、"ntp.sjtu.edu.cn" and "us.pool.ntp.org" will be used by default.
-  For query commands, the ``<timezone>`` parameter queried may be different from the set ``<timezone>`` parameter. Because the ``<timezone>`` parameter supports the second UTC offset format, such as setting  ``AT+CIPSNTPCFG=1,015``. So when querying, ESP-AT will ignore the leading 0 of the time zone parameter,that is, which means the set value is ``15`` . It does not belong to the first format, so it is parsed according to the second UTC offset format, which is ``UTC+00:15``, and the queried time zone is 0.
-  Due to the fact that SNTP is based on the UDP protocol for sending requests and receiving replies, when network packet loss occurs, it can cause the | Equipment Name | time to be unable to synchronize in a timely manner.

Example
^^^^^^^^

::

    // Enable SNTP Server and Set China Timezone (UTC+08:00):
    AT+CIPSNTPCFG=1,8,"cn.ntp.org.cn","ntp.sjtu.edu.cn"
    or
    AT+CIPSNTPCFG=1,800,"cn.ntp.org.cn","ntp.sjtu.edu.cn"

    // Enable SNTP Server and Set New York, USA Timezone (UTC−05:00):
    AT+CIPSNTPCFG=1,-5,"0.pool.ntp.org","time.google.com"
    or
    AT+CIPSNTPCFG=1,-500,"0.pool.ntp.org","time.google.com"

    // Enable SNTP Server and Set Chatham Islands, New Zealand Timezone (UTC+12:45):
    AT+CIPSNTPCFG=1,1245,"0.pool.ntp.org","time.google.com"

.. _cmd-SNTPT:

:ref:`AT+CIPSNTPTIME <TCPIP-AT>`: Query SNTP Time
-------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIPSNTPTIME? 

**Response:**

::

    +CIPSNTPTIME:<asctime style time>
    OK

Notes
^^^^^^

-  For the definition of asctime, please refer to `asctime man page <https://linux.die.net/man/3/asctime>`_.
-  After entering Light sleep or Deep sleep in |Equipment-Name| and then waking up, the system time may be inaccurate. We suggest that you resend the :ref:`AT+CIPSNTPCFG <cmd-SNTPCFG>` command to retrieve the new time from the NTP server.

Example
^^^^^^^^

::

    AT+CWMODE=1
    AT+CWJAP="1234567890","1234567890"
    AT+CIPSNTPCFG=1,8,"cn.ntp.org.cn","ntp.sjtu.edu.cn"
    AT+CIPSNTPTIME?
    +CIPSNTPTIME:Tue Oct 19 17:47:56 2021
    OK

    or

    AT+CWMODE=1
    AT+CWJAP="1234567890","1234567890"
    AT+CIPSNTPCFG=1,530
    AT+CIPSNTPTIME?
    +CIPSNTPTIME:Tue Oct 19 15:17:56 2021
    OK

.. _cmd-SNTPINTV:

:ref:`AT+CIPSNTPINTV <TCPIP-AT>`: Query/Set SNTP Time Synchronization Interval
----------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIPSNTPINTV? 

**Response:**

::

    +CIPSNTPINTV:<interval second>

    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPSNTPINTV=<interval second>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<interval second>**: SNTP time synchronization interval. Unit: seconds. Scope:[15,4294967]。

Notes
^^^^^^

- Set the time synchronization interval means how often the |Equipment-Name| retrieves new time from the NTP server.

Example
^^^^^^^^

::

    AT+CIPSNTPCFG=1,8,"cn.ntp.org.cn","ntp.sjtu.edu.cn"

    OK

    //Synchronize time once per hour
    AT+CIPSNTPINTV=3600

    OK

.. _cmd-FWVER:

:ref:`AT+CIPFWVER (Not supported) <TCPIP-AT>`: Query the AT Firmware Version Already on the Server
-----------------------------------------------------------------------------------------------------

.. _cmd-UPDATE:

:ref:`AT+CIUPDATE(Not supported) <TCPIP-AT>`: Upgrade Firmware via Wi-Fi
--------------------------------------------------------------------------------------

.. _cmd-IPDINFO:

:ref:`AT+CIPDINFO <TCPIP-AT>`: Set +IPD Message Details
----------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+CIPDINFO?

**Response:**

::

    +CIPDINFO:true
    OK

or

::
    
    +CIPDINFO:false
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPDINFO=<mode>  

**Response:**

::

    OK  

Parameters
^^^^^^^^^^^^

-  **<mode>**:

   -  0: In the "+IPD" and "+CIPRECVDATA" messages, do not prompt for the IP address and port information of the remote end (default is 0)
   -  1: In the "+IPD" and "+CIPRECVDATA" messages, prompt for the IP address and port information of the remote end

Example
^^^^^^^^

::

    AT+CIPDINFO=1

.. _cmd-SSLCCONF:

:ref:`AT+CIPSSLCCONF <TCPIP-AT>`: Query/Set SSL Client Configuration
-------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the configuration information for each connection when |Equipment-Name| is acting as an SSL client

**Command:**

::

    AT+CIPSSLCCONF?

**Response:**

::

    +CIPSSLCCONF:<link ID>,<auth_mode>,<pki_number>,<ca_number>
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    // Single connection:(AT+CIPMUX=0)
    AT+CIPSSLCCONF=<auth_mode>[,<pki_number>][,<ca_number>]

    // Multiple connections:(AT+CIPMUX=1)
    AT+CIPSSLCCONF=<link ID>,<auth_mode>[,<pki_number>][,<ca_number>]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**   : Network connection ID (0 ~ max), in the case of multiple connections, if the parameter value is set to max, it means all connections, and the default value of this parameter is 5.
-  **<auth_mode>** :

   -  0: Do not authenticate, there is no need to fill in the parameters ``<pki_number>`` and ``<ca_number>`` at this time;
   -  1: WinnerMicro-AT provides client certificates for server-side CA certificate verification;
   -  2: WinnerMicro-AT client loads a CA certificate to verify the server-side certificate;
   -  3: Mutual authentication.

-  **<pki_number>**: The index of the certificate and private key. If there is only one certificate and private key, its value should be 0.
-  **<ca_number>** : The index of CA, if there is only one CA, should be 0.

Notes
^^^^^^

-  If you want this configuration to take effect immediately, please run this command before establishing an SSL connection.
-  When AT+SYSSTORE=1 is set, configuration changes will be saved in the NVS area.
-  If you want to use your own certificate or multiple sets of certificates, please refer to `be done <https://#>`_ .
-  If ``<auth_mode>`` is set to 2 or 3,  to verify the server's certificate validity period, please ensure |Equipment-Name| has obtained the current time before sending the :ref:`AT+CIPSTART <cmd-START>`  command. (You can configure SNTP by sending the :ref:`AT+CIPSNTPCFG <cmd-SNTPCFG>` command to get the current time and send the :ref:`AT+CIPSNTPTIME? <cmd-SNTPT>`  command to query the current time.)

.. _cmd-SSLCCN:

:ref:`AT+CIPSSLCCN <TCPIP-AT>`: Query/Set SSL Client's Common Name
-------------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the common name of the client in each SSL connection

**Command:**

::

    AT+CIPSSLCCN?

**Response:**

::

    +CIPSSLCCN:<link ID>,<"common name">
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    // Single connection:(AT+CIPMUX=0)
    AT+CIPSSLCCN=<"common name">

    // Multiple connections:(AT+CIPMUX=1)
    AT+CIPSSLCCN=<link ID>,<"common name">

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**      : Network connection ID (0 ~ max), in single connection mode, this parameter value is 0; in multiple connection mode, if the parameter value is set to max, it means all connections; the default value of this parameter is 5.
-  **<"common name">**: This parameter is used to authenticate the common name in the server's certificate. The maximum length of the common name is 64 bytes.

Notes
^^^^^^

-  To make this configuration effective immediately, run this command before establishing an SSL connection.

.. _cmd-SSLCSNI:

:ref:`AT+CIPSSLCSNI <TCPIP-AT>`: Query/Set SSL Client's SNI
---------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the SNI configuration of each connection

**Command:**

::

    AT+CIPSSLCSNI?

**Response:**

::

    +CIPSSLCSNI:<link ID>,<"sni">
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    Single connection:(AT+CIPMUX=0)
    AT+CIPSSLCSNI=<"sni">

    Multiple connections:(AT+CIPMUX=1)
    AT+CIPSSLCSNI=<link ID>,<"sni">

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**: Network connection ID (0 ~ max), in single connection mode, this parameter value is 0; in multiple connection mode, if the parameter value is set to max, it means all connections; the default value of this parameter is 5.
-  **<"sni">**: SNI in ClientHello. The maximum length of SNI is 64 bytes.

Notes
^^^^^^

-  To make this configuration effective immediately, run this command before establishing an SSL connection.

.. _cmd-SSLCALPN:

:ref:`AT+CIPSSLCALPN <TCPIP-AT>`: Query/set SSL Client's ALPN
-----------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the ALPN configuration of each connection when |Equipment-Name| acts as an SSL client

**Command:**

::

    AT+CIPSSLCALPN?

**Response:**

::

    +CIPSSLCALPN:<link ID>,<"alpn">[,<"alpn">][,<"alpn">]

    OK

Set Command
^^^^^^^^^^^^^
^^

**Command:**

::

    // Single connection:(AT+CIPMUX=0)
    AT+CIPSSLCALPN=<counts>[,<"alpn">][,<"alpn">][,<"alpn">]

    // Multiple connections:(AT+CIPMUX=1)
    AT+CIPSSLCALPN=<link ID>,<counts>[,<"alpn">][,<"alpn">][,<"alpn">]

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**: Network connection ID (0 ~ max), in single connection mode, this parameter value is 0; in multiple connection mode, if the parameter value is set to max, it means all connections; the default value of this parameter is 5.
-  **<counts>** : The number of ALPN. Range: [0,5].

  - 0    : Clear ALPN configuration.
  - [1,5]: Set ALPN configuration.

-  **<"alpn">**: String parameter representing the ALPN in ClientHello. The maximum length of ALPN is limited by the maximum length of the command.

Notes
^^^^^^

-  To make this configuration effective immediately, run this command before establishing an SSL connection.

.. _cmd-SSLCPSK:

:ref:`AT+CIPSSLCPSK <TCPIP-AT>`: Query/Set SSL Client's PSK
---------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the PSK configuration of each connection when |Equipment-Name| acts as an SSL client

**Command:**

::

    AT+CIPSSLCPSK?

**Response:**

::

    +CIPSSLCPSK:<link ID>,<"psk">,<"hint">
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    // Single connection:(AT+CIPMUX=0)
    AT+CIPSSLCPSK=<"psk">,<"hint">

    // Multiple connections:(AT+CIPMUX=1)
    AT+CIPSSLCPSK=<link ID>,<"psk">,<"hint">

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<link ID>**: Network connection ID (0 ~ max), in single connection mode, this parameter value is 0; in multiple connection mode, if the parameter value is set to max, it means all connections; the default value of this parameter is 5.
-  **<"psk">**  : PSK identity, maximum length: 32.
-  **<"hint">**  : PSK hint, maximum length: 32.

Notes
^^^^^^
-  To make this configuration effective immediately, run this command before establishing an SSL connection.

.. _cmd-AUTOCONNINT:

:ref:`AT+CIPRECONNINTV <TCPIP-AT>`: Query/Set TCP/UDP/SSL Reconnection Interval under Wi-Fi:term:`Passthrough Mode`
---------------------------------------------------------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query TCP/UDP/SSL reconnection interval under Wi-Fi:term:`Passthrough Mode`

**Command:**

::

    AT+CIPRECONNINTV?

**Response:**

::

    +CIPRECONNINTV:<interval>
    OK

Set Command
^^^^^^^^^^^^^


**Function:**

Set TCP/UDP/SSL reconnection interval under Wi-Fi:term:`Passthrough Mode`

**Command:**

::

    AT+CIPRECONNINTV=<interval>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

-  **<interval>**: The automatic reconnection interval time, in units of 100 milliseconds, default value: 1, range:[1,36000]。

Notes
^^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>` is set, the configuration change will be saved in the NVS area.


Example
^^^^^^^^

::

    AT+CIPRECONNINTV=10  

.. _cmd-CIPRECVMODE:

:ref:`AT+CIPRECVMODE <TCPIP-AT>`: Query/Set Socket Receive Mode
-----------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query socket receive mode

**Command:**

::

    AT+CIPRECVMODE?

**Response:**

::

    +CIPRECVMODE:<mode>
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    AT+CIPRECVMODE=<mode>

**Response:**

::

    OK

Parameter
^^^^^^^^^^^^

- **<mode>**: Socket data reception mode, default value: 0.
   
   - 0: Active mode, WinnerMicro-AT immediately sends all received socket data to the host MCU, with a header of "+IPD" (the socket receiving window is 5760 bytes, and each time it sends a maximum of 2920 bytes of valid data to the MCU).
   - 1: Passive mode, WinnerMicro-AT saves all received socket data to an internal cache(socket receive window, default value is 5760 bytes), waiting for MCU to read. For TCP and SSL connections, if the cache is full, socket transmission will be blocked; For UDP transmission, if the cache is full, data loss will occur.

Notes
^^^^^^

-  This configuration cannot be used for Wi-Fi :term:`Passthrough Mode`。

-  When WinnerMicro-AT receives socket data in passive mode, it will prompt different information depending on the situation:

   -  When multiple connected (AT+CIPMUX=1), prompt ``+IPD,<link ID>,<len>``;
   -  When single connected (AT+CIPMUX=0), prompt ``+IPD,<len>`` .

-  ``<len>`` represents the total length of socket data in the cache.
-  Once a ``+IPD`` is reported, you should run :ref:`AT+CIPRECVDATA <cmd-CIPRECVDATA>` command to read the data. Otherwise, the next ``+IPD`` will not be reported to the host MCU until the previous ``+IPD``  is read.
-  In the case of disconnection, the buffered socket data still exists and the MCU can still read it until sending :ref:`AT+CIPCLOSE <cmd-CLOSE>` (AT as client) or :ref:`AT+CIPSERVER=0,1 <cmd-SERVER>` (AT as server). In other words, if the ``+IPD`` has already been reported, the information ``CLOSED`` for this connection will never appear until you send :ref:`AT+CIPCLOSE <cmd-CLOSE>` or :ref:`AT+CIPSERVER=0,1 <cmd-SERVER>` or :ref:`AT+CIPRECVDATA <cmd-CIPRECVDATA>` command to read all data.
-  When the device is expected to receive a large amount of network data and the MCU is unable to process it in time, you can refer to :ref:`example <using-passive-mode>` and use passive data reception mode.

Example
^^^^^^^^

::

    AT+CIPRECVMODE=1   

.. _cmd-CIPRECVDATA:

:ref:`AT+CIPRECVDATA <TCPIP-AT>`: Get Socket data in Passive Receive mode
-------------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Command:**

::

    // Single connection:(AT+CIPMUX=0)
    AT+CIPRECVDATA=<len>

    // Multiple connections:(AT+CIPMUX=1)
    AT+CIPRECVDATA=<link_id>,<len>

**Response:**

::

    +CIPRECVDATA:<actual_len>,<data>
    OK

or

::

    +CIPRECVDATA:<actual_len>,<remote IP>,<remote port>,<data>
    OK

Parameters
^^^^^^^^^^^^

-  **<link_id>**      : Connection ID in multiple connection mode.
-  **<len>**          : The maximum value is0x7fffffff. If the actual received data length is smaller than the value of this parameter, the actual length of the data will be returned.
-  **<actual_len>**   : Actual length of data obtained.
-  **<data>**         : Obtained data.
-  **[<remote IP>]**  : A string parameter representing the IP address of the remote end, enabled by the command: :ref:`AT+CIPDINFO=1 <cmd-IPDINFO>` .
-  **[<remote port>]**: The remote port, enable through the command :ref:`AT+CIPDINFO=1 <cmd-IPDINFO>` .

Example
^^^^^^^^

::

    AT+CIPRECVMODE=1

    // For example, if the host MCU receives 100 bytes of data from connection 0,
    // The message "+IPD,0,100"will be prompted,
    // Then, you can read this 100 byte data by running the following command:
    AT+CIPRECVDATA=0,100

.. _cmd-CIPRECVLEN:

:ref:`AT+CIPRECVLEN <TCPIP-AT>`: Query the Length of Socket Data in Passive Receive Mode
----------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the length of all cached data in a certain connection

**Command:**

::

    AT+CIPRECVLEN?

**Response:**

::

    +CIPRECVLEN:<data length of link0>,<data length of link1>,<data length of link2>,<data length of link3>,<data length of link4>
    OK

Parameter
^^^^^^^^^^^^

- **<data length of link>**: The length of all data buffered in a certain connection.

Notes
^^^^^^

-  In SSL connections, WinnerMicro-AT will return the length of the encrypted data, so the returned length will be greater than the length of the real data.

Example
^^^^^^^^

::

    AT+CIPRECVLEN?
    +CIPRECVLEN:100,,,,,
    OK

.. _cmd-CIPPING:

:ref:`AT+PING <TCPIP-AT>`: Ping the Remote Host
----------------------------------------------------

Set Command
^^^^^^^^^^^^^


**Function:**

Ping the remote host

**Command:**

::

    AT+PING=<"host">

**Response:**

::

    +PING:<time>

    OK

or

::

    +PING:TIMEOUT   // This reply only occurs when domain name resolution fails or PING timeout occurs

    ERROR

Parameters
^^^^^^^^^^^^

- **<"host">**: A string parameter that represents the IPv4 address, IPv6 address, or domain name of the peer host.
- **<time>**  : The response time of ping, in milliseconds.

Notes
^^^^^^
- If you want to ping the peer host based on IPv6 network, please perform the following steps:

  - Ensure that the AP supports IPv6
  - Set :ref:`AT+CIPV6=1<cmd-IPV6>`
  - Obtain an IPv6 address through the command :ref:`AT+CWJAP<cmd-JAP>` 
  - (Optional)Check if |Equipment-Name| has obtained an IPv6 address using the :ref:`AT+CIPSTA?<cmd-IPSTA>` command

- If the remote host is a domain name string, ping will first perform domain name resolution through DNS (prioritizing IPv4 address resolution), and then ping the IP address of the opposite host

Example
^^^^^^^^

::

    AT+PING="192.168.1.1"
    AT+PING="www.baidu.com"

    //ping IPv6 address
    AT+PING="240E:FF:E020:966:0:FF:B042:F296"

.. _cmd-DNS:

:ref:`AT+CIPDNS <TCPIP-AT>`: Query/Set DNS Server Information
------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query current DNS server information

**Command:**

::

    AT+CIPDNS?

**Response:**

::

    +CIPDNS:<enable>[,<"DNS IP1">][,<"DNS IP2">]
    OK

Set Command
^^^^^^^^^^^^^

**Function:**

Set DNS server information

**Command:**

::

    AT+CIPDNS=<enable>[,<"DNS IP1">][,<"DNS IP2">]

**Response:**

::

    OK

or

::

    ERROR

Parameters
^^^^^^^^^^^^

-  **<enable>**: Set up DNS server

   -  0: Enable automatic DNS server setting. The DNS server will revert to ``208.67.222.222`` and ``8.8.8.8``. The DNS server might be updated only after the |Equipment-Name| station completes the DHCP process.
   -  1: Enable manual DNS server setting.  If the ``<DNS IPx>`` parameter is not set, the default values  ``208.67.222.222`` and ``8.8.8.8``.

-  **<DNS IP1>**: The first DNS server IP address. For the Set command, this parameter is valid only when the <enable> parameter is 1, meaning manual DNS setting is enabled. If  <enable> is set to 1 and this parameter is assigned a value, WinnerMicro-AT will return this parameter as the current DNS setting when you run the query command.
-  **<DNS IP2>**: The second DNS server IP address. For the Set command, this parameter is valid only when the <enable> parameter is 1, meaning manual DNS setting is enabled. If  <enable>  is set to 1 and this parameter is assigned a value, WinnerMicro-AT will return this parameter as the current DNS setting when you run the query command.

Notes
^^^^^^

-  If :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`, configuration changes will be saved in the NVS area.
-  These three parameters cannot be set on the same server.
-  When ``<enable>``  is 0, the DNS server may change based on the configuration of the router connected to the |Equipment-Name| device.

Example
^^^^^^^^

::

    AT+CIPDNS=0
    AT+CIPDNS=1,"208.67.222.222","8.8.8.8"

    // The first DNS server based on IPv6: Next Generation Internet National Engineering Center
    // The second DNS server based on IPv6: google-public-dns-a.google.com
    AT+CIPDNS=1,"240c::6666","2001:4860:4860::8888"

.. _cmd-TCPOPT:

:ref:`AT+CIPTCPOPT <TCPIP-AT>`: Query/Set Socket Options
---------------------------------------------------------------------

Query command
^^^^^^^^^^^^^^^^

**Function:**

Query current socket options

**Command:**

::

    AT+CIPTCPOPT?

**Response:**

::

    +CIPTCPOPT:<link_id>,<so_linger>,<tcp_nodelay>,<so_sndtimeo>,<keep_alive>
    OK

Set Command
^^^^^^^^^^^^^


**Command:**

::

    // Single connection:(AT+CIPMUX=0):
    AT+CIPTCPOPT=[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>][,<keep_alive>]

    // Multiple connections:(AT+CIPMUX=1):
    AT+CIPTCPOPT=<link ID>,[<so_linger>],[<tcp_nodelay>],[<so_sndtimeo>][,<keep_alive>]

**Response:**

::

    OK

or

::

    ERROR

Parameters
^^^^^^^^^^^^

-  **<link_id>**    : Network connection ID (0 ~ max). In the case of multiple connections, if the parameter value is set to max, it indicates all connections; the default value of this parameter is 5.
-  **<so_linger>**  : Set the socket ``SO_LINGER`` option (reference: `SO_LINGER Introduction <https://man7.org/linux/man-pages/man7/socket.7.html#SO_LINGER>`_), unit: seconds, default value: -1.

   -  = -1: Disable;
   -  = 0: Enable, linger time = 0;
   -  > 0: Enable, linger time = <so_linger>;

-  **<tcp_nodelay>**: Set the socket ``TCP_NODELAY`` option (reference: `TCP_NODELAY Introduction <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_NODELAY>`_), default value: 0.

   -  0: Disable  TCP_NODELAY
   -  1: Enable TCP_NODELAY

-  **<so_sndtimeo>**: Set the socket ``SO_SNDTIMEO`` option (reference: `SO_SNDTIMEO Introduction <https://man7.org/linux/man-pages/man7/socket.7.html#SO_SNDTIMEO>`_), unit: milliseconds, default value: 0.

-  **<keep_alive>** : Set the socket ``SO_KEEPALIVE`` option (reference: `SO_KEEPALIVE Introduction <https://man7.org/linux/man-pages/man7/socket.7.html#SO_KEEPALIVE>`_), unit: seconds.

  - Range: [0,7200]。

    - 0: Disable keep-alive function; (default)
    - 1 ~ 7200: Enable keep-alive function.  `TCP_KEEPIDLE <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPIDLE>`_ value is **<keep_alive>**, `TCP_KEEPINTVL <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPINTVL>`_ value is 1,  `TCP_KEEPCNT <https://man7.org/linux/man-pages/man7/tcp.7.html#TCP_KEEPCNT>`_ value is 3.

The ``<keep_alive>`` parameter in this command is the same as the ``<keep_alive>`` parameter in the :ref:`AT+CIPSTART <cmd-START>` command. The final value is determined by the command that was set later. If the ``<keep_alive>`` parameter is not set when running this command, the last configured value will be used by default.

Notes
^^^^^^

-  Before setting socket options, **please fully understand the function of the option and the possible impacts after configuration.**。
-  SO_LINGER    The option is not recommended to be set with a large value. For example, setting the SO_LINGER value to 60 will cause the :ref:`AT+CIPCLOSE <cmd-CLOSE>` command to block for 60 seconds if the TCP FIN packet from the other end is not received, thus preventing the AT from responding to other commands. Therefore, the SO_LINGER option is recommended to be kept at its default value.
-  TCP_NODELAY  The option is suitable for scenarios with small throughput but high real-time requirements. After enabling it, :term:`LwIP` will speed up TCP transmission, but if the network environment is poor, throughput may decrease due to retransmission. Therefore, the TCP_NODELAY option is recommended to be kept at its default value.
-  SO_SNDTIMEO  The option is suitable for applications where the :ref:`AT+CIPSTART <cmd-START>` command does not set the keepalive parameter. After setting this option, the :ref:`AT+CIPSEND <cmd-SEND>`, :ref:`AT+CIPSENDL <cmd-SENDL>`, :ref:`AT+CIPSENDEX <cmd-SENDEX>` commands will exit within this timeout period, regardless of whether the send was successful. Here, it is recommended to configure the SO_SNDTIMEO option to 5 ~ 10 seconds.
-  SO_KEEPALIVE The option is suitable for applications that actively detect whether the connection is disconnected at regular intervals. It is usually recommended to configure this option when AT is used as a TCP server. After setting this option, additional network bandwidth will be increased. It is recommended that the SO_KEEPALIVE option be configured with a value not less than 60 seconds.
