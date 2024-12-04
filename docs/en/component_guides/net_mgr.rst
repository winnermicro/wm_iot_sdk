
.. _net_mgr:

Net Manager
=============

Introduction
-------------

Net Manager  provides users with a unified network management API. By shielding the differences in the underlying protocol stack, it offers simple and rapid methods for network connection and configuration.


Function List
-----------------
1. :ref:`Rapid Network Connection <nm_wifi_station>`
2. :ref:`Quick SoftAP Startup <nm_wifi_softap>`
3. :ref:`General Network Management <nm_net_config>`

Feature Overview
-----------------

**Quick Network Connection**: Provides an easy-to-use Wi-Fi connection feature. Users only need to provide the SSID and Password, and Net Manager takes care of the initialization of Wi-Fi and network protocol stacks, Wi-Fi connection, DHCP requests, reconnection upon disconnection, etc. Users can start sending and receiving network data once the device obtains an IP address.

**Quick SoftAP:** Provides a quick SoftAP startup feature. Users only need to specify SSID and Password to quickly start SoftAP, DHCP Server,and other related functions.

**General Network Management:** Provides basic network configuration and query functions for basic network features such as IP addresses, MAC addresses, DHCP Servers, DHCP, and DNS.

Framework Model
-----------------

.. figure:: ../../_static/component-guides/network/net_manger/net_manager_block_diagram_en.svg
    :align: center
    :alt: Net manager block diagram

..

Net Manager mainly primarily offers two functionalities:

    1. Encapsulates WiFi and network protocol stack capabilities, providing simple and convenient interfaces, allowing users to quickly connect to the network and start sending and receiving network data.

    2. Encapsulates unified network management interfaces, where users do not need to concern themselves with the network protocol stack. By calling Net Manager interfaces, users can inquire about and configure basic network settings.

**Suggestion**: Net Manager provides comprehensive encapsulation of Wi-Fi and network protocol stacks. If users only need simple Wi-Fi connection functionalities, it is recommended that they use Net manager to complete network configuration. However, if users require the full range of Wi-Fi capabilities, it is recommended that they disable Net manager and use the relevant APIs provided by WM_WiFi to achieve network connectivity.


Net manager Function Configuration
-----------------------------------------

Wi-Fi Station Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - Configuration Item
     - Description

   * - COMPONENT_NM_WIFI_STA_ENABLED
     - Net Manager Wi-Fi Station function enable switch.  When enabled, users can call the ``wm_nm_start_wifi_station`` function to quickly connect to the network.

   * - COMPONENT_NM_WIFI_STA_ENABLED
     - Net Manager Wi-Fi Station automatic reconnection function. When enabled, Net Manager is responsible for handling Wi-Fi disconnection and reconnection.

   * - NM_STA_RECONNECT_INTERVAL
     - Wi-Fi Station reconnection interval after disconnection, defaulting to 1000 milliseconds.

Wi-Fi SoftAP Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - Configuration Item   
     - Description

   * - COMPONENT_NM_WIFI_SOFTAP_ENABLED
     - Net Manager Wi-Fi SoftAP enable switch.  After enabling, users can call the ``wm_nm_start_wifi_softap`` function to quickly start the SoftAP feature.
    
   * - NM_WIFI_SOFTAP_AUTO_CHANNEL
     - Net Manager Wi-Fi SoftAP automatic channel selection. When enabled, the SoftAP channel follows the channel of the connected station. 
	   It is recommended to enable this function in scenarios where the station and SoftAP coexist, as inconsistency in their channel configurations may cause some functions to fail. 
	   If the station is not connected, the SoftAP will use channel 1 as the default.
	   
   * - NM_WIFI_SOFTAP_CHANNEL
     - After disabling automatic channel selection, users need to specify the SoftAP channel, supporting channels 1 to 13.
	 
   * - NM_WIFI_SOFTAP_MAX_STA_CONN
     - Specifies the maximum number of connections supported by SoftAP. By default, it supports up to 8 station connections.

