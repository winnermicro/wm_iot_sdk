
.. _MQTT-AT:
.. |Equipment-Name| replace:: w800

**********************
MQTT AT Command Set
**********************


-  :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>`: Set MQTT user Attributes
-  :ref:`AT+MQTTLONGCLIENTID <cmd-MQTTLONGCLIENTID>`: Set MQTT client ID
-  :ref:`AT+MQTTLONGUSERNAME <cmd-MQTTLONGUSERNAME>`: Set MQTT login username
-  :ref:`AT+MQTTLONGPASSWORD <cmd-MQTTLONGPASSWORD>`: Set MQTT login password
-  :ref:`AT+MQTTCONNCFG <cmd-MQTTCONNCFG>`: Set configuration of MQTT connection 
-  :ref:`AT+MQTTALPN <cmd-MQTTALPN>`: Set MQTT Application Layer Protocol Negotiation (ALPN)
-  :ref:`AT+MQTTCONN <cmd-MQTTCONN>`: Connect to MQTT Broker
-  :ref:`AT+MQTTPUB <cmd-MQTTPUB>`: Publish MQTT Messages in string
-  :ref:`AT+MQTTPUBRAW <cmd-MQTTPUBRAW>`: Publish long MQTT messages
-  :ref:`AT+MQTTSUB <cmd-MQTTSUB>`: Subscribe to MQTT Topic
-  :ref:`AT+MQTTUNSUB <cmd-MQTTUNSUB>`: Unsubscribe from MQTT Topic
-  :ref:`AT+MQTTCLEAN <cmd-MQTTCLEAN>`: Disconnect from MQTT
-  :ref:`MQTT AT Description <MQTTNote>`



.. _cmd-MQTTUSERCFG:

:ref:`AT+MQTTUSERCFG <MQTT-AT>`: Set MQTT User Attributes
-------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the MQTT user attributes that have been set

**Command:**

::

    AT+MQTTUSERCFG?    


**Response:**

::

    +MQTTUSERCFG:<LinkID>,<scheme>,<"client_id">,<"username">,<"password">,<cert_key_ID>,<CA_ID>,<"path">

    OK

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Configure MQTT user attributes

**Command:**

::

    AT+MQTTUSERCFG=<LinkID>,<scheme>,<"client_id">,<"username">,<"password">,<cert_key_ID>,<CA_ID>,<"path">

**Response:**

::

   OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<scheme>**:

   -  1: MQTT over TCP;
   -  2: MQTT over TLS(certificate unverified);
   -  3: MQTT over TLS(Verify Server Certificate);
   -  4: MQTT over TLS(provide client certificate);
   -  5: MQTT over TLS (verify the server certificate and provide the client certificate);
   -  6: MQTT over WebSocket(based on TCP);
   -  7: MQTT over WebSocket Secure(based on TLS, certificate unverified);
   -  8: MQTT over WebSocket Secure(based on TLS, verifying server certificates);
   -  9: MQTT over WebSocket Secure(based on TLS, providing client certificates);
   -  10: MQTT over WebSocket Secure(based on TLS, verify server certificates and provide client certificates).

-  **<client_id>**: MQTT client ID, maximum length: 256 bytes.
-  **<username>**: username, used to log in to MQTT broker, maximum length: 64 bytes.
-  **<password>**: Password, used to log in to MQTT broker, maximum length: 64 bytes.
-  **<cert_key_ID>**: Certificate ID. Currently, |Equipment-Name|-AT only supports one set of cert certificates with parameter 0.
-  **<CA_ID>**: CA ID, currently |Equipment-Name|-AT only supports one set of CA certificates, with a parameter value of 0.
-  **<path>**: Resource path, maximum length: 32 bytes.

Explaination
^^^^^^^^^^^^^^^

-  The total length of each AT command cannot exceed 256 bytes.
-  If the ``<scheme>`` is configured as 3, 5, 8, or 10,  in order to verify the validity period of the server's certificate, please ensure that has obtained the current time before sending the command :ref:`AT+MQTTCONN <cmd-MQTTCONN>` . (You can send the command :ref:`AT+CIPSNTPCFG <cmd-SNTPCFG>` to configure SNTP and obtain the current time, and send the command :ref:`AT+CIPSNTPTIME? <cmd-SNTPT>` to query the current time.If you are in a LAN or in an environment where connecting to an NTP server is not possible, you can modify the MBEDTLS_HAVE_TIME_DATE option in wm_mbedtls_config.h to disable the certificate validity period check.)

.. _cmd-MQTTLONGCLIENTID:

:ref:`AT+MQTTLONGCLIENTID <MQTT-AT>`: Set MQTT Client ID
------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Set MQTT client ID

**Command:**

::

    AT+MQTTLONGCLIENTID=<LinkID>,<length>

**Response:**

::

    OK

    >

The above response indicates that the AT is ready to receive the MQTT client ID. At this point, you can enter the client ID. When the length of the client ID received by the AT reaches ``<length>``, return:

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<length>**: MQTT client ID length. Scope: [1,1024].

Explaination
^^^^^^^^^^^^^^^

- The command :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` can also set the MQTT client ID, and the differences between the two include:

    - The command :ref:`AT+MQTTLONGCLIENTID <cmd-MQTTLONGCLIENTID>` can be used to set relatively long client IDs because the length of the :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` command is limited;
    - After setting :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` use :ref:`AT+MQTTLONGCLIENTID <cmd-MQTTLONGCLIENTID>`。

