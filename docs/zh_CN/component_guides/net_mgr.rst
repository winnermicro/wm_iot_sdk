
.. _net_mgr:

Net manager
=============

简介
-------------

    Net manager 是为用户提供一套统一的网络管理 API。通过屏蔽底层协议栈差异，封装复杂的 Wi-Fi、网络协议栈等组件接口；提供简易、快速的联网配网方式。


功能列表
-------------

    1. :ref:`快速连网 <nm_wifi_station>`
    2. :ref:`快速启动 softAP <nm_wifi_softap>`
    3. :ref:`通用网络管理 <nm_net_config>`

功能概述
-------------

    **快速连网：** 提供简易连接 Wi-Fi 功能，用户只需要提供 SSID、 Password，由 Net manager 负责 Wi-Fi 和网络协议栈初始化， Wi-Fi 连线， DHCP 请求，断线重连等工作，
    用户只需要等待设备获取到 IP 地址后，即可开始网络数据收发操作。

    **快速 softAP：** 提供快速启动 softAP 功能，用户只需要指定 SSID、 Password，即可快速启动 softAP，DHCP server 等功能。

    **通用网络管理：** 提供 IP 地址、MAC 地址、DHCP server、DHCP、DNS 等基础网络功能配置和查询功能。

框架模型
-------------

.. figure:: ../../_static/component-guides/network/net_manger/net_manager_block_diagram.svg
    :align: center
    :alt: Net manager block diagram

..

Net manager 主要提供两部分功能：

    1. 封装 Wi-Fi 和网络协议栈能力，提供简单便捷的接口，用户可以快速的连接网络，开始网络数据收发

    2. 封装统一的网络管理接口，用户无需关心网络协议栈，调用 Net manager 接口即可查询和配置基础网络配置

    **建议**：Net manager 对 Wi-Fi 和网络协议栈做了比较全面的封装，若用户仅使用简单的 Wi-Fi 连线等功能，建议用户使用 Net manager 完成网络配置。
    若需要使用完整的 Wi-Fi 能力，建议用户关闭 Net manager，使用 WM_WiFi 提供的相关 API，实现连网功能。


Net manager 功能配置
---------------------

Wi-Fi station 配置
^^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - 配置项
     - 描述

   * - COMPONENT_NM_WIFI_STA_ENABLED
     - Net manager Wi-Fi station 功能使能开关，使能后，用户可以调用 wm_nm_start_wifi_station 函数实现快速连网功能。

   * - COMPONENT_NM_WIFI_STA_ENABLED
     - Net manager Wi-Fi station 自动重连功能，使能后，由 Net manager 负责 Wi-Fi 断线重连工作。

   * - NM_STA_RECONNECT_INTERVAL
     - Wi-Fi station 断线重连间隔，默认为 1000 毫秒。

Wi-Fi softAP 配置
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - 配置项
     - 描述

   * - COMPONENT_NM_WIFI_SOFTAP_ENABLED
     - Net manager Wi-Fi softAP 功能使能开关，使能后，用户可以调用 wm_nm_start_wifi_softap 函数实现快速启动 softAP 功能。

   * - NM_WIFI_SOFTAP_AUTO_CHANNEL
     - Net manager Wi-Fi softAP 自动选择信道，使能后，softAP 信道会跟随 station 连线的信道。
       在 station 和 softAP 共存的场景下，建议使能本功能，否则若 station 和 softAP 信道配置不一致的话，会导致部分功能失效。
       使能本功能后，若 station 未连线的情况下，启动 softAP 功能，将使用信道 1 作为默认信道。

   * - NM_WIFI_SOFTAP_CHANNEL
     - 关闭自动选择信道功能后，需要用户指定 softAP 信道，支持 1 ~ 13 信道选择。

   * - NM_WIFI_SOFTAP_MAX_STA_CONN
     - 指定 softAP 支持的最大连接数，默认支持 8 台 station 接入。

DHCP server 配置
^^^^^^^^^^^^^^^^^^^^^^^^

