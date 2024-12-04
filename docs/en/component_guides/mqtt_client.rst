
MQTT Client
==============

MQTT Client Programming Model
------------------------------------

The MQTT Client programming model for the W800 is illustrated below in the following diagram :

.. mermaid::
    :align: center

    graph LR

    A[Application task]-->|"API"|B[MQTT task]
    B[MQTT task]-->|"API"|A[Application task]
    B[MQTT task]-->|"API"|C[TCP/IP task]
    C[TCP/IP task]-->|"API"|B[MQTT task]
    B[MQTT task]-->|"event"|D[event task]
    D[event task]-->|"event"|A[Application task]

MQTT Client program can be seen as a black box that is unaware of the upper-layer code (such as the TCP/IP protocol stack, application tasks, event tasks, etc.). The application task is responsible for calling the MQTT Client program API to implement MQTT Client initialization, triggering events in the MQTT Client, and the application uses the triggered events to perform the next API call.



MQTT Client Introduction
------------------------------

The MQTT Client is an implementation of the `MQTT <https://mqtt.org/>`_ protocol client. MQTT is a lightweight messaging protocol based on the publish/subscribe model.

MQTT Client Features
------------------------------

    - Supports MQTT implementation based on TCP, SSL, Websocket, and Websocket Secure.
    - Supports subscription, publication, last will and testament, heartbeat maintenance, and three Quality of Service (QoS) levels.
    - Simplifies configuration processes through the use of URI.

MQTT Client Initialization
------------------------------

- Before using the MQTT Client connection, initialization must be performed. Call the ``wm_mqtt_client_init()`` function to complete the initialization configuration of the ``wm_mqtt_client_config_t`` structure and generate a handle for starting the MQTT Client.

- The ``wm_mqtt_client_deinit()`` function is used to destroy the client handle.

Initialization Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``wm_mqtt_client_config_t`` is initialized and configured with the following content:

====================        =================================
Field                         Description
====================        =================================
uri                          Uniform Resource Locator
client_id                    Client ID
username                     Username
password                     Password
will_topic                   Last Will topic
will_msg                     Last Will message
will_qos                     Quality of Service for Last Will Message
will_retain                  Retain Flag for Message
keep_alive                   Keep-Alive period
clean_session                Clean Session
debug                        Debug Print
auth_mode                    Authentication Mode for Connection
====================        =================================

Connection Authentication Mode
--------------------------------

==============================================     =================================
 WM_MQTT_CLIENT_AUTH_MODE_NOT_CERT                   No-Certificate Authentication
 WM_MQTT_CLIENT_AUTH_MODE_AUTH_SERVER_CERT           Validate Server Certificate
 WM_MQTT_CLIENT_AUTH_MODE_PROVIDE_CLIENT_CERT        Provide Client Certificate
 WM_MQTT_CLIENT_AUTH_MODE_MUTUAL_AUTH                Two-Way Authentication
==============================================     =================================

Message Service Quality
--------------------------------

- **QoS 0** (At most once)

 QoS 0 messages are fire-and-forget, requiring no acknowledgment, storage, or retransmission. This can result in message loss.

- **QoS 1** (At least once)   

To ensure message delivery, QoS 1 incorporates an acknowledgment and retransmission mechanism. The sender considers the message delivered only after receiving a PUBACK message from the receiver. Until then, the sender stores the PUBLISH message for potential retransmission.

- **QoS 2** (Only deliver once)

 Each QoS 2 message delivery requires at least two request/response exchanges between the sender and receiver, resolving the potential for duplicate messages in QoS 1.

Connection Address
-----------------------
MQTT connection addresses usually contain: server IP or domain name, server port, and connection protocol.

MQTT Connection Based on TCP 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``mqtt`` is a standard TCP connection, typically using port 1883.

- ``mqtts`` is a secure connection based on TLS/SSL, typically using port 8883.

::

    Example:  mqtt://broker.emqx.io:1883 is a TCP-based MQTT connection address.

MQTT Connection Based on WebSocket
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- ``ws`` is a standard WebSocket connection, typically using port 8083.

- ``wss`` is a secure WebSocket connection, typically using port 8084.

::

    Example: ws://broker.emqx.io:8083 is a WebSocket-based MQTT connection address.

MQTT Client Connection
--------------------------

- Call the ``wm_mqtt_client_connect()`` function, passing the initialized `handle` as a function parameter to complete the MQTT Client connection.