.. _cmd-MQTTLONGUSERNAME:

:ref:`AT+MQTTLONGUSERNAME <MQTT-AT>`: Set MQTT Login Username
-----------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Set MQTT username

**Command:**

::

    AT+MQTTLONGUSERNAME=<LinkID>,<length>

**Response:**

::

    OK

    >

The above response indicates that the AT is ready to receive the MQTT username. At this point, you can enter the MQTT username. When the length of the MQTT username received by the AT reaches ``<length>`` , return:

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<length>**: MQTT username length. Scope: [1,1024].

Explaination
^^^^^^^^^^^^^^^

-  The command :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` can also set the MQTT username, and the differences between the two include:

   - The command :ref:`AT+MQTTLONGUSERNAME <cmd-MQTTLONGUSERNAME>` can be used to set relatively long usernames because the length of the :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` command is limited;
   - After setting :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` use :ref:`AT+MQTTLONGUSERNAME <cmd-MQTTLONGUSERNAME>`。

.. _cmd-MQTTLONGPASSWORD:

:ref:`AT+MQTTLONGPASSWORD <MQTT-AT>`: Set MQTT Login Password
-------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Set MQTT password

**Command:**

::

    AT+MQTTLONGPASSWORD=<LinkID>,<length>

**Response:**

::

    OK

    >

The above response indicates that the AT is ready to receive the MQTT password. At this point, you can enter the MQTT password. When the length of the MQTT password received by the AT reaches ``<length>`` , return:

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<length>**: MQTT password length. Scope: [1,1024].

Explaination
^^^^^^^^^^^^^^^

-  The command :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` can also set the MQTT password, and the differences between the two include:

   - The command :ref:`AT+MQTTLONGPASSWORD <cmd-MQTTLONGPASSWORD>` can be used to set relatively long usernames because the length of the :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` command is limited;
   - After setting :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` use :ref:`AT+MQTTLONGPASSWORD <cmd-MQTTLONGPASSWORD>`。

.. _cmd-MQTTCONNCFG:

:ref:`AT+MQTTCONNCFG <MQTT-AT>`: Set MQTT Connection Properties
-------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the MQTT connection properties that have been set

**Command:**

::

    AT+MQTTCONNCFG?    


**Response:**

::

    +MQTTCONNCFG:<LinkID>,<keepalive>,<disable_clean_session>,<"lwt_topic">,<"lwt_msg">,<lwt_qos>,<lwt_retain>

    OK

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Set MQTT connection properties

**Command:**