.. list-table::
   :widths: 25 25
   :header-rows: 0
   :align: center

   * - 配置项
     - 描述

   * - NM_DHCPS_IP
     - DHCP 服务器 IP 地址。

   * - NM_DHCPS_NETMASK
     - DHCP 服务器 IP 地址掩码。

   * - NM_DHCPS_GATEWAY
     - DHCP 服务器网关地址。

   * - NM_DHCPS_START_IP
     - DHCP 服务器分配的 IP 起始地址，需要与 NM_DHCPS_IP 保持在同网段。

   * - NM_DHCPS_LEASE_TIME
     - DHCP 服务器分配的 IP 存活时间，单位秒。

主要功能
-------------

.. _nm_wifi_station:

快速连网
^^^^^^^^^^^^

**Wi-Fi station 状态机**

.. figure:: ../../_static/component-guides/network/net_manger/net_manager_state_machine.svg
    :align: center
    :alt: Net manager Wi-Fi station state machine

..

用户调用 wm_nm_start_wifi_station 时，Net manager 将启动WiFi连线和网络协议栈初始化，并向路由器请求分配 IP 地址。

在调用 wm_nm_start_wifi_station 后，可在程序中同步等待分配 IP 地址的标志，当设备获取到 IP 地址后，用户可开始网络数据收发。示例：

    .. code:: c

        wm_nm_wifi_station_cfg_t cfg = { "ssid", "password" };
        wm_nm_start_wifi_station(&cfg);
        while (wm_nm_get_wifi_station_state() != WM_NM_WIFI_STA_GOT_IP) {
            vTaskDelay(10);;
        }
        wm_log_info("Wi-Fi station start success, network is ready!");


另外：用户同样需要关注 WM_NM_EV 的 WM_NM_WIFI_STA_LOST_IP 事件，若在通信过程中，出现 Wi-Fi 断网，或其他网络异常情况，Net manager 会推送 WM_NM_WIFI_STA_LOST_IP 事件，
用户需要等待网络连接后，再进行数据收发。

    .. code:: c

        /** WM_NM_EV 事件响应函数 */
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
                    wm_log_error("net manager lost IP, waiting for Wi-Fi reconnection");
                    break;
                }
                default:
                    break;
            }
        }

        /** 注册 WM_NM_EV 事件响应函数 */
        wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

若用户需要释放 Wi-Fi 连线时，可调用 wm_nm_stop_wifi_station 断开连线，释放资源。

    .. code:: c

        wm_nm_stop_wifi_station();

.. _nm_wifi_softap:

快速启动 softAP
^^^^^^^^^^^^^^^^^^^^

当用户需要启用 softAP 时，可以调用 wm_nm_start_wifi_softap，该函数会自动初始化 Wi-Fi 及网络协议栈，并启动 DHCP server。
调用 wm_nm_start_wifi_softap 后，可同步等待 WM_NM_WIFI_AP_READY 标志位

    .. code:: c

        wm_nm_wifi_softap_cfg_t cfg = { "wm_ssid", "password" };
        wm_nm_start_wifi_softap(&cfg);
        while (wm_nm_get_wifi_softap_state() != WM_NM_WIFI_AP_READY) {
            vTaskDelay(10);;
        }

同样，用户可以监听 WM_NM_EV 的 WM_NM_WIFI_AP_ASSIGNED_IP_ID 事件，用以感知 station 的加入

    .. code:: c

        /** WM_NM_EV 事件响应函数 */
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

        /** 注册 WM_NM_EV 事件响应函数 */
        wm_event_add_callback(WM_NM_EV, WM_EVENT_ANY_TYPE, wm_nm_event_handle, NULL);

若用户需要停止 softAP 时，可调用 wm_nm_stop_wifi_softap 停止 softAP，释放资源。

    .. code:: c

        wm_nm_stop_wifi_station();

.. _nm_net_config:

通用网络管理
^^^^^^^^^^^^^^^^^
**查找 Net manager 句柄**

Net manager 提供基础网络配置接口，用户可以通过对应的 API 查询或设置相关参数。
Net manager API 依赖 wm_nm_netif_t 进行操作，用户可以通过以下示例获取该参数：

    .. code:: c

        /** 遍历所有的 netif */
        for (wm_nm_netif_t netif = wm_nm_get_netif(); netif != WM_NM_INVALID_NETIF; netif = wm_nm_get_next_netif(netif)) {
        }

        /** 查询 Wi-Fi station netif, 注意需要 Wi-Fi 连线后才能查询到可用的 netif */
        wm_nm_netif_t netif = wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME);

        /** 查询 Wi-Fi softAP netif, 注意需要 softAP 启动后才能查询到可用的 netif */
        wm_nm_netif_t netif = wm_nm_get_netif_by_name(WIFI_SOFTAP_NETIF_NAME);

