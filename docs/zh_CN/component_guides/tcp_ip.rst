
.. _tcp_ip:

TCP/IP
==============

.. WM IoT SDK 使用以下几种开源协议栈
.. ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

..  -  :ref:`LWIP 协议栈 <LWIP>`
..  -  :ref:`FreeRTos TCP 协议栈 <freertos_tcp>`
..  -  :ref:`BSD TCP/IP 协议栈 <bsd_tcpip>`

WM IoT SDK 默认使用 LWIP 协议栈。

TCP/IP 模型
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 的 TCP/IP 模型如下图所示：


.. mermaid::
  :align: center

  graph TB
  
  A(APP)<-->|"DATA"|B[TCP/IP]
  B-->|"Event"|C[EV]
  C-->|"Event"|A
  E-->|"CMD"|B
  D[WiFi API]-->|"CMD"|E[WM_NETIF]


其中有如下几种事件：

- WM_EVENT_WIFI_STA_GOT_IP ：        
  
  Station 获取到IPv4地址。

- WM_EVENT_WIFI_STA_GOT_IP6 ：        
  
  Station 获取到IPv6地址。

- WM_EVENT_WIFI_STA_LOST_IP ：        
  
  Station 丢失IPv4地址。

- WM_EVENT_WIFI_STA_LOST_IP6 ：        
  
  Station 丢失IPv6地址。

- WM_EVENT_WIFI_AP_STAIPASSIGNED ：    
  
  SoftAP 为连接上的 Station  成功分配 IP 地址。


.. _LWIP:

:ref:`LWIP 协议栈 <LWIP>`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

WM IoT SDK 的 LWIP TCP/IP 协议栈具体 API 如下：
"""""""""""""""""""""""""""""""""""""""""""""""""""

**BSD Socket API：**

BSD Socket API 是一种常见的跨平台 TCP/IP 套接字 API，最初源于 UNIX 操作系统的伯克利标准发行版，现已标准化
为 POSIX 规范的一部分。

在 WM IoT SDK 中，LWIP 支持 BSD 套接字 API 的所有常见用法。

**Netconn API：**

在 WM IoT SDK 中，LWIP 支持两种较低级别的 API，即 Netconn API 和 Raw API：

- LWIP Raw API 适用于单线程设备。

- Netconn API 用于在 LWIP 内部使用 BSD 套接字 API。相较于 BSD Socket API，该 API 占用资源更少。

.. attention:: 

    一般情况下推荐使用 BSD Socket 。

有关 Netconn API 的更多信息，请参阅  `lwIP 非官方应用程序开发手册 <https://lwip.fandom.com/wiki/Netconn_API>`_。

**IPv6：**

WM IoT SDK 支持 IPv6 协议。

WM IoT SDK 提供 menuconfig 配置LWIP
"""""""""""""""""""""""""""""""""""""""

使用编译命令行进入 ``menuconfig``，选择 ``Components configuration`` 进入组件配置 选择 ``LWIP`` 后可以查看
配置关于 LWIP 的相关属性，主要配置参数有：

.. list-table::
   :widths: 45 45 25
   :header-rows: 0
   :align: center

  * - 配置名称
    - 配置描述
    - 默认值

  * - CONFIG_LWIP_TCP_MAXRTX
    - 设置数据段的最大重传次数
    - 3

  * - CONFIG_LWIP_TCP_SYNMAXRTX
    - 设置 SYN 报文段的最大重传次数
    - 4
  
  * - CONFIG_LWIP_TCP_MSS
    - 设置 TCP 传输的最大段大小
    - 1460
  
  * - CONFIG_LWIP_TCP_WND_DEFAULT 
    - 设置默认的TCP接收窗口大小
    - 8

  * - CONFIG_LWIP_TCP_SND_BUF_DEFAULT 
    - 设置默认的发送缓冲区大小。
    - 6

  * - CONFIG_LWIP_IP_FORWARD 
    - 是否允许数据包跨多个转发
    - N
  
  * - CONFIG_LWIP_ENABLE_NAPT
    - 是否启用 napt
    - N

  * - CONFIG_LWIP_ENABLE_IPV4 
    - 是否启用 IPv4。
    - Y

  * - CONFIG_LWIP_ENABLE_IPV6  
    - 是否启用 IPv6
    - N

  * - CONFIG_LWIP_IPV6_AUTOCONFIG
    - 是否允许设备 IPV6 无状态地址参见 RFC 4862
    - Y

  * - CONFIG_LWIP_IPV6_DHCP6 
    - 是否开启 DHCPv6 的 IPv6 无状态地址自动配置功能
    - Y

.. _freertos_tcp:

.. :ref:`FreeRTos TCP 协议栈 <freertos_tcp>`
.. ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. 暂不支持


.. _bsd_tcpip:

.. :ref:`BSD TCP/IP 协议栈 <bsd_tcpip>`
.. ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. 暂不支持