::

    AT+MQTTCONNCFG=<LinkID>,<keepalive>,<disable_clean_session>,<"lwt_topic">,<"lwt_msg">,<lwt_qos>,<lwt_retain>

**Response:**

::

   OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<keepalive>**: MQTT ping timeout, unit: seconds. Scope: [0,7200]. Default: 0, It will be forcibly changed to 120 seconds.
-  **<disable_clean_session>**: Set MQTT clean session flag. For more information about this parameter, please refer to the  `Clean Session <http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.pdf>`_ chapter.

    - 0: Enable session cleaning
    - 1: Disable cleaning sessions

-  **<lwt_topic>**:  Last Will and Testament topic, maximum length: 128 bytes.
-  **<lwt_msg>**:  Last Will and Testament message, maximum length: 128 bytes.
-  **<lwt_qos>**:  Last Will and Testament QoS, optional parameters 0, 1, 2, default value: 0.
-  **<lwt_retain>**: Last Will and Testament retain flag, optional parameter 0 or 1, default value: 0.

.. _cmd-MQTTALPN:

:ref:`AT+MQTTALPN <MQTT-AT>`: Set MQTT Application Layer Protocol Negotiation (ALPN)
-------------------------------------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query the MQTT Application Layer Protocol Negotiation (ALPN) that has been set up

**Command:**

::

    AT+MQTTALPN?    

**Response:**

::

    +MQTTALPN:<LinkID>,<"alpn">
    +MQTTALPN:<LinkID>,<"alpn">
    +MQTTALPN:<LinkID>,<"alpn">
    +MQTTALPN:<LinkID>,<"alpn">
    +MQTTALPN:<LinkID>,<"alpn">

    OK

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Set up MQTT Application Layer Protocol Negotiation (ALPN)

**Command:**

::

    AT+MQTTALPN=<LinkID>,<alpn_counts>[,<"alpn">][,<"alpn">][,<"alpn">]

**Response:**

::

   OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<alpn_counts>**: Number of parameters for<"alpn"> . Scope: [0,5].

   - 0: Clear MQTT ALPN configuration
   - [1,5]: Set MQTT ALPN configuration

-  **<"alpn">**: A string parameter that represents the ALPN in ClientHello. Users can send multiple ALPN fields to the server.

Explaination
^^^^^^^^^^^^^^^

- The length of the entire AT command should be less than 256 bytes.
- The MQTT ALPN field only takes effect when MQTT is based on TLS or WSS.
- After  setting :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>` use :ref:`AT+MQTTALPN <cmd-MQTTALPN>`。

Example
^^^^^^^^^^^^

::

    AT+CWMODE=1
    AT+CWJAP="ssid","password"
    AT+CIPSNTPCFG=1,8,"ntp1.aliyun.com","ntp2.aliyun.com"
    AT+MQTTUSERCFG=0,5,"w800","WinnerMicro","1234567890",0,0,""
    AT+MQTTALPN=0,2,"mqtt-ca.cn","mqtt-ca.us"
    AT+MQTTCONN=0,"192.168.200.2",8883,1

.. _cmd-MQTTCONN:

:ref:`AT+MQTTCONN <MQTT-AT>`: Connect to MQTT Broker
--------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Query |Equipment-Name| MQTT brokers connected to the device

**Command:**

::

    AT+MQTTCONN?

**Response:**

::

    +MQTTCONN:<LinkID>,<state>,<scheme><"host">,<port>,<"path">,<reconnect>
    OK

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Connect to MQTT Broker

**Command:**

::

    AT+MQTTCONN=<LinkID>,<"host">,<port>,<reconnect>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<host>**: MQTT broker domain name, maximum length: 128 bytes,does not support setting an empty string.
-  **<port>**: MQTT broker port, maximum port: 65535.
-  **<path>**: Resource path, maximum length: 32 bytes.
-  **<reconnect>**:

   -  0: MQTT does not automatically reconnect. If MQTT establishes a connection but then disconnects, it cannot be reconnected using this command. You need to first send the command :ref:`AT+MQTTCLEAN=0 <cmd-MQTTCLEAN>` to clear the information, reconfigure the parameters, and then establish a new connection.
   -  1: MQTT automatic reconnection will consume more memory resources.

-  **<state>**: MQTT status:

   -  0: MQTT not initialized;
   -  1: Already set :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>`；
   -  2: Already set :ref:`AT+MQTTCONNCFG <cmd-MQTTCONNCFG>`；
   -  3: The connection has been disconnected;
   -  4: Connection established;
   -  5: Connected but not subscribed to the topic;
   -  6: Connected and subscribed to the topic.