**配置 IPv4 地址**

设置 Wi-Fi station 静态 IPv4 地址，注意设置静态 IPv4 地址后会联动关闭 DHCP 功能：

    .. code:: c

        wm_netif_ip_info_t ip_info      = { 0 };
        ip_info.ip.u_addr.ip4.addr      = inet_addr("192.168.1.10");    /** 设置IPv4地址 */
        ip_info.netmask.u_addr.ip4.addr = inet_addr("255.255.255.0");   /** 设置IPv4网络掩码 */
        ip_info.gw.u_addr.ip4.addr      = inet_addr("192.168.1.1");     /** 设置IPv4网关地址 */
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);

如果需要重新打开 Wi-Fi station DHCP 功能，需要预先清空静态 IPv4 地址后，再使能 DHCP 功能：

    .. code:: c

        wm_netif_ip_info_t ip_info      = { 0 };
        ip_info.ip.u_addr.ip4.addr      = 0;    /** 清空静态 IPv4 地址 */
        ip_info.netmask.u_addr.ip4.addr = 0;
        ip_info.gw.u_addr.ip4.addr      = 0;
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);
        wm_nm_set_netif_ip_info(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME), &ip_info);
        wm_nm_start_netif_dhcpc(wm_nm_get_netif_by_name(WIFI_STATION_NETIF_NAME));

**配置 DHCP server 参数**

Net manager 支持用户配置 DHCP server 相关参数，包括 server IP 地址，网络掩码，网关 IP 地址，分配的起始 IP 地址，分配 IP 的持续时间
注意，server IP 地址，网关 IP 地址，分配的起始 IP 地址需要处于同一子网：

    .. code:: c

        wm_nm_netif_t netif;
        wm_nm_dhcps_option_t dhcps_option = { 0 };

        netif = wm_nm_get_netif_by_name(WIFI_SOFTAP_NETIF_NAME); /** 查询 softAP 句柄 */

        dhcps_option.lease_time = 7200;                         /** 写分配 IP 的持续时间 */
        dhcps_option.server_ip  = inet_addr("192.168.8.1");     /** 写 server IP 地址 */
        dhcps_option.start_ip   = inet_addr("192.168.8.100");   /** 写网络掩码 */
        dhcps_option.ip_mask    = inet_addr("255.255.255.0");   /** 写网关 IP 地址 */
        dhcps_option.gateway    = inet_addr("192.168.8.1");     /** 写分配的起始 IP 地址 */
        dhcps_option.dns1       = inet_addr("8.8.8.8");         /** 写 DNS 主地址 */
        dhcps_option.dns2       = inet_addr("114.114.114.114"); /** 写备份 DNS 地址 */

        wm_nm_set_netif_dhcps_option(netif, &dhcps_option); /** 设置 DHCP server 参数 */
        wm_nm_start_netif_dhcps(netif);                     /** 启动 DHCP server */


注意事项
-------------

.. warning:: 不建议 Net manager 与 Wi-Fi API 混用，如果只需简单连网，建议使用 Net manager；否则，请关闭 Net manager 组件直接调用 Wi-Fi API 和网络协议栈 API

应用实例
-------------

使用 Net manager 基本示例请参照 :ref:`examples/network <network_example>` 其中：
    1. Wi-Fi station 基本示例请参照 sdk 中： examples/network/net_manager/wifi_station/ 工程
    2. Wi-Fi softAP 基本示例请参照 sdk 中： examples/network/net_manager/wifi_softap/ 工程


Net manager 的 meunconfig 配置
----------------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_COMPONENT_NET_MANAGER_ENABLED
     - 是否启用 Net manager 组件
     - N

   * - CONFIG_COMPONENT_NM_WIFI_STA_ENABLED
     - 是否支持 Wi-Fi station
     - N

   * - CONFIG_COMPONENT_NM_WIFI_SOFTAP_ENABLED 
     - 是否支持 Wi-Fi softAP
     - N
