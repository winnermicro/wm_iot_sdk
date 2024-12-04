
WiFi
===========

WiFi 编程模型
----------------

.. only:: w800

    W800 的 WiFi 编程模型如下图所示：
    
.. mermaid::
    :align: center

    graph LR

    A[TCP stack]-->|"event"|B[Event task]
    B-->|"event"|C[Application task]
    D[WPA]-->|"event"|B
    D-->|"callback"|E[WiFi]
    E-->|"callback"|D
    C-->|"API"|E

WiFi 程序可以看作是一个无法感知上层代码（如 TCP/IP 协议栈、应用程序任务、事件任务等）的黑匣子。通常应用程序任务（代码）负责调用 WiFi 程序 API 来初始化 WiFi，WiFi 触发 WPA 的回调，WPA 会触发 WiFi 事件。然后 WiFi 程序接收并处理 WPA 数据，之后应用程序会得到响应。


WiFi 连接配置
---------------------

WiFi 的连接配置主要由以下几部分构成：

1. WiFi tx buf number

这是 WiFi MAC 层的发送缓冲区的个数，主要是用来存放 ``MAC`` 还没发送出去的数据，用户可以根据实际需要进行修改。

2. WiFi rx buf number

这是 WiFi MAC 层的接收缓冲区的个数，主要是用来存放 ``MAC`` 层接收到的还没发往应用层的数据，用户可以根据实际需要进行修改。

WiFi 初始化
-------------------

在使用 WiFi 之前必须要进行初始化，具体方法是调用 ``wm_wifi_init()`` 函数完成初始化操作。

WiFi 扫描模式
------------------

仅 station 或 station/AP 共存模式支持 ``wm_wifi_scan_start()`` API。

扫描类型
~~~~~~~~~~~~~~~

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 模式
     - 描述
     
   * - 主动扫描
     - 通过发送 probe request 进行扫描。该模式为默认的扫描模式。
     
   * - 被动扫描
     - 不发送 probe request。跳至某一特定信道并等待 beacon。应用程序可通过 ``wm_wifi_scan_config_t`` 中的 ``wm_wifi_scan_type_t`` 字段使能被动扫描。

   * - 全信道扫描
     - ``wm_wifi_scan_config_t`` 中的 ``channel`` 字段为 0 时，当前模式为全信道扫描。
     
   * - 特定信道扫描
     - ``wm_wifi_scan_config_t`` 中的 ``channel``  字段为 1-14 时，当前模式为快速扫描。

   

扫描配置
~~~~~~~~~~~~~~~

扫描类型与其他扫描属性通过函数 ``wm_wifi_scan_start()`` 进行配置。下表详细描述了函数 ``wm_wifi_scan_config_t`` 各字段信息。

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 字段
     - 描述

   * - ssid
     - 如果该字段的值不为 NULL，则仅可扫描到具有相同 SSID 值的 AP。

   * - bssid
     - 如果该字段的值不为 NULL，则仅可扫描到具有相同 BSSID 值的 AP。

   * - channel
     - 如果该字段值为 0，将进行全信道扫描；反之，将针对特定信道进行扫描。
     
   * - show_hidden
     - 如果该字段值为 0，本次扫描将忽略具有隐藏 SSID 的 AP；反之，这些 AP 也会在扫描时被视为正常 AP。

   * - scan_type
     - 如果该字段值为 ``WM_WIFI_SCAN_TYPE_ACTIVE`` 为主动扫描， ``WM_WIFI_SCAN_TYPE_PASSIVE`` 将被视为被动扫描。

   * - scan_time
     -  该字段用于控制每个信道的扫描时间。

        被动扫描时，scan_time.passive 字段负责为每个信道指定扫描时间。

        主动扫描时，每个信道的扫描时间如下列表所示。其中，min 代表 scan_time_active_min，max 代表 scan_time_active_max。

        min=0, max=0：每个信道的扫描时间为 120 ms。

        min>0, max=0：每个信道的扫描时间为 120 ms。

        min=0, max>0：每个信道的扫描时间为 max ms。

        min>0, max>0：每个信道扫描的最短时间为 min ms。 如果在这段时间内未找到 AP，将跳转至下一个信道。如这段时间内找到 AP，则该信道的扫描时间为 max ms。

        如希望提升 Wi-Fi 扫描性能，则可修改上述两个参数。

   * - home_chan_dwell_time
     - 在home频道扫描连续频道之间花费的时间

