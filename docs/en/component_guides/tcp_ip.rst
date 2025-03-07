
.. _tcp_ip:

TCP/IP
==============

The WM IoT SDK defaults to using the LWIP protocol stack.

TCP/IP Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The TCP/IP model of the WM IoT SDK is  illustrated in the following diagram:



.. mermaid::
  :align: center

  graph TB
  
  A(APP)<-->|"DATA"|B[TCP/IP]
  B-->|"Event"|C[EV]
  C-->|"Event"|A
  E-->|"CMD"|B
  D[WiFi API]-->|"CMD"|E[WM_NETIF]


The following events are included:

- WM_EVENT_WIFI_STA_GOT_IP ：        
  
  The station has obtained an IPv4 address.

- WM_EVENT_WIFI_STA_GOT_IP6 ：        
  
  The station has obtained an IPv6 address.

- WM_EVENT_WIFI_STA_LOST_IP ：        
  
  The station has lost its IPv4 address.

- WM_EVENT_WIFI_STA_LOST_IP6 ：        
  
  The station has lost its IPv6 address.

- WM_EVENT_WIFI_AP_STAIPASSIGNED ：    
  
  The SoftAP has successfully assigned an IP address to a connected station.


.. _LWIP:

:ref:`LWIP Protocol Stack <LWIP>`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The specific APIs of the LWIP TCP/IP protocol stack in the WM IoT SDK are as follows:
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

**BSD Socket API：**

The BSD Socket API is a common cross-platform TCP/IP socket API that originated from the Berkeley Standard Distribution of the UNIX operating system and has now been standardized as part of the POSIX specification.

In the WM IoT SDK, LWIP supports all common usages of the BSD Socket API.

**Netconn API：**

In the WM IoT SDK, LWIP supports two lower-level APIs, namely the Netconn API and the Raw API:

- The LWIP Raw API is suitable for single-threaded devices.

- The Netconn API is used internally by LWIP to implement the BSD Socket API. Compared to the BSD Socket API, this API consumes fewer resources.


.. attention:: 

    Generally, it is recommended to use the BSD Socket API.

For more information on the Netconn API, please refer to the  `Unofficial lwIP Application Developer's Manual <https://lwip.fandom.com/wiki/Netconn_API>`_ .

**IPv6：**

The WM IoT SDK supports the IPv6 protocol.

Configuring LWIP with WM IoT SDK's menuconfig 
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""

Enter ``menuconfig`` using the compile command line, 
select  ``Components configuration``  to enter the component configuration and then select ``LWIP`` to view the relevant properties related to LWIP, 
Main configuration parameters:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center


  * - Configuration name
    - Configuration description
    - Default values

  * - CONFIG_LWIP_TCP_MAXRTX
    - Set the maximum number of retransmissions of a data segment
    - 3

  * - CONFIG_LWIP_TCP_SYNMAXRTX
    - Set the maximum number of retransmissions of SYN packet segments
    - 4

  * - CONFIG_LWIP_TCP_MSS
    - Set the maximum segment size for TCP transfers
    - 1460

  * - CONFIG_LWIP_TCP_WND_DEFAULT
    - Set the default TCP receive window size
    - 8

  * - CONFIG_LWIP_TCP_SND_BUF_DEFAULT
    - Set the default send buffer size.
    - 6

  * - CONFIG_LWIP_IP_FORWARD
    - Whether packets are allowed to be forwarded across multiple
    - N

  * - CONFIG_LWIP_ENABLE_NAPT
    - Whether napt is enabled or not
    - N

  * - CONFIG_LWIP_ENABLE_IPV4
    - Whether IPv4 is enabled or not.
    - Y

  * - CONFIG_LWIP_ENABLE_IPV6
    - Whether IPv6 is enabled
    - N

  * - CONFIG_LWIP_IPV6_AUTOCONFIG
    - See RFC 4862 for details on whether devices are allowed to have IPV6 stateless addresses
    - Y

  * - CONFIG_LWIP_IPV6_DHCP6
    - Whether to enable IPv6 stateless address auto-configuration of DHCPv6
    - Y