DHCP Server Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - Configuration Item   
     - Description

   * - NM_DHCPS_IP
     - DHCP Server IP address.

   * - NM_DHCPS_NETMASK
     - DHCP Server IP address mask.

   * - NM_DHCPS_GATEWAY
     - DHCP Server gateway address.

   * - NM_DHCPS_START_IP
     - The start IP address assigned by  the DHCP Server, which should be in the same subnet as NM_DHCPS_IP.

   * - NM_DHCPS_LEASE_TIME
     - The lease time of the IP address assigned by the DHCP Server, in seconds.

Main Features
------------------

.. _nm_wifi_station:

Quick Network Connection
^^^^^^^^^^^^^^^^^^^^^^^^^^

**Wi-Fi Station State Machine**

.. figure:: ../../_static/component-guides/network/net_manger/net_manager_state_machine_en.svg
    :align: center
    :alt: Net manager WiFi Station state machine

..

When users call ``wm_nm_start_wifi_station`` , Net Manager will start the WiFi connection and network protocol stack initialization, and request an IP address from the router.

After calling ``wm_nm_start_wifi_station`` , you can synchronously wait for the IP address allocation flag in the program. Once the device obtains the IP address, users can start sending and receiving network data. Example:

    .. code:: c

        wm_nm_wifi_station_cfg_t cfg = { "ssid", "password" };
        wm_nm_start_wifi_station(&cfg);
        while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
            vTaskDelay(10);;
        }
        wm_log_info("wifi station start success, network is ready!");


Additionally, users need to pay attention to the WM_NM_EV event WM_NM_WIFI_STA_LOST_IP. If a WiFi disconnection or other network anomalies occur during communication, Net Manager will push the WM_NM_WIFI_STA_LOST_IP event. 
Users need to wait for the network to reconnect before resuming data transmission.

    .. code:: c

        /** WM_NM_EV event response function */
        static void wm_nm_event_handle(wm_event_group_t group, int event, void *data, void *priv)
        {
            switch (event) {
                case WM_NM_WIFI_STA_GOT_IP:
                {
                    wm_log_info("net manager got IP");
                    break;
                }
                case WM_NM_WIFI_STA_LOST_IP:
                {
                    wm_log_error("net manager lost IP, waiting for WiFi reconnection");
                    break;
                }
                default:
                    break;
            }
        }

        /** Register WM_NM_EV event response function */
        wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

If users need to disconnect the WiFi connection, they can call ``wm_nm_stop_wifi_station`` to disconnect and release resources.

    .. code:: c

        wm_nm_stop_wifi_station();

.. _nm_wifi_softap:

Quick Start for SoftAP 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When users need to enable SoftAP, they can call ``wm_nm_start_wifi_softap``, which will automatically initialize Wi-Fi and the network protocol stack, and start the DHCP Server. 
After calling ``wm_nm_start_wifi_softap`` , you can synchronously wait for the ``WM_NM_WIFI_AP_READY`` flag.

    .. code:: c

        wm_nm_wifi_softap_cfg_t cfg = { "wm_ssid", "password" };
        wm_nm_start_wifi_softap(&cfg);
        while (wm_nm_get_wifi_softap_state() != WM_NM_WIFI_AP_READY) {
            vTaskDelay(10);;
        }