扫描完成后事件处理阶段
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

当所有信道扫描全部完成后，将产生 ``WM_EVENT_WIFI_SCAN_DONE`` 事件。

应用程序的事件回调函数告知应用程序任务已接收到 ``WIFI_EVENT_SCAN_DONE`` 事件。调用函数 ``wm_wifi_scan_get_ap_num()`` 获取在本次扫描中找到的 AP 数量。然后，分配出足够的事物槽，并调用函数 ``wm_wifi_scan_get_ap_records()`` 获取 AP 记录。请注意，在扫描完成后，请尽快调用函数 ``wm_wifi_scan_get_ap_records()`` 获取 AP 记录，记录会留存一段时间后自动释放，在此之前可重复获取记录。

WiFi STA 模式
------------------

STA 连接配置
~~~~~~~~~~~~~~~

调用 ``wm_wifi_set_config()`` 函数进行 STA 的配置。STA 模式可以配置的内容如下：

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 字段
     - 描述

   * - ssid
     - 目标 AP 的 SSID。

   * - password
     - 目标 AP 的密码。

   * - scan_method
     - 执行所有通道扫描或快速扫描。

   * - bssid_set
     - 是否设置目标 AP 的 MAC 地址。一般来说，station_config 的 bssid_set 需要为 0，只有
       
       当用户需要查看 AP 的 MAC 地址时才需要设置为 1。

   * - bssid
     - 目标 AP 的 MAC 地址。

   * - channel 
     - 目标 AP 的信道。设置为 1 ~ 13，从指定的信道开始扫描，然后再接入 AP。如果 AP 的

       信道未知，设置为 0。

   * - listen_interval
     - 设置 WM_WIFI_PS_MAX_MODEM 时接收信标的监听间隔，单位: AP 信标间隔。
       如果设置为 0，默认为 10。

   * - sort_method
     - 按 rssi 或安全模式对列表中的连接 AP 进行排序。

   * - threshold
     - 设置 sort_method 时，只有认证模式比所选认证模式更安全且信号比最小 RSSI 强的 AP

       才会被使用。

   * - pmf_cfg
     - 配置保护管理框架。将在 RSN IE 中的 RSN 功能中发布。

通常情况下在建立 WiFi 连接之前都要进行配置。但如果已经在: ref:`NVS <nvs>` 中保存过配置，或者想要保存配置，在下一次连接的时候不用进行配置，这也是可以的。

STA 连接
~~~~~~~~~~~~~

调用 ``wm_wifi_connect()`` 函数连接 AP 。如果连接成功将会返回 ``WM_EVENT_WIFI_STA_CONNECTED`` 事件，然后，事件任务将启动 DHCP 客户端服务，最终触发 DHCP
程序。如果因为某些原因连接失败，例如：密码错误、信道错误、未找到 AP 等，就会返回 ``WM_EVENT_WIFI_STA_DISCONNECTED`` 事件并提示相应的失败原因。

STA 获取IP
~~~~~~~~~~~~~~~

一旦 DHCP 客户端初始化完成，WiFi 将进入获取 IP 的阶段。如果 WiFi 成功从 DHCP 服务器接收到 IP 地址，将会触发 ``WM_EVENT_WIFI_STA_GOT_IP`` 或 ``WM_EVENT_WIFI_STA_GOT_IP6`` 事件，这样才是真正的连接上了 AP。

STA 断开连接
~~~~~~~~~~~~~~~~~

可以调用 ``wm_wifi_disconnect()`` 主动断开 WiFi 的连接，WiFi 因为某些原因（例如：AP 掉电、RSSI 较弱等）也会使得 WiFi 断开连接，将产生 ``WM_EVENT_WIFI_STA_DISCONNECTED`` 事件。

WiFi SoftAP 模式
-----------------

SoftAP 配置
~~~~~~~~~~~~~

