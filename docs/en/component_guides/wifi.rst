
WiFi
===========

WiFi Programming Model
--------------------------------

.. only:: w800

    The WiFi programming model for the W800 is shown in the following figure:
    
.. mermaid::
    :align: center

    graph LR

    A[TCP stack]-->|"event"|B[Event task]
    B-->|"event"|C[Application task]
    D[WPA]-->|"event"|B
    D-->|"callback"|E[WiFi]
    E-->|"callback"|D
    C-->|"API"|E

The WiFi program can be viewed as a black box that is unaware of the upper-layer code, such as the TCP/IP protocol stack, application tasks, event tasks, etc. Typically, the application task (code) is responsible for invoking the WiFi program API to initialize the WiFi. The WiFi triggers callbacks in WPA, which in turn trigger WiFi events. The WiFi program then receives and processes the WPA data, and the application receives a response.

WiFi Connection Configuration
-------------------------------------

The WiFi connection configuration mainly consists of the following components:

1. WiFi tx buf number

This represents the number of transmit buffers at the WiFi MAC layer, primarily used to store data that has not yet been sent out by the  ``MAC`` . Users can modify this based on actual needs.

2. WiFi rx buf number

This represents the number of receive buffers at the WiFi MAC layer, primarily used to store data received by the  ``MAC``  layer that has not yet been sent to the application layer. Users can modify this based on actual needs.

Meunconfig configuration of WiFi
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The main configurations are as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration name
    - Configuration description
    - Default values

  * - CONFIG_COMPONENT_WIFI_ENABLED
    - Whether to enable the WiFi component
    - N

  * - CONFIG_WIFI_MASCW_TX_BUF_NUM 
    - The number of sending buffers at the WiFi MAC layer is specified
    - 7

  * - CONFIG_WIFI_MASCW_RX_BUF_NUM 
    - The number of receiving buffers at the WiFi MAC layer is specified
    - 3

  * - CONFIG_WIFI_API_ENABLED 
    - Whether to enable the WiFi API
    - Y

WiFi Initialization
-------------------------------------

Before using WiFi, it is necessary to perform initialization by calling the ``wm_wifi_init()`` function.

WiFi Scan Mode
-------------------------------------

The ``wm_wifi_scan_start()`` API is supported only in station mode or station/AP coexistence mode.
Note that scanning may fail when the station is networked.

Scan Type
~~~~~~~~~~~~~~~

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Modes
     - Description
     
   * - Active Scanning
     - Scanning is performed by sending probe requests. This mode is the default scanning mode.
     
   * - Passive Scanning
     - Do not send probe requests. It Jumps to a specific channel and wait for beacons. Application can enable passive scanning through the ``wm_wifi_scan_config_t`` field in ``wm_wifi_scan_type_t``.

   * - Full Channel Scanning
     - When the ``channel`` field in ``wm_wifi_scan_config_t`` is set to 0, the current mode is full channel scanning.
     
   * - Specific channel scanning
     -  When the ``channel`` field in ``wm_wifi_scan_config_t`` is set 1-14, the current mode is fast scanning.

   

Scan configuration
~~~~~~~~~~~~~~~~~~~~~~

The scan type and other scan properties are configured through the  ``wm_wifi_scan_start()`` function. The table below provides a detailed description of the field information for the ``wm_wifi_scan_config_t`` function.

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Field
     - Description

   * - ssid
     - If the value of this field is not NULL, only APs with the same SSID value can be scanned.

   * - bssid
     - If the value of this field is not NULL, only APs with the same BSSID value can be scanned.

   * - channel
     - If the value of this field is 0, a full channel scan will be performed; On the contrary, scanning will be conducted on specific channels.
     
   * - show_hidden
     - If the value of this field is 0, APs with hidden SSIDs will be ignored during this scan; On the contrary, these APs will also be considered as normal APs during scanning.

   * - scan_type
     - If the value of this field is ``WM_WIFI_SCAN_TYPE_ACTIVE``, it indicates active scanning, ``WM_WIFI_SCAN_TYPE_PASSIVE`` will be considered as passive scanning.

   * - scan_time
     -  This field is used to control the scanning time for each channel.

        During passive scanning, the scan_time.assive field is responsible for specifying the scanning time for each channel.

        When actively scanning, the scanning time for each channel is shown in the following list. Among them, min represents scan_time-active_min, and max represents scan_time-active_max.

        min=0, max=0：The scanning time for each channel is 120 ms.

        min>0, max=0：The scanning time for each channel is 120 ms.

        min=0, max>0：The scanning time for each channel is max ms.

        min>0, max>0：The minimum scanning time for each channel is min ms. If no AP is found during this period, it will jump to the next channel. If an AP is found during this period, the scanning time for that channel is max ms.

        If you want to improve Wi Fi scanning performance, you can modify the above two parameters.

   * - home_chan_dwell_time
     - The time spent scanning consecutive channels before returning to the home channe.