Similarly, users can listen for the  WM_NM_WIFI_AP_ASSIGNED_IP_ID event of WM_NM_EV to be notified when a station joins the network..

    .. code:: c

        /** WM_NM_EV event response function */
        static void wm_nm_event_handle(wm_event_group_t group, int event, void *data, void *priv)
        {
            static wm_app_msg_t app_msg;
            switch (event) {
                case WM_NM_WIFI_AP_READY:
                {
                    wm_log_info("softAP is ready");
                    break;
                }
                case WM_NM_WIFI_AP_ASSIGNED_IP_ID:
                {
                    wm_log_info("assigned IP to staion");
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        /** Register WM_NM_EV event response function */
        wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

If users need to stop the SoftAP, they can call ``wm_nm_stop_wifi_softap`` to stop the softAP and release resources.

    .. code:: c

        wm_nm_stop_wifi_station();

.. _nm_net_config:

General Network Management
^^^^^^^^^^^^^^^^^^^^^^^^^^^
**Finding Net Manager Handles**

Net Manager provides basic network configuration interfaces. Users can query or set related parameters through the corresponding APIs. 
Net Manager APIs depend on wm_nm_netif_t for operation, which users can obtain through the following example:

    .. code:: c

        /** Iterate through all netif */
        for (wm_nm_netif_t netif = wm_nm_get_netif(); netif != WM_NM_INVALID_NETIF; netif = wm_nm_get_next_netif(netif)) {
        }

        /** Query WiFi station netif, note that a netif can only be queried after WiFi connection */
        wm_nm_netif_t netif = wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME);

        /** Query WiFi softAP netif, note that a netif can only be queried after softAP startup */
        wm_nm_netif_t netif = wm_nm_get_netif_by_name(WIFI_SOFTAP_NETIF_NAME);

**Configuring IPv4 Addresses**

Set a static IPv4 address for the Wi-Fi station. Note that setting a static IPv4 address will disable DHCP function:

    .. code:: c

        wm_netif_ip_info_t ip_info      = { 0 };
        ip_info.ip.u_addr.ip4.addr      = inet_addr("192.168.1.10");    /** Set IPv4 address */
        ip_info.netmask.u_addr.ip4.addr = inet_addr("255.255.255.0");   /** Set IPv4 network mask */
        ip_info.gw.u_addr.ip4.addr      = inet_addr("192.168.1.1");     /** Set IPv4 gateway address */
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);


If you need to re-enable the DHCP function for the Wi-Fi station, you must first clear the static IPv4 address and then enable DHCP:

    .. code:: c

        wm_netif_ip_info_t ip_info      = { 0 };
        ip_info.ip.u_addr.ip4.addr      = 0;    /** Clear static IPv4 address */
        ip_info.netmask.u_addr.ip4.addr = 0;
        ip_info.gw.u_addr.ip4.addr      = 0;
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);
        wm_nm_start_netif_dhcpc(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME));

**Configuring DHCP Server Parameters**

The Net Manager supports users to configure DHCP server-related parameters, including server IP address, subnet mask, gateway IP address, starting IP address for allocation, and IP allocation duration. Note that the server IP address, gateway IP address, and starting IP address for allocation must be in the same subnet:

    .. code:: c

        wm_nm_netif_t netif;
        wm_nm_dhcps_option_t dhcps_option = { 0 };

        netif = wm_nm_get_netif_by_name(WIFI_SOFTAP_NETIF_NAME); /** Query softAP handle */

        dhcps_option.lease_time = 7200;                         /** Write the duration of IP allocation */
        dhcps_option.server_ip  = inet_addr("192.168.8.1");     /** Write the server IP address */
        dhcps_option.start_ip   = inet_addr("192.168.8.100");   /** Write network mask */
        dhcps_option.ip_mask    = inet_addr("255.255.255.0");   /** Write gateway IP address */
        dhcps_option.gateway    = inet_addr("192.168.8.1");     /** Write the starting IP address for allocation */
        dhcps_option.dns1       = inet_addr("8.8.8.8");         /** Write DNS primary address */
        dhcps_option.dns2       = inet_addr("114.114.114.114"); /** Write backup DNS address */

        wm_nm_set_netif_dhcps_option(netif, &dhcps_option); /** Set DHCP Server parameters */
        wm_nm_start_netif_dhcps(netif);                     /** Start DHCP Server */


Important Notes
---------------------------------

.. warning:: It is not recommended to mix and match the Net Manager with Wi-Fi APIs. For simple networking needs, it is advisable to use the Net manager. Otherwise, please disable the Net manager component and directly call the Wi-Fi API and network protocol stack API.

Application Examples
---------------------
For basic examples using the Net manager, please refer to :ref:`examples/network <network_example>` . Specifically:

Please refer to the SDK for basic examples of using net manager for Wi-Fi Stations: examples/network/net_manager/wifi_station/ 工程

Please refer to the SDK for basic examples of using net manager's Wi-Fi SoftAP: examples/network/net_manager/wifi_softap/ 工程