调用 ``wm_wifi_set_config()`` 函数进行 WiFi 的配置。AP 模式可以配置的内容如下：

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 字段
     - 描述

   * - ssid
     - SoftAP 的 SSID。如果 ssid_len 字段为 0，则必须是一个以 Null 结尾的字符串。否则，
       
       根据 ssid_len 设置长度。

   * - password
     - SoftAP 的密码。

   * - ssid_len
     - 可选的 SSID 字段长度。

   * - channel
     - SoftAP 的信道。

   * - authmode
     - SoftAP 的认证模式。

   * - ssid_hidden 
     - 是否广播 SSID，默认为 0，广播 SSID。

   * - max_connection
     - 允许连接 STA 的最大个数，默认 4 个，最大 8 个。

   * - beacon_interval
     - 信标间隔，应该是 100 的倍数。单位: TU (时间单位，1 tu = 1024 us)。范围:
       
       100 ~ 60000。默认值: 100。

   * - pairwise_cipher
     - SoftAP 的成对密码，组密码将由此导出。

通常情况下在创建 SoftAP 之前都要进行配置。但如果已经在: ref:`NVS <nvs>` 中保存过配置，或者想要保存配置，在下一次连接的时候不用进行配置，这也是可以的。

SoftAP 开启
~~~~~~~~~~~~~

调用 ``wm_wifi_ap_start()`` 函数开启 SoftAP 。如果成功开启将会返回 ``WM_EVENT_WIFI_AP_START`` 事件。如果因为某些原因开启失败，例如：配置错误、硬件损坏
等，就会返回 ``WM_EVENT_WIFI_AP_STOP`` 事件并提示相应的失败原因。

SoftAP 连接
~~~~~~~~~~~~~~~~~

一旦有 STA 连接到 SoftAP，就会返回 ``WM_EVENT_WIFI_AP_STACONNECTED`` 事件，之后成功为 STA 分配 IP 后，会触发 ``WM_EVENT_WIFI_AP_STAIPASSIGNED`` 事件。如果因为某些原因 STA 断开了连接，则会返回 ``WM_EVENT_WIFI_AP_STADISCONNECTED`` 事件。

SoftAP 关闭
~~~~~~~~~~~~~~~~~

可以调用 ``wm_wifi_ap_stop()`` 主动关闭 SoftAP。或者出现异常情况如：断电、重启等，使得 SoftAP 被关闭，都会触发 ``WM_EVENT_WIFI_AP_STOP`` 事件。

WiFi 事件描述
-----------------

以下是 WiFi 中的事件：

.. list-table:: 
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 事件
     - 描述

   * - WM_EVENT_WIFI_SCAN_DONE
     - WiFi 完成对 AP 的扫描。

   * - WM_EVENT_WIFI_STA_CONNECTED
     - STA 连接 AP。

   * - WM_EVENT_WIFI_STA_DISCONNECTED
     - STA 断开 AP。

   * - WM_EVENT_WIFI_STA_BSS_RSSI_LOW
     - STA 连接 AP 的信号强度过低。

   * - WM_EVENT_WIFI_STA_BEACON_TIMEOUT
     - STA 获取 AP 的 beacon 帧超时。

   * - WM_EVENT_WIFI_AP_START
     - 开启 SoftAP。

   * - WM_EVENT_WIFI_AP_STOP
     - 关闭 SoftAP。

   * - WM_EVENT_WIFI_AP_STACONNECTED
     - SOFTAP 模式下有 STA 成功连接到 SOFTAP。

   * - WM_EVENT_WIFI_AP_STADISCONNECTED
     - SOFTAP 模式下有 STA 与 SOFTAP 断开连线。

   * - WM_EVENT_WIFI_STA_GOT_IP
     - STA 获取到 AP 的 ipv4 IP。

   * - WM_EVENT_WIFI_STA_GOT_IP6
     - STA 获取到 AP 的 ipv6 IP。

   * - WM_EVENT_WIFI_STA_LOST_IP
     - STA 失去 AP 的 ipv4 IP。

   * - WM_EVENT_WIFI_STA_LOST_IP6
     - STA 失去 AP 的 ipv6 IP。

   * - WM_EVENT_WIFI_AP_STAIPASSIGNED
     - SoftAP 为连接的 STA 分配 IP。

   * - WM_EVENT_WIFI_ONESHOT_SCAN_DONE
     - OneShot 模式下扫描完成。

   * - WM_EVENT_WIFI_ONESHOT_FOUND_CHANNEL
     - OneShot 模式下获取到信道。

   * - WM_EVENT_WIFI_ONESHOT_GOT_SSID_PSWD
     - OneShot 模式下获取到 SSID 和 Password。

   * - WM_EVENT_WIFI_ONESHOT_GOT_CUSTOM_DATA
     - OneShot 模式下获取到自定义数据。

   * - WM_EVENT_WIFI_ONESHOT_SEND_REPLY_DONE
     - OneShot 模式下发送 ACK 完成。