Event Handling Phase After Scan Completion
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When all channels have been scanned, the ``WM_EVENT_WIFI_SCAN_DONE`` event will be generated..

The event callback function of the application notifies that the application task has received the ``WIFI_EVENT_SCAN_DONE`` event. 
The function ``wm_wifi_scan_get_ap_num()``  is then called to obtain the number of APs (Access Points) found during this scan. 
Afterward, allocate sufficient transaction slots and invoke the function ``wm_wifi_scan_get_ap_records()`` to retrieve the AP records. 
Please note that after the scan is completed, call the ``wm_wifi_scan_get_ap_records()`` function as soon as possible to retrieve AP records. The records will be automatically released after a period of time, but can be retrieved repeatedly before then.


WiFi STA Mode
------------------

STA Connection Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Call the ``wm_wifi_set_config()`` function to configure STA. The configurable content for STA mode is as follows:

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Field
     - Description

   * - ssid
     - The SSID of the target AP field.

   * - password
     - The password for the target AP.

   * - scan_method
     - Whether to perform  an all-channel scan or quick scan.

   * - bssid_set
     - Indicates whether the MAC address of the target AP is set. Generally, the bssid_set in station_config should be set to 0. It only needs to be set to 1 when the user needs to view the MAC address of the AP.
     
   * - bssid
     - The MAC address of the target AP.

   * - channel 
     - The channel of the target AP. Set to 1~13, start scanning from the specified channel, and then connect to the AP.
       If the channel of the AP is unknown, set it to 0.

   * - listen_interval
     - The listen interval for receiving beacons when WM_WIFI_PS_MAX_MODEM is set, measured in AP beacon intervals. If set to 0, the default value is 10.
   
   * - sort_method
     - Sort the connected APs in the list by RSSI or security mode.

   * - threshold
     - When sort_method is set,only APs with an authentication modes that is more secure than the selected one and a signal stronger than the minimum RSSI will be used.

   * - pmf_cfg
     - Configure the Protected Management Frame(PMF).This will be advertised in the RSN capabilities within the RSN Information Element (IE).

Usually, configuration is required before establishing a WiFi connection. But if you have already saved the configuration in :ref: `NVS<nvs>` or want to save it, you don't need to configure it on the next connection, which is also possible.

STA Connection
~~~~~~~~~~~~~~~~~~~

Invoke the ``wm_wifi_connect()`` function to connect to an AP. If the connection is successful, the ``WM_EVENT_WIFI_STA_CONNECTED`` event will be returned,
Subsequently, the event task will initiate the DHCP client service, ultimately triggering the DHCP process. If the connection fails for some reason, such as incorrect password, wrong channel, or AP not found, the  ``WM_EVENT_WIFI_STA_DISCONNECTED`` event will be returned with a corresponding failure reason.

STA Obtaining IP
~~~~~~~~~~~~~~~~~~~

Once the DHCP client initialization is complete, the WiFi will enter the phase of obtaining an IP address. If the WiFi successfully receives an IP address from the DHCP server, it will trigger the ``WM_EVENT_WIFI_STA_GOT_IP`` or ``WM_EVENT_WIFI_STA_GOT_IP6`` event, indicating a successful connection to the AP.

STA Disconnection
~~~~~~~~~~~~~~~~~~~

You can invoke the ``wm_wifi_disconnect()`` function to actively disconnect from the WiFi. The WiFi may also disconnect due to certain reasons (e.g., AP power failure, weak RSSI, etc.), which will generate the ``WM_EVENT_WIFI_STA_DISCONNECTED`` event.