-  **<scheme>**：

   -  1: MQTT over TCP；
   -  2: MQTT over TLS(Do not validate certificate);
   -  3: MQTT over TLS (Validate Server Certificate);
   -  4: MQTT over TLS (provide client certificate);
   -  5: MQTT over TLS (Validate server certificate and provide client certificate);
   -  6: MQTT over WebSocket (Based on TCP);
   -  7: MQTT over WebSocket Secure (Based on TLS, certificate unverified);
   -  8: MQTT over WebSocket Secure (Based on TLS, verifying server certificates);
   -  9: MQTT over WebSocket Secure (Based on TLS, providing client certificates);
   -  10: MQTT over WebSocket Secure (Based on TLS, verifying server certificates and providing client certificates). 

.. _cmd-MQTTPUB:

:ref:`AT+MQTTPUB <MQTT-AT>`: Publish MQTT Message in String
---------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Publish MQTT  **string** messages through topic. If the amount of data you post is relatively large and exceeds the length threshold of ``256`` bytes for a single AT instruction, please use the :ref:`AT+MQTTPUBRAW <cmd-MQTTPUBRAW>` 命令。

**Command:**

::

    AT+MQTTPUB=<LinkID>,<"topic">,<"data">,<qos>,<retain>

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<topic>**: MQTT topic, maximum length: 128 bytes,does not support setting an empty string.
-  **<data>**: MQTT string message.
-  **<qos>**: QoS for publishing messages, with optional parameters of 0, 1, or 2, default value: 0.
-  **<retain>**: Release retain.

Explaination
^^^^^^^^^^^^^^^

-  The total length of each AT command cannot exceed 256 bytes.
-  This command cannot send data ``\0``. If you need to send this data, please use the :ref:`AT+MQTTPUBRAW <cmd-MQTTPUBRAW>` command.

Example
^^^^^^^^^^^^

::

    AT+CWMODE=1
    AT+CWJAP="ssid","password"
    AT+MQTTUSERCFG=0,1,"w800","WinnerMicro","1234567890",0,0,""
    AT+MQTTCONN=0,"192.168.10.234",1883,0
    AT+MQTTPUB=0,"topic","\"{\"timestamp\":\"20201121085253\"}\"",0,0  // When sending this command, please pay attention to whether special characters need to be escaped.

.. _cmd-MQTTPUBRAW:

:ref:`AT+MQTTPUBRAW <MQTT-AT>`: Publish Long MQTT Messages
------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Publish long MQTT messages through topics. If the amount of data you post is relatively small, not exceeding the length threshold of ``256`` bytes for a single AT instruction, you can also use the :ref:`AT+MQTTPUB <cmd-MQTTPUB>` command.

**Command:**

::

    AT+MQTTPUBRAW=<LinkID>,<"topic">,<length>,<qos>,<retain>

**Response:**

::

    OK
    > 

The symbol ``>`` indicates that the AT is ready to receive serial data. At this point, you can input data, and when the data length reaches the value of the parameter ``<length>`` , data transmission begins.

If the transmission is successful, AT returns:

::

    +MQTTPUB:OK

If the transmission fails, AT returns:

::

    +MQTTPUB:FAIL

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<topic>**: MQTT topic, maximum length: 128 bytes,does not support setting an empty string.
-  **<length>**: MQTT message length ,|Equipment-Name| The maximum length of a device is limited by available memory.
-  **<qos>**: QoS for publishing messages, with optional parameters of 0, 1, or 2, default value: 0.
-  **<retain>**: Publish retain.