WiFi 错误码
---------------

.. only:: w800

    W800 的 WiFi 错误码有很多，这里进行说明：

    .. list-table::
        :header-rows: 1
        :widths: 25 5 55
        :align: center

        * - 错误码
          - 码值
          - 描述

        * - **WM_WIFI_REASON_UNSPECIFIED**
          - 1
          - 出现内部错误，例如：内存已满，内部发送失败，或该原因已被远端接收等。

        * - **WM_WIFI_REASON_AUTH_EXPIRE**
          - 2
          - 先前的 authentication 已失效。

        * - **WM_WIFI_REASON_AUTH_LEAVE**
          - 3
          - authentication 取消，因为发送 station 正在离开（或已经离开）。

        * - **WM_WIFI_REASON_ASSOC_EXPIRE**
          - 4
          - 因为 AP 不活跃，association 取消。

        * - **WM_WIFI_REASON_ASSOC_TOOMANY**
          - 5
          - association 取消，因为 AP 无法同时处理所有当前已关联的 STA。

        * - **WM_WIFI_REASON_NOT_AUTHED**
          - 6
          - 因为 AP 未认证，association 取消。

        * - **WM_WIFI_REASON_NOT_ASSOCED**
          - 7
          - 因为 AP 未关联，association 取消。

        * - **WM_WIFI_REASON_ASSOC_LEAVE**
          - 8
          - 因为 AP 关联离开，association 取消。

        * - **WM_WIFI_REASON_ASSOC_NOT_AUTHED**
          - 9
          - 因为 AP 关联未认证，association 取消。

        * - **WM_WIFI_REASON_DISASSOC_PWRCAP_BAD**
          - 10
          - 因为 AP 功率不足，association 取消。

        * - **WM_WIFI_REASON_DISASSOC_SUPCHAN_BAD**
          - 11
          - 因为 AP 支持的信道不足，association 取消。

        * - **WM_WIFI_REASON_BSS_TRANSITION_DISASSOC**
          - 12
          -  STA 断开关联，原因是 BSS 过渡。

        * - **WM_WIFI_REASON_IE_INVALID**
          - 13
          - 因为 AP 发送的 IE 无效，association 取消。

        * - **WM_WIFI_REASON_MIC_FAILURE**
          - 14
          - 因为 AP 发送的 MIC 无效，association 取消。

        * - **WM_WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT**
          - 15
          - 因为 AP 握手超时，association 取消。

        * - **WM_WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT**
          - 16
          - 组密钥 (Group-Key) 握手超时。

        * - **WM_WIFI_REASON_IE_IN_4WAY_DIFFERS**
          - 17
          - 四次握手中产生的元素与 (re-)association 后的 request/probe 以及 response/beacon frame 中的信息不同。

        * - **WM_WIFI_REASON_GROUP_CIPHER_INVALID**
          - 18
          - STA 断开关联，原因是 Group Cipher 无效。

        * - **WM_WIFI_REASON_PAIRWISE_CIPHER_INVALID**
          - 19
          - STA 断开关联，原因是 Pairwise Cipher 无效。

        * - **WM_WIFI_REASON_AKMP_INVALID**
          - 20
          - STA 断开关联，原因是 Pairwise Cipher 无效。

        * - **WM_WIFI_REASON_UNSUPP_RSN_IE_VERSION**
          - 21
          - STA 断开关联，原因是 RSN IE 版本无效。

        * - **WM_WIFI_REASON_INVALID_RSN_IE_CAP**
          - 22
          - STA 断开关联，原因是 RSN IE 能力无效。

        * - **WM_WIFI_REASON_CIPHER_SUITE_REJECTED**
          - 24
          - STA 断开关联，原因是 Cipher Suite 被拒绝。

        * - **WM_WIFI_REASON_UNSPECIFIED_QOS_REASON**
          - 32
          - STA 断开关联，由于不明确的 QoS 相关原因。

        * - **WM_WIFI_REASON_NOT_ENOUGH_BANDWIDTH**
          - 33
          - STA 断开关联，由于QoS AP 缺少足够的带宽给该 QoS STA 使用

        * - **WM_WIFI_REASON_DISASSOC_LOW_ACK**
          - 34
          - STA 断开关联，原因是低 ACK。

        * - **WM_WIFI_REASON_EXCEEDED_TXOP**
          - 35
          - STA 断开关联，由于 STA 的传输超过了 TXOPs 的限制。

        * - **WM_WIFI_REASON_STA_LEAVING**
          - 36
          - 请求 STA 离开了 BSS 或者重置了

        * - **WM_WIFI_REASON_TIMEOUT**
          - 39
          - 对端 STA 的请求超时。

        * - **WM_WIFI_REASON_INVALID_PMKID**
          - 49
          - 无效的成对主密钥标识符（PMKID）。

        * - **WM_WIFI_REASON_NO_AP_FOUND**
          - 200
          - 该错误码的含义是没有发现 AP，产生这种错误的主要原因是 AP 不存在，或对 AP 的安全配置错误。

        * - **WM_WIFI_REASON_AUTH_FAIL**
          - 201
          - 该错误码的含义是认证失败。产生这种错误的原因可能是密码错误，或 AP 的安全配置错误。

        * - **WM_WIFI_REASON_ASSOC_FAIL**
          - 202
          - association 失败，但并非由 ASSOC_EXPIRE 或 ASSOC_TOOMANY 引发。

        * - **WM_WIFI_REASON_BEACON_TIMEOUT**
          - 203
          - 该错误码的含义是无法接收到来自 AP 的 Beacon 帧，产生这种错误的主要原因是 AP 掉电。
            
          