- Call the ``wm_mqtt_client_disconnect()`` function to disconnect the MQTT Client.

MQTT Client Subscription
--------------------------

.. hint:: The client must be connected before subscribing.

Call the  ``wm_mqtt_client_subscribe()`` function to subscribe to a topic. The function parameters are as follows:

=========       =====================================================
Parameter          Description
=========       =====================================================
handle          The handle generated during client initialization
topic           The topic to subscribe to
qos             The message service quality
=========       =====================================================

Call the ``wm_mqtt_client_unsubcribe()`` function to unsubscribe from a topic. The function parameters are the same as above.

MQTT Client Publishing
-----------------------

Call the ``wm_mqtt_client_publish()``  function to publish a message to the `MQTT broker`. The function parameters are as follows:

=========       =====================================================
Parameter          Description
=========       =====================================================
handle          The handle generated during client initialization
topic           The topic to publish to
message         The message string to publish
msg_len         The length of the message
qos             The message service quality
retain          Message retain flag
=========       =====================================================

Events
--------

MQTT Client may publish the following events:

========================================= ===================================================================================================================================================================
Event                                         Description
========================================= ===================================================================================================================================================================
WM_EVENT_MQTT_CLIENT_CONNECTED              The client has successfully connected to the server
WM_EVENT_MQTT_CLIENT_DISCONNECTED           The client has disconnected from the server
WM_EVENT_MQTT_CLIENT_DATA                   The client received a published message
WM_EVENT_MQTT_CLIENT_PUBLISHED              The server has received the client's published message.This event is only for QoS 1 and QoS 2 publications, as QoS 0 does not require message acknowledgment.
========================================= ===================================================================================================================================================================

Error Codes
----------------

There are many MQTT Client error codes are numerous. Here are some explanations:

.. list-table:: 
    :widths: 25 25 
    :header-rows: 0
    :align: center

    * - Error Code
      - Description

    * - **WM_MQTT_CLIENT_ERROR_SUCCESS**
      - Indicates no error, successfully completed the operation.

    * - **WM_MQTT_CLIENT_ERROR_FAILURE**
      - Indicates failure of the MQTT client operation.

    * - **WM_MQTT_CLIENT_ERROR_PERSISTENCE_ERROR**
      - Indicates an application-specific persistence error.
      
    * - **WM_MQTT_CLIENT_ERROR_DISCONNECTED**
      - This error occurs because the MQTT client is not connected.

    * - **WM_MQTT_CLIENT_ERROR_MAX_MESSAGES_INFLIGHT**
      - This error occurs because the maximum number of messages allowed to be sent simultaneously has been reached.

    * - **WM_MQTT_CLIENT_ERROR_BAD_UTF8_STRING**
      - This error usually occurs when an invalid UTF-8 string is detected in the input string.

    * - **WM_MQTT_CLIENT_ERROR_NULL_PARAMETER**
      - This error occurs because a null parameter was passed in.
      
    * - **WM_MQTT_CLIENT_ERROR_TOPICNAME_TRUNCATED**
      - Indicates the topic name was truncated due to an embedded NULL character.

    * - **WM_MQTT_CLIENT_ERROR_BAD_STRUCTURE**
      - This error occurs because the passed-in parameter structure is incorrect.

    * - **WM_MQTT_CLIENT_ERROR_BAD_QOS**
      - Indicates the QoS value is out of the specified range (`0`, `1`, `2``)

    * - **WM_MQTT_CLIENT_ERROR_SSL_NOT_SUPPORTED**
      - This error occurs when attempting to use SSL with a non-SSL version of the library.
      
    * - **WM_MQTT_CLIENT_ERROR_BAD_MQTT_VERSION**
      - This error occurs because the MQTT version is unrecognized.
      
    * - **WM_MQTT_CLIENT_ERROR_BAD_PROTOCOL**
      - Indicates an incorrect access protocol prefix( `mqtt` 、 `mqtt` 、 `ws` 、 `wss` )

    * - **WM_MQTT_CLIENT_ERROR_BAD_MQTT_OPTION**
      - This error occurs because the passed-in MQTT option is not applicable to the requested MQTT version.

    * - **WM_MQTT_CLIENT_ERROR_WRONG_MQTT_VERSION**
      - This error occurs because the call is not applicable to the requested MQTT version.

    * - **WM_MQTT_CLIENT_ERROR_0_LEN_WILL_TOPIC**
      - This error occurs because the WILL topic string length is 0.