.. _cmd-MQTTSUB:

:ref:`AT+MQTTSUB <MQTT-AT>`: Subscribe to MQTT Topic
--------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^

**Function:**

Search for subscribed topics

**Command:**

::

    AT+MQTTSUB?    


**Response:**

::

    +MQTTSUB:<LinkID>,<state>,<"topic1">,<qos>
    +MQTTSUB:<LinkID>,<state>,<"topic2">,<qos>
    +MQTTSUB:<LinkID>,<state>,<"topic3">,<qos>
    ...
    OK

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Subscribe to specified QoS for MQTT topics and support subscribing to multiple topics

**Command:**

::

    AT+MQTTSUB=<LinkID>,<"topic">,<qos>


**Response:**

::

    OK

When AT receives an MQTT message for a subscribed topic, it returns:

::

    +MQTTSUBRECV:<LinkID>,<"topic">,<data_length>,data

If the topic has already been subscribed to, return:

::

   ALREADY SUBSCRIBE

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<state>**: MQTT status:

   -  0: MQTT not initialized;
   -  1: Already set: :ref:`AT+MQTTUSERCFG <cmd-MQTTUSERCFG>`；
   -  2: Already set: :ref:`AT+MQTTCONNCFG <cmd-MQTTCONNCFG>`；
   -  3: The connection has been disconnected;
   -  4: Connection established;
   -  5: Connected but not subscribed to the topic;
   -  6: Connected and subscribed to MQTT topic.

-  **<topic>**: The subscribed topic, does not support setting an empty string.
-  **<qos>**: QoS for subscription.

Explaination
^^^^^^^^^^^^^^^

-  You can subscribe to up to 10 topics, and when the subscription is full, you will receive a prompt: ``ALREADY FULL SUBSCRIBE``.

.. _cmd-MQTTUNSUB:

:ref:`AT+MQTTUNSUB <MQTT-AT>`: Unsubscribe from MQTT Topic
--------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

The client can unsubscribe from a specified topic by calling this command multiple times to unsubscribe from different topics.

**Command:**

::

    AT+MQTTUNSUB=<LinkID>,<"topic">


**Response:**

::

    OK

If the topic has not been subscribed to, return:

::

  NO UNSUBSCRIBE 
  
  OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.
-  **<topic>**: MQTT topic, maximum length: 128 bytes, does not support setting an empty string.

.. _cmd-MQTTCLEAN:

:ref:`AT+MQTTCLEAN <MQTT-AT>`: Disconnect from MQTT
------------------------------------------------------------

Set Command
^^^^^^^^^^^^^
^^^

**Function:**

Disconnect MQTT connection and release resources.

**Command:**

::

    AT+MQTTCLEAN=<LinkID>  

**Response:**

::

    OK

Parameters
^^^^^^^^^^^^

-  **<LinkID>**: Currently only supports link ID 0.

.. _MQTTNote:

:ref:`MQTT AT Description <MQTT-AT>`
-----------------------------------------

-  Generally speaking, AT MQTT commands respond within 10 seconds, except for the command :ref:`AT+MQTTCONN <cmd-MQTTCONN>` . For example, if the router cannot access the internet, the command :ref:`AT+MQTTPUB <cmd-MQTTPUB>` will respond within 10 seconds, but the command :ref:`AT+MQTTCONN <cmd-MQTTCONN>` may take more time to retransmit packets in the poor network environments.
-  If :ref:`AT+MQTTCONN <cmd-MQTTCONN>` is based on TLS connection and the timeout time for each packet is 10 seconds, the total timeout time will become longer based on the number of handshake packets.
-  When the MQTT connection is disconnected, it will prompt the message  ``+MQTTDISCONNECTED:<LinkID>`` .
-  When an MQTT connection is established, it will prompt the message  ``+MQTTCONNECTED:<LinkID>,<scheme>,<"host">,port,<"path">,<reconnect>`` .