WiFi APSTA模式
-----------------

.. only:: w800

    要想开启 APSTA 模式，推荐的操作步骤是在 STA 模式下先连接 AP，在获取到 AP 的信道信息之后，根据获取到的信道信息配置 SoftAP。由于目前 W800 只有一个 RF，不支持 STA 和 SoftAP 工作在不同的信道上，所以当 STA 和 SoftAP 被设置在两个不同的信道上时，SoftAP 可能无法正常工作。


其它操作跟 STA 和 SoftAP 模式一样，这里就不再说明。

WiFi Sniffer 模式
--------------------

WiFi Sniffer 模式可以通过 ``wm_wifi_set_promiscuous()`` 使能。

.. only:: w800

    W800 可以接收管理帧和数据帧，无法接收控制帧。

接收到的帧由帧信息和帧数据两部分组成，帧信息包含的内容如下：

.. list-table::
   :widths: 25 25 
   :header-rows: 0
   :align: center

   * - 字段
     - 描述

   * - rssi
     - 报文的 RSSI。单位: dBm。

   * - rate
     - 数据包的 PHY 速率编码。

   * - sig_mode
     - 0 非 HT (11bg) 包；1 HT (11n) 包；

   * - mcs
     - 调制编码方案。

   * - cwb 
     - 报文的信道带宽。

   * - smoothing
     - ----------。

   * - not_sounding
     - ----------。

   * - aggregation
     - 0 MPDU 报文；1 AMPDU 报文。

   * - stbc
     - 0 非 STBC 报文；1 STBC 报文。

   * - fec_coding
     - 标识为 LDPC 的 11n 个报文。

   * - sgi
     - 短导程。

   * - noise_floor
     - 射频模块 (RF) 的本底噪声。单位: 0.25 dbm。

   * - ampdu_cnt
     - 聚合帧的个数。

   * - channel 
     - 接收数据包的主信道。

   * - secondary_channel
     - 接收此数据包的辅助信道。

   * - rx_state
     - 数据包的状态。

   * - timestamp
     - 时间戳。

   * - sig_len
     - 不包括 FCS 的报文长度。