WiFi SoftAP Mode
----------------------

SoftAP Configuration
~~~~~~~~~~~~~~~~~~~~~~

Invoke the ``wm_wifi_set_config()`` function to configure WiFi in SoftAP mode. The configurable contents for AP mode are as follows:

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Field
     - Description

   * - ssid
     - The SSID of the SoftAP. If the `ssid_len`` is 0, it must be a null-terminated string. Otherwise, 
     
       the length is set according to `ssid_len`.

   * - password
     - The password for the SoftAP.

   * - ssid_len
     - The optional SSID field length.

   * - channel
     - The channel for the SoftAP.

   * - authmode
     - The authentication mode for the SoftAP.

   * - ssid_hidden 
     - Whether to broadcast the SSID. The default is 0, which means to broadcast the SSID.

   * - max_connection
     - The maximum number of STA allowed to connect. The default is 4, with a maximum of 8.

   * - beacon_interval
     - The beacon interval, which should be a multiple of 100. The unit is TU (Time Unit, 1 TU = 1024 microseconds). The range is 100 to 60000. The default value is 100. 
     
       
   * - pairwise_cipher
     - The Pairwise cipher for the SoftAP, from which the group cipher will be derived.

Configuration is typically required before creating the SoftAP. However, if the configuration has already been saved in :ref: `NVS <nvs>`, or if you wish to save the configuration for future connections without re-configuring, that is also acceptable.

Start SoftAP 
~~~~~~~~~~~~~~~

Call the ``wm_wifi_ap_start()`` function to start the SoftAP. If the SoftAP starts successfully, the ``WM_EVENT_WIFI_AP_START`` event will be returned. 
If it fails to start for some reasons, such as configuration errors or hardware damage, the ``WM_EVENT_WIFI_AP_STOP`` event will be returned along with a corresponding failure reason.

SoftAP Connection
~~~~~~~~~~~~~~~~~~~~~

Once a STA connects to the SoftAP, the ``WM_EVENT_WIFI_AP_STACONNECTED`` event will be returned, After successfully assigning an IP to the STA, the ``WM_EVENT_WIFI_AP_STAIPASSIGNED`` event will be triggered. If the STA disconnects for any reason, the ``WM_EVENT_WIFI_AP_STADISCONNECTED`` event will be returned.

Stop SoftAP 
~~~~~~~~~~~~~~~~~~~~~

You can actively stop the SoftAP by calling the ``wm_wifi_ap_stop()`` function. Alternatively, abnormal situations such as power outages or restarts that cause the SoftAP to be shut down will trigger the ``WM_EVENT_WIFI_AP_STOP``  event.

WiFi Event Description
----------------------------------

The following are events in WiFi:

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Event
     - Description

   * - WM_EVENT_WIFI_SCAN_DONE
     - WiFi has completed scanning for APs.

   * - WM_EVENT_WIFI_STA_CONNECTED
     - STA has connected to the AP.

   * - WM_EVENT_WIFI_STA_DISCONNECTED
     - STA has disconnected from the AP.

   * - WM_EVENT_WIFI_STA_BSS_RSSI_LOW
     - The signal strength of the STA's connection to the AP is too low.

   * - WM_EVENT_WIFI_STA_BEACON_TIMEOUT
     - STA timed out while waiting for beacon frames from the AP.

   * - WM_EVENT_WIFI_AP_START
     - SoftAP has started.

   * - WM_EVENT_WIFI_AP_STOP
     - SoftAP has stopped.

   * - WM_EVENT_WIFI_AP_STACONNECTED
     - A STA has successfully connected to the SoftAP in SoftAP mode.

   * - WM_EVENT_WIFI_AP_STADISCONNECTED
     - A STA has disconnected from the SoftAP in SoftAP mode.

   * - WM_EVENT_WIFI_STA_GOT_IP
     - STA has obtained an IPv4 IP from the AP.

   * - WM_EVENT_WIFI_STA_GOT_IP6
     - STA has obtained an IPv6 IP from the AP.

   * - WM_EVENT_WIFI_STA_LOST_IP
     - STA has lost its IPv4 IP from the AP.

   * - WM_EVENT_WIFI_STA_LOST_IP6
     - STA has lost its IPv6 IP from the AP.

   * - WM_EVENT_WIFI_AP_STAIPASSIGNED
     - SoftAP has assigned an IP to a connected STA.

   * - WM_EVENT_WIFI_ONESHOT_SCAN_DONE
     - Scanning is complete in OneShot mode.

   * - WM_EVENT_WIFI_ONESHOT_FOUND_CHANNEL
     - A channel has been acquired in OneShot mode.

   * - WM_EVENT_WIFI_ONESHOT_GOT_SSID_PSWD
     - SSID and Password have been obtained in OneShot mode.

   * - WM_EVENT_WIFI_ONESHOT_GOT_CUSTOM_DATA
     - Custom data has been obtained in OneShot mode.

   * - WM_EVENT_WIFI_ONESHOT_SEND_REPLY_DONE
     - Sending ACK is complete in OneShot mode.

WiFi Error Codes
-------------------

.. only:: w800

    There are many WiFi error codes for W800, which are explained here:

    .. list-table::
        :header-rows: 1
        :widths: 25 5 55
        :align: center

        * - Error Code
          - Code Value
          - Description

        * - **WM_WIFI_REASON_UNSPECIFIED**
          - 1
          - An internal error occurred, such as memory full, internal send failure, or the reason was already received remotely.

        * - **WM_WIFI_REASON_AUTH_EXPIRE**
          - 2
          - The previous authentication has expired.

        * - **WM_WIFI_REASON_AUTH_LEAVE**
          - 3
          - Authentication was canceled because the sending station is leaving (or has left).

        * - **WM_WIFI_REASON_ASSOC_EXPIRE**
          - 4
          - Association was canceled because the AP is not active.

        * - **WM_WIFI_REASON_ASSOC_TOOMANY**
          - 5
          - Association was canceled because the AP cannot handle all currently associated STAs.

        * - **WM_WIFI_REASON_NOT_AUTHED**
          - 6
          - Association was canceled because the AP did not authenticate.

        * - **WM_WIFI_REASON_NOT_ASSOCED**
          - 7
          - Association was canceled because the AP was not associated.

        * - **WM_WIFI_REASON_ASSOC_LEAVE**
          - 8
          - Association was canceled because the AP association left.

        * - **WM_WIFI_REASON_ASSOC_NOT_AUTHED**
          - 9
          - Association was canceled because the AP association was not authenticated

        * - **WM_WIFI_REASON_DISASSOC_PWRCAP_BAD**
          - 10
          - Association was canceled due to insufficient AP power.

        * - **WM_WIFI_REASON_DISASSOC_SUPCHAN_BAD**
          - 11
          - Association was canceled due to insufficient AP-supported channels.

        * - **WM_WIFI_REASON_BSS_TRANSITION_DISASSOC**
          - 12
          - STA disconnected from association due to BSS transition.

        * - **WM_WIFI_REASON_IE_INVALID**
          - 13
          - Association was canceled because the IE sent by the AP is invalid.

        * - **WM_WIFI_REASON_MIC_FAILURE**
          - 14
          - Association was canceled because the MIC sent by the AP is invalid.

        * - **WM_WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT**
          - 15
          - Association was canceled due to AP handshake timeout.

        * - **WM_WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT**
          - 16
          - Group-Key handshake timeout.

        * - **WM_WIFI_REASON_IE_IN_4WAY_DIFFERS**
          - 17
          - Elements generated during the four-way handshake differ from those in the (re-)association request/probe and response/beacon frames.

        * - **WM_WIFI_REASON_GROUP_CIPHER_INVALID**
          - 18
          - STA disconnected from association due to invalid Group Cipher.

        * - **WM_WIFI_REASON_PAIRWISE_CIPHER_INVALID**
          - 19
          - STA disconnected from association due to invalid Pairwise Cipher.

        * - **WM_WIFI_REASON_AKMP_INVALID**
          - 20
          - STA disconnected from association due to invalid AKMP.

        * - **WM_WIFI_REASON_UNSUPP_RSN_IE_VERSION**
          - 21
          - STA disconnected from association due to unsupported RSN IE version.

        * - **WM_WIFI_REASON_INVALID_RSN_IE_CAP**
          - 22
          - STA disconnected from association due to invalid RSN IE capabilities.

        * - **WM_WIFI_REASON_CIPHER_SUITE_REJECTED**
          - 24
          - STA disconnected from association due to rejected Cipher Suite.

        * - **WM_WIFI_REASON_UNSPECIFIED_QOS_REASON**
          - 32
          - STA disconnected from association due to unspecified QoS-related reasons.

        * - **WM_WIFI_REASON_NOT_ENOUGH_BANDWIDTH**
          - 33
          - STA disconnected from association due to lack of sufficient bandwidth for the QoS AP to use for the QoS STA.

        * - **WM_WIFI_REASON_DISASSOC_LOW_ACK**
          - 34
          - STA disconnected from association due to low ACK.
        * - **WM_WIFI_REASON_EXCEEDED_TXOP**
          - 35
          - STA disconnected from association due to the STA's transmissions exceeding the TXOPs limit.

        * - **WM_WIFI_REASON_STA_LEAVING**
          - 36
          - The requesting STA left the BSS or was reset.

        * - **WM_WIFI_REASON_TIMEOUT**
          - 39
          - The request from the peer STA timed out.

        * - **WM_WIFI_REASON_INVALID_PMKID**
          - 49
          - Invalid Pairwise Master Key Identifier (PMKID).

        * - **WM_WIFI_REASON_NO_AP_FOUND**
          - 200
          - This error code means that no AP was found. The main reason for this error is that the AP does not exist or there is an error in the security configuration of the AP. 

        * - **WM_WIFI_REASON_AUTH_FAIL**
          - 201
          - This error code means that authentication failed. The reasons for this error may be incorrect passwords or errors in the security configuration of the AP.

        * - **WM_WIFI_REASON_ASSOC_FAIL**
          - 202
          - Association failed, but not caused by ASSOC_EXPIRE or ASSOC_TOOMANY.

        * - **WM_WIFI_REASON_BEACON_TIMEOUT**
          - 203
          - This error code means that beacon frames from the AP cannot be received. The main reason for this error is that the AP is powered off.
            
          

WiFi APSTA Mode
-----------------

.. only:: w800

    To enable APSTA mode, the recommended operation steps are to connect to an AP in STA mode first, and after obtaining the channel information of the AP, configure the SoftAP based on the obtained channel information. Since W800 currently has only one RF and does not support STA and SoftAP operating on different channels, when STA and SoftAP are set on two different channels, SoftAP may not work properly.


Other operations are the same as for STA and SoftAP modes and will not be further explained here.

WiFi Sniffer Mode
--------------------

WiFi Sniffer mode can be enabled by ``wm_wifi_set_promiscuous()``.

.. only:: w800

    W800 can receive management frames and data frames, but not control frames.

The received frame consists of frame information and frame data. The frame information includes the following:

.. list-table::
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - Field
     - Description

   * - rssi
     - The RSSI of the message. Unit: dBm.

   * - rate
     - The PHY rate encoding of the data packet.

   * - sig_mode
     - 0 for non-HT (11bg) packets; 1 for HT (11n) packets.

   * - mcs
     - Modulation and Coding Scheme.

   * - cwb 
     - The Channel bandwidth of the packet.

   * - smoothing
     - ----------。

   * - not_sounding
     - ----------。

   * - aggregation
     - 0 for MPDU packets; 1 for AMPDU packets.。

   * - stbc
     - 0 for non-STBC packets; 1 for STBC packets.

   * - fec_coding
     - Indicates whether the packet is LDPC-encoded in 11n.

   * - sgi
     - Short Guard Interval.

   * - noise_floor
     - The noise floor of the radio frequency module (RF). Unit: 0.25 dBm.

   * - ampdu_cnt
     - The number of aggregated frames.

   * - channel 
     - The primary channel  on which the packet was received.

   * - secondary_channel
     - The secondary channel on which the packet was received.

   * - rx_state
     - The status of the packet.

   * - timestamp
     - Timestamp.

   * - sig_len
     - The length of the message excluding FCS.


