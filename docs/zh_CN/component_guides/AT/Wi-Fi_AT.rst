
.. |Equipment-Name| replace:: w800
.. _WiFi-AT:

****************************
Wi-Fi AT 命令集
****************************


-  :ref:`AT+CWINIT <cmd-INIT>`                  ：初始化/清理 Wi-Fi 驱动程序
-  :ref:`AT+CWMODE <cmd-MODE>`                  ：查询/设置 Wi-Fi 模式 (Station/SoftAP/Station+SoftAP)
-  :ref:`AT+CWSTATE <cmd-WSTATE>`               ：查询 Wi-Fi 状态和 Wi-Fi 信息
-  :ref:`AT+CWJAP <cmd-JAP>`                    ：连接 AP
-  :ref:`AT+CWRECONNCFG <cmd-RECONNCFG>`        ：查询/设置 Wi-Fi 重连配置
-  :ref:`AT+CWLAPOPT <cmd-LAPOPT>`              ：设置 :ref:`AT+CWLAP <cmd-LAP>` 命令扫描结果的属性
-  :ref:`AT+CWLAP <cmd-LAP>`                    ：扫描当前可用的 AP
-  :ref:`AT+CWQAP <cmd-QAP>`                    ：断开与 AP 的连接
-  :ref:`AT+CWSAP <cmd-SAP>`                    ：配置 |Equipment-Name| SoftAP 参数
-  :ref:`AT+CWLIF <cmd-LIF>`                    ：查询连接到 |Equipment-Name| SoftAP 的 Station 信息
-  :ref:`AT+CWQIF <cmd-QIF>`                    ：断开 Station 与 |Equipment-Name| SoftAP 的连接
-  :ref:`AT+CWDHCP <cmd-DHCP>`                  ：启用/禁用 DHCP
-  :ref:`AT+CWDHCPS <cmd-DHCPS>`                ：查询/设置 |Equipment-Name| SoftAP DHCP 分配的 IPv4 地址范围
-  :ref:`AT+CWAUTOCONN <cmd-AUTOC>`             ：上电是否自动连接 AP
-  :ref:`AT+CWAPPROTO <cmd-APPROTO>`            ：查询/设置 SoftAP 模式下 802.11 b/g/n 协议标准
-  :ref:`AT+CWSTAPROTO <cmd-STAPROTO>`          ：设置 Station 模式下 802.11 b/g/n 协议标准
-  :ref:`AT+CIPSTAMAC <cmd-STAMAC>`             ：查询/设置 |Equipment-Name| Station 的 MAC 地址
-  :ref:`AT+CIPAPMAC <cmd-APMAC>`               ：查询/设置 |Equipment-Name| SoftAP 的 MAC 地址
-  :ref:`AT+CIPSTA <cmd-IPSTA>`                 ：查询/设置 |Equipment-Name| Station 的 IP 地址
-  :ref:`AT+CIPAP <cmd-IPAP>`                   ：查询/设置 |Equipment-Name| SoftAP 的 IP 地址
-  :ref:`AT+CWSTARTSMART（暂不支持）<cmd-STARTS>`：开启 SmartConfig
-  :ref:`AT+CWSTOPSMART（暂不支持）<cmd-STOPS>`  ：停止 SmartConfig
-  :ref:`AT+WPS（暂不支持）<cmd-WPS>`            ：设置 WPS 功能
-  :ref:`AT+MDNS（暂不支持）<cmd-MDNS>`          ：设置 mDNS 功能
-  :ref:`AT+CWJEAP（暂不支持）<cmd-JEAP>`        ：连接 WPA2 企业版 AP
-  :ref:`AT+CWHOSTNAME <cmd-HOSTNAME>`          ：查询/设置 |Equipment-Name| Station 的主机名称
-  :ref:`AT+CWCOUNTRY <cmd-COUNTRY>`            ：查询/设置 Wi-Fi 国家代码



.. _cmd-INIT:

:ref:`AT+CWINIT <WiFi-AT>`：初始化/清理 Wi-Fi 驱动程序
-------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| 设备的 Wi-Fi 初始化状态

**命令：**

::

    AT+CWINIT?

**响应：**

::

    +CWINIT:<init>
    OK

设置命令
^^^^^^^^

**功能：**

初始化或清理 |Equipment-Name| 设备的 Wi-Fi 驱动程序

**命令：**

::

    AT+CWINIT=<init>

**响应：**

::

    OK

参数
^^^^

-  **<init>**：

   -  0: 清理 Wi-Fi 驱动程序
   -  1: 初始化 Wi-Fi 驱动程序（默认值）

说明
^^^^

- 本设置不保存到 flash，重启后会恢复为默认值 1。
- 当您 RAM 资源不足时，在不使用 Wi-Fi 的前提下，可以使用此命令清理 Wi-Fi 驱动程序，以释放 RAM 资源。

示例
^^^^

::

    // 清理 Wi-Fi 驱动程序
    AT+CWINIT=0

.. _cmd-MODE:

:ref:`AT+CWMODE <WiFi-AT>`：查询/设置 Wi-Fi 模式 (Station/SoftAP/Station+SoftAP)
-------------------------------------------------------------------------------------


查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| 设备的 Wi-Fi 模式

**命令：**

::

    AT+CWMODE?

**响应：**

::

    +CWMODE:<mode>
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| 设备的 Wi-Fi 模式

**命令：**

::

    AT+CWMODE=<mode>[,<auto_connect>]

**响应：**

::

    OK

参数
^^^^

-  **<mode>**：模式

   -  0: 无 Wi-Fi 模式，并且关闭 Wi-Fi RF 
   -  1: Station 模式
   -  2: SoftAP 模式
   -  3: SoftAP+Station 模式

-  **<auto_connect>**：切换 |Equipment-Name| 设备的 Wi-Fi 模式时（例如，从 SoftAP 或无 Wi-Fi 模式切换为 Station 模式或 SoftAP+Station 模式），是否启用自动连接 AP 的功能，默认值：1。参数缺省时，使用默认值，也就是能自动连接。

   -  0: 禁用自动连接 AP 的功能
   -  1: 启用自动连接 AP 的功能，若之前已经将自动连接 AP 的配置保存到 flash 中，则 |Equipment-Name| 设备将自动连接 AP

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，本设置将保存在 NVS 分区

-  当使用 AT+CWMODE=1 切换回Station模式时默认会自动连接Wi-Fi

示例
^^^^

::

    AT+CWMODE=1 
  
.. _cmd-WSTATE:

:ref:`AT+CWSTATE <WiFi-AT>`：查询 Wi-Fi 状态和 Wi-Fi 信息
-------------------------------------------------------------


查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| 设备的 Wi-Fi 状态和 Wi-Fi 信息

**命令：**

::

    AT+CWSTATE?

**响应：**

.. code-block:: text

    +CWSTATE:<state>,<"ssid">
    OK

参数
^^^^

-  **<state>**：当前 Wi-Fi 状态

   - 0: |Equipment-Name| Station 尚未进行任何 Wi-Fi 连接
   - 1: |Equipment-Name| Station 已经连接上 AP，但尚未获取到 IPv4 地址
   - 2: |Equipment-Name| Station 已经连接上 AP，并已经获取到 IPv4 地址
   - 3: |Equipment-Name| Station 正在进行 Wi-Fi 连接或 Wi-Fi 重连
   - 4: |Equipment-Name| Station 处于 Wi-Fi 断开状态

-  **<"ssid">**：目标 AP 的 SSID

说明
^^^^

- 当 |Equipment-Name| Station 没有连接上 AP 时，推荐使用此命令查询 Wi-Fi 信息；当 |Equipment-Name| Station 已连接上 AP 后，推荐使用 :ref:`AT+CWJAP <cmd-JAP>` 命令查询 Wi-Fi 信息

.. _cmd-JAP:

:ref:`AT+CWJAP <WiFi-AT>`：连接 AP
------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询与 |Equipment-Name| Station 连接的 AP 信息

**命令：**

::

    AT+CWJAP?

**响应：**

::

    +CWJAP:<ssid>,<bssid>,<channel>,<rssi>,<pci_en>,<reconn_interval>,<listen_interval>,<scan_mode>,<pmf>

    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| Station 需连接的 AP

**命令：**

::

    AT+CWJAP=[<ssid>,][<pwd>][,<bssid>][,<pci_en>][,<reconn_interval>][,<listen_interval>][,<scan_mode>][,<jap_timeout>][,<pmf>]

**响应：**

::

    WIFI CONNECTED
    OK
    WIFI GOT IP
    [WIFI GOT IPv6 LL]
    [WIFI GOT IPv6 GL]

或

::

    ERROR

执行命令
^^^^^^^^

**功能：**

将 |Equipment-Name| Station 连接至上次 Wi-Fi 配置中的 AP

**命令：**

::

    AT+CWJAP

**响应：**

::

    WIFI CONNECTED
    OK
    WIFI GOT IP
    [WIFI GOT IPv6 LL]
    [WIFI GOT IPv6 GL]

或

::

    ERROR

参数
^^^^

-  **<ssid>**：目标 AP 的 SSID

   -  如果 SSID 和密码中有 ``,``、``"``、``\\`` 等特殊字符，需转义
   -  AT 支持连接 SSID 为中文的 AP，但是某些路由器或者热点的中文 SSID 不是 UTF-8 编码格式。您可以先扫描 SSID，然后使用扫描到的 SSID 进行连接。

-  **<pwd>**            ：字符串参数，密码，范围：8 ~ 63 字节 ASCII（或 64 字节 HEX）
-  **<bssid>**          ：目标 AP 的 MAC 地址，当多个 AP 有相同的 SSID 时，该参数不可省略
-  **<channel>**        ：信道号
-  **<rssi>**           ：信号强度
-  **<pci_en>**         ：PCI 认证

   - 0: |Equipment-Name| Station 可与任何一种加密方式的 AP 连接，包括 OPEN 和 WEP
   - 1: |Equipment-Name| Station 可与除 OPEN 和 WEP 之外的任何一种加密方式的 AP 连接

-  **<reconn_interval>**：Wi-Fi 重连间隔，单位：秒，默认值：1，最大值：7200

   -  0       : 断开连接后，|Equipment-Name| Station 不重连 AP
   -  [1,7200]: 断开连接后，|Equipment-Name| Station 每隔指定的时间与 AP 重连

-  **<listen_interval>**：监听 AP beacon 的间隔，单位为 AP beacon 间隔，默认值：3，范围：[1,100]
-  **<scan_mode>**      ：扫描模式

   -  0: 快速扫描，找到目标 AP 后终止扫描，|Equipment-Name| Station 与第一个扫描到的 AP 连接
   -  1: 全信道扫描，所有信道都扫描后才终止扫描，|Equipment-Name| Station 与扫描到的信号最强的 AP 连接

-  **<jap_timeout>**    ：:ref:`AT+CWJAP <cmd-JAP>` 命令超时的最大值，单位：秒，默认值：15，范围：[3,600]
-  **<pmf>**：PMF（Protected Management Frames，受保护的管理帧），默认值：1

   - 0 表示禁用 PMF
   - bit 0: 具有 PMF 功能，提示支持 PMF，如果其他设备具有 PMF 功能，则 |Equipment-Name| 设备将优先选择以 PMF 模式连接
   - bit 1: 需要 PMF，提示需要 PMF，设备将不会关联不支持 PMF 功能的设备

说明
^^^^

-  如果 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

-  使用本命令需要开启 station 模式
-  当 |Equipment-Name| station 已连接上 AP 后，推荐使用此命令查询 Wi-Fi 信息；当 |Equipment-Name| Station 没有连接上 AP 时，推荐使用 :ref:`AT+CWSTATE <cmd-WSTATE>` 命令查询 Wi-Fi 信息
-  本命令中的 ``<reconn_interval>`` 参数与 :ref:`AT+CWRECONNCFG <cmd-RECONNCFG>` 命令中的 ``<interval_second>`` 参数相同。如果运行本命令时不设置 ``<reconn_interval>`` 参数，Wi-Fi 重连间隔时间将采用默认值 1
-  如果同时省略 ``<ssid>`` 和 ``<password>`` 参数，将使用上一次设置的值
-  执行命令与设置命令的超时时间相同，默认为 15 秒，可通过参数 ``<jap_timeout>`` 设置
-  想要获取 IPv6 地址，需要先设置 :ref:`AT+CIPV6=1<cmd-IPV6>`
-  回复 ``OK`` 代表 IPv4 网络已经准备就绪，而不代表 IPv6 网络准备就绪。当前 WinnerMicro-AT 以 IPv4 网络为主，IPv6 网络为辅。
-  ``WIFI GOT IPv6 LL`` 代表已经获取到本地链路 IPv6 地址，这个地址是通过 EUI-64 本地计算出来的，不需要路由器参与。由于并行时序，这个打印可能在 ``OK`` 之前，也可能在 ``OK`` 之后。
-  ``WIFI GOT IPv6 GL`` 代表已经获取到全局 IPv6 地址，该地址是由 AP 下发的前缀加上内部计算出来的后缀进行组合而来的，需要路由器参与。由于并行时序，这个打印可能在 ``OK`` 之前，也可能在 ``OK`` 之后；也可能由于 AP 不支持 IPv6 而不打印。

示例
^^^^

::

    // 如果目标 AP 的 SSID 是 "WinnerMicro_WiFi"，密码是 "12345678"，则命令是：
    AT+CWJAP="WinnerMicro_WiFi","12345678"

    // 如果多个 AP 有相同的 SSID "WinnerMicro_WiFi"，可通过 BSSID 找到目标 AP：
    AT+CWJAP="WinnerMicro_WiFi","12345678","28:6d:cd:55:e5:7b" 

.. _cmd-RECONNCFG:

:ref:`AT+CWRECONNCFG <WiFi-AT>`：查询/设置 Wi-Fi 重连配置
-------------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 Wi-Fi 重连配置

**命令：**

::

    AT+CWRECONNCFG?

**响应：**

::

    +CWRECONNCFG:<interval_second>,<repeat_count>
    OK

设置命令
^^^^^^^^

**功能：**

设置 Wi-Fi 重连配置

**命令：**

::

    AT+CWRECONNCFG=<interval_second>,<repeat_count>

**响应：**

::

    OK

参数
^^^^

-  **<interval_second>**：Wi-Fi 重连间隔，单位：秒，默认值：0，最大值 7200

   -  0       : 断开连接后，|Equipment-Name| Station 不重连 AP
   -  [1,7200]: 断开连接后，|Equipment-Name| Station 每隔指定的时间与 AP 重连

-  **<repeat_count>**   ：|Equipment-Name| 设备尝试重连 AP 的次数，本参数在 ``<interval_second>`` 不为 0 时有效，默认值：0，最大值：1000

   -  0       : |Equipment-Name| Station 始终尝试连接 AP
   -  [1,1000]: |Equipment-Name| Station 按照本参数指定的次数重连 AP

示例
^^^^

.. parsed-literal::

    // |Equipment-Name| Station 每隔 1 秒尝试重连 AP，共尝试 100 次
    AT+CWRECONNCFG=1,100

    // |Equipment-Name| Station 在断开连接后不重连 AP
    AT+CWRECONNCFG=0,0

说明
^^^^

-  如果 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

-  本命令中的 ``<interval_second>`` 参数与 :ref:`AT+CWJAP <cmd-JAP>` 中的 ``[<reconn_interval>]`` 参数相同
-  该命令适用于被动断开 AP 、Wi-Fi 模式切换和开机后 Wi-Fi 自动连接

.. _cmd-LAPOPT:

:ref:`AT+CWLAPOPT <WiFi-AT>`：设置 :ref:`AT+CWLAP <cmd-LAP>` 命令扫描结果的属性
----------------------------------------------------------------------------------------------------

设置命令
^^^^^^^^

**命令：**

::

    AT+CWLAPOPT=<print mask>[,<rssi filter>][,<authmode mask>]

**响应：**

::

    OK

或者

::

    ERROR

参数
^^^^

-  **<print mask>**：:ref:`AT+CWLAP <cmd-LAP>` 的扫描结果是否显示以下参数，默认值：0x1FF，若 bit 设为 1，则显示对应参数，若设为 0，则不显示对应参数

   -  bit 0: 是否显示 <ecn>
   -  bit 1: 是否显示 <ssid>
   -  bit 2: 是否显示 <rssi>
   -  bit 3: 是否显示 <mac>
   -  bit 4: 是否显示 <channel>
   -  bit 5: 是否显示 <pairwise_cipher>
   -  bit 6: 是否显示 <group_cipher>
   -  bit 7: 是否显示 <bgn>
   -  bit 8: 是否显示 <wps>

-  **[<rssi filter>]**  ：:ref:`AT+CWLAP <cmd-LAP>` 的扫描结果是否按照本参数过滤，也即，是否过滤掉信号强度低于 ``rssi filter`` 参数值的 AP，单位：dBm，默认值：–100，范围：[–100,40]
-  **[<authmode mask>]**：:ref:`AT+CWLAP <cmd-LAP>` 的扫描结果是否显示以下认证方式的 AP，默认值：0x3FF，如果 ``bit x`` 设为 1，则显示对应认证方式的 AP，若设为 0，则不显示

   -  bit 0: 是否显示 ``OPEN`` 认证方式的 AP
   -  bit 1: 是否显示 ``WEP`` 认证方式的 AP
   -  bit 2: 是否显示 ``WPA_PSK`` 认证方式的 AP
   -  bit 3: 是否显示 ``WPA2_PSK`` 认证方式的 AP
   -  bit 4: 是否显示 ``WPA_WPA2_PSK`` 认证方式的 AP
   -  bit 5: 是否显示 ``WPA2_ENTERPRISE`` 认证方式的 AP
   -  bit 6: 是否显示 ``WPA3_PSK`` 认证方式的 AP
   -  bit 7: 是否显示 ``WPA2_WPA3_PSK`` 认证方式的 AP
   -  bit 8: 是否显示 ``WAPI_PSK`` 认证方式的 AP

示例
^^^^

::

    // 第一个参数为 31，即 0x1F，表示所有值为 1 的 bit 对应的参数都会显示出来
    AT+CWLAPOPT=31
    AT+CWLAP

    // 只显示认证方式为 OPEN 的 AP
    AT+CWLAPOPT=31,-100,1
    AT+CWLAP

.. _cmd-LAP:

:ref:`AT+CWLAP <WiFi-AT>`：扫描当前可用的 AP
------------------------------------------------------

设置命令
^^^^^^^^

**功能：**

列出符合特定条件的 AP，如指定 SSID、MAC 地址或信道号

**命令：**

::

    AT+CWLAP=[ssid,][mac,][channel,][scan_type,][scan_time_min,][scan_time_max]

执行命令
^^^^^^^^

**功能：**

列出当前可用的 AP

**命令：**

::

    AT+CWLAP

**响应：**

::

    +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<pairwise_cipher>,<group_cipher>,<bgn>,<wps>
    OK

参数
^^^^

-  **<ecn>**：加密方式

   -  0: OPEN
   -  1: WEP
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA_WPA2_PSK
   -  5: WPA2_ENTERPRISE
   -  6: WPA3_PSK
   -  7: WPA2_WPA3_PSK
   -  8: WAPI_PSK

-  **<ssid>**     ：字符串参数，AP 的 SSID
-  **<rssi>**     ：信号强度
-  **<mac>**      ：字符串参数，AP 的 MAC 地址
-  **<channel>**  ：信道号
-  **<scan_type>**：Wi-Fi 扫描类型，默认值为：0

   -  0: 主动扫描
   -  1: 被动扫描
-  **<scan_time_min>**：每个信道最短扫描时间，单位：毫秒，范围：[0,1500]，如果扫描类型为被动扫描，本参数无效
-  **<scan_time_max>**：每个信道最长扫描时间，单位：毫秒，范围：[0,1500]，如果设为 0，固件采用参数默认值，主动扫描为 120 ms，被动扫描为 360 ms

-  **<pairwise_cipher>**：成对加密类型

   - 0: None
   - 1: WEP40
   - 2: WEP104
   - 3: TKIP
   - 4: CCMP
   - 5: TKIP and CCMP
   - 6: AES-CMAC-128
   - 7: 未知

-  **<group_cipher>**：组加密类型，与 ``<pairwise_cipher>`` 参数的枚举值相同
-  **<bgn>**          ：802.11 b/g/n，若 bit 设为 1，则表示使能对应模式，若设为 0，则表示禁用对应模式

   -  bit 0: 是否使能 802.11b/g 模式
   -  bit 2: 是否使能 802.11n 模式

-  **<wps>**：wps flag

   - 0: 不支持 WPS
   - 1: 支持 WPS


说明
^^^^^

- 使用时需注意，当 station 联网过程中执行扫描，可能会失败（自动重连亦然）。


示例
^^^^

.. code-block:: text

    AT+CWLAP="WinnerMicro_WiFi","42:c1:82:80:2a:15",6,0
    
    // 寻找指定 SSID 的 AP
    AT+CWLAP="WinnerMicro_WiFi"

.. _cmd-QAP:

:ref:`AT+CWQAP <WiFi-AT>`：断开与 AP 的连接
--------------------------------------------------------

执行命令
^^^^^^^^

**命令：**

::

    AT+CWQAP

**响应：**

::

    OK

.. _cmd-SAP:

:ref:`AT+CWSAP <WiFi-AT>`：配置 |Equipment-Name| SoftAP 参数
-------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| SoftAP 的配置参数

**命令：**

::

    AT+CWSAP?

**响应：**

::

    +CWSAP:<ssid>,<pwd>,<channel>,<ecn>,<max conn>,<ssid hidden>
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| SoftAP 的配置参数

**命令：**

::

    AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>[,<max conn>][,<ssid hidden>]

**响应：**

::

    OK

参数
^^^^

-  **<ssid>**   ：字符串参数，接入点名称
-  **<pwd>**    ：字符串参数，密码，范围：8 ~ 63 字节 ASCII（或 64 字节 HEX）
-  **<channel>**：信道号
-  **<ecn>**    ：加密方式，不支持 WEP

   -  0: OPEN
   -  2: WPA_PSK
   -  3: WPA2_PSK
   -  4: WPA_WPA2_PSK

-  **[<max conn>]**   ：允许连入 |Equipment-Name| SoftAP 的最多 Station 数目，取值范围：参考 `待做 max_connection 描述 <https://#>`_。
-  **[<ssid hidden>]**：

   -  0: 广播 SSID（默认）
   -  1: 不广播 SSID

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存在 NVS 分区

- 本指令只有当 :ref:`AT+CWMODE=2 <cmd-MODE>` 或者 :ref:`AT+CWMODE=3 <cmd-MODE>` 时才有效
- 默认 SSID 因设备而异，因为它由设备的 MAC 地址组成。 您可以使用 :ref:`AT+CWSAP? <cmd-SAP>` 查询默认的SSID。

示例
^^^^

.. code-block:: text

    AT+CWSAP="WinnerMicro_WiFi","12345678",5,3

.. _cmd-LIF:

:ref:`AT+CWLIF <WiFi-AT>`：查询连接到 |Equipment-Name| SoftAP 的 Station 信息
---------------------------------------------------------------------------------------------

执行命令
^^^^^^^^

**命令：**

::

    AT+CWLIF

**响应：**

::

    +CWLIF:<ip addr>,<mac>
    OK

参数
^^^^

-  **<ip addr>**：连接到 |Equipment-Name| SoftAP 的 Station 的 IP 地址
-  **<mac>**    ：连接到 |Equipment-Name| SoftAP 的 Station 的 MAC 地址

说明
^^^^^

- 本指令无法查询静态 IP，仅支持在 |Equipment-Name| SoftAP 和连入的 station DHCP 均使能的情况下有效

.. _cmd-QIF:

:ref:`AT+CWQIF <WiFi-AT>`：断开 Station 与 |Equipment-Name| SoftAP 的连接
------------------------------------------------------------------------------------

执行命令
^^^^^^^^

**功能：**

断开所有连入 |Equipment-Name| SoftAP 的 Station

**命令：**

::

    AT+CWQIF

**响应：**

::

    OK

设置命令
^^^^^^^^

**功能：**

断开某个连入 |Equipment-Name| SoftAP 的 Station

**命令：**

::

    AT+CWQIF=<mac>

**响应：**

::

    OK

参数
^^^^

-  **<mac>**：需断开连接的 Station 的 MAC 地址

.. _cmd-DHCP:

:ref:`AT+CWDHCP <WiFi-AT>`：启用/禁用 DHCP
-----------------------------------------------------

查询命令
^^^^^^^^

**命令：**

::

    AT+CWDHCP?

**响应：**

::

  +CWDHCP:<state>
  OK

设置命令
^^^^^^^^

**功能：**
 
启用/禁用 DHCP

**命令：**

::

    AT+CWDHCP=<operate>,<mode>

**响应：**

::

    OK

参数
^^^^

-  **<operate>**：

   -  0: 禁用
   -  1: 启用

-  **<mode>**：

   -  Bit0: Station 的 DHCP
   -  Bit1: SoftAP 的 DHCP

-  **<state>**：DHCP 的状态
   
   - Bit0:

     - 0: 禁用 Station 的 DHCP
     - 1: 启用 Station 的 DHCP
   
   - Bit1:

     - 0: 禁用 SoftAP 的 DHCP
     - 1: 启用 SoftAP 的 DHCP

   - Bit2:

     - 0: 禁用 Ethernet 的 DHCP
     - 1: 启用 Ethernet 的 DHCP

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

-  本设置命令与设置静态 IPv4 地址的命令会相互影响，如 :ref:`AT+CIPSTA <cmd-IPSTA>`
  
  -  若启用静态 IPv4 地址，则自动禁用 DHCP.
  -  若启用 DHCP，则自动禁用静态 IPv4 地址.
  -  最后一次配置会覆盖上一次配置
  
示例
^^^^

::

    // 启用 Station DHCP，如果原 DHCP mode 为 2，则现 DHCP mode 为 3
    AT+CWDHCP=1,1

    // 禁用 SoftAP DHCP，如果原 DHCP mode 为 3，则现 DHCP mode 为 1
    AT+CWDHCP=0,2


.. _cmd-DHCPS:

:ref:`AT+CWDHCPS <WiFi-AT>`：查询 |Equipment-Name| SoftAP DHCP 分配的 IPv4 地址范围
-----------------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**命令：**

::

    AT+CWDHCPS?

**响应：**

::

    +CWDHCPS=<lease time>,<begin IP>,<end IP>
    OK

设置命令
^^^^^^^^

**功能：**

设置 SoftAP DHCP 服务器分配的 IPv4 地址范围

**命令：**

::

    AT+CWDHCPS=<enable>,<lease time>,<begin IP>,<end IP>

**响应：**

::

    OK

参数
^^^^

-  **<enable>**：

   - 0: 清除 DHCP server 信息，恢复默认值，后续参数无需填写
   - 1: 设置 DHCP server 信息，后续参数必须填写

-  **<lease time>**：租约时间，单位：分钟，取值范围：[1,2880]
-  **<begin IP>**  ：|Equipment-Name| SoftAP DHCP 服务器 IPv4 地址池的起始 IP
-  **<end IP>**    ：|Equipment-Name| SoftAP DHCP 服务器 IPv4 地址池的结束 IP

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>` 时，配置更改将保存在 NVS 区。

-  本命令必须在 |Equipment-Name| SoftAP 模式使能，且开启 DHCP server 的情况下使用

-  设置的 IPv4 地址范围必须与 |Equipment-Name| SoftAP 在同一网段

.. tip::

    如果想更改 SoftAP 的 DHCP 范围，可以通过 AT+CIPAP=<"ip">[,<"gateway">,<"netmask">]
    来改变 SoftAP 的 IP ，再打开 SoftAP 就可以改变 DHCP 的范围

.. _cmd-AUTOC:

:ref:`AT+CWAUTOCONN <WiFi-AT>`：上电是否自动连接 AP
--------------------------------------------------------------------------------

设置命令
^^^^^^^^

**命令：**

::

    AT+CWAUTOCONN=<enable>

**响应：**

::

    OK

参数
^^^^

-  **<enable>**：

   -  1: 上电自动连接 AP（默认）   
   -  0: 上电不自动连接 AP

说明
^^^^

-  本设置保存到 NVS 区域

示例
^^^^

::

    AT+CWAUTOCONN=1

.. _cmd-APPROTO:

:ref:`AT+CWAPPROTO <WiFi-AT>`：查询/设置 SoftAP 模式下 802.11 b/g/n 协议标准
------------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**命令：**

::

    AT+CWAPPROTO?

**响应：**

::

    +CWAPPROTO=<protocol>
    OK

设置命令
^^^^^^^^

**命令：**

::

    AT+CWAPPROTO=<protocol>

**响应：**

::

    OK

参数
^^^^

-  **<protocol>**：

   -  bit0: 802.11b 协议标准
   -  bit1: 802.11b/g 协议标准
   -  bit2: 802.11b/g/n 协议标准

说明
^^^^

-  默认情况下，|Equipment-Name| 设备的 PHY mode 是 802.11bgn 模式

.. _cmd-STAPROTO:

:ref:`AT+CWSTAPROTO <WiFi-AT>`：设置 Station 模式下 802.11 b/g/n 协议标准
--------------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**命令：**

::

    AT+CWSTAPROTO?

**响应：**

::

    +CWSTAPROTO=<protocol>
    OK

设置命令
^^^^^^^^

**命令：**

::

    AT+CWSTAPROTO=<protocol>

**响应：**

::

    OK

参数
^^^^

-  **<protocol>**：

.. only:: w800

   -  bit0: 802.11b 协议标准
   -  bit1: 802.11b/g 协议标准
   -  bit2: 802.11b/g/n 协议标准


说明
^^^^

-  默认情况下，|Equipment-Name| 设备的 PHY mode 是 802.11bgn 模式

.. _cmd-STAMAC:

:ref:`AT+CIPSTAMAC <WiFi-AT>`：查询/设置 |Equipment-Name| Station 的 MAC 地址
--------------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| Station 的 MAC 地址

**命令：**

::

    AT+CIPSTAMAC?

**响应：**

::

    +CIPSTAMAC:<mac>
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| Station 的 MAC 地址

**命令：**

::

    AT+CIPSTAMAC=<mac>

**响应：**

::

    OK

参数
^^^^

-  **<mac>**：字符串参数，表示 |Equipment-Name| Station 的 MAC 地址

说明
^^^^

- 若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

- |Equipment-Name| Station 的 MAC 地址与 |Equipment-Name| SoftAP  
  不同，不要为二者设置同样的 MAC 地址
- MAC 地址的 Bit 0 不能为 1，例如，MAC 地址可以是 "18:…"，但不可以是 "15:…"
- FF:FF:FF:FF:FF:FF 和 00:00:00:00:00:00 是无效地址，不能设置

示例
^^^^

.. code-block:: text

    AT+CIPSTAMAC="18:6d:cd:55:e5:7b"

.. _cmd-APMAC:

:ref:`AT+CIPAPMAC <WiFi-AT>`：查询/设置 |Equipment-Name| SoftAP 的 MAC 地址
------------------------------------------------------------------------------------

查询命令
^^^^^^^^
**功能：**

查询 |Equipment-Name| SoftAP 的 MAC 地址

**命令：**

::

    AT+CIPAPMAC?

**响应：**

::

    +CIPAPMAC:<mac>
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| SoftAP 的 MAC 地址

**命令：**

::

    AT+CIPAPMAC=<mac>

**响应：**

::

    OK

参数
^^^^

-  **<mac>**：字符串参数，表示 |Equipment-Name| SoftAP 的 MAC 地址

说明
^^^^

- 若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

- |Equipment-Name| SoftAP 的 MAC 地址
  与 |Equipment-Name| Station 不同，不要为二者设置同样的 MAC 地址
- MAC 地址的 Bit 0 不能为 1，例如，MAC 地址可以是 "18:…"，但不可以是 "15:…"
- FF:FF:FF:FF:FF:FF 和 00:00:00:00:00:00 是无效地址，不能设置

示例
^^^^

.. code-block:: text

    AT+CIPAPMAC="18:fe:35:98:d3:7b" 

.. _cmd-IPSTA:

:ref:`AT+CIPSTA <WiFi-AT>`：查询/设置 |Equipment-Name| Station 的 IP 地址
-----------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| Station 的 IP 地址

**命令：**

::

    AT+CIPSTA?

**响应：**

.. code-block:: text

    +CIPSTA:ip:<"ip">
    +CIPSTA:gateway:<"gateway">
    +CIPSTA:netmask:<"netmask">
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| Station 的 IPv4 地址

**命令：**

.. code-block:: text

    AT+CIPSTA=<"ip">[,<"gateway">,<"netmask">]

**响应：**

::

    OK

参数
^^^^

-  **<"ip">**     ：字符串参数，表示 |Equipment-Name| Station 的 IPv4 地址
-  **<"gateway">**：网关
-  **<"netmask">**：子网掩码

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

-  使用查询命令时，只有当 |Equipment-Name| Station 连入 AP 或者配置过静态 IP 地址后，才能查询到它的 IP 地址
-  本设置命令与设置 DHCP 的命令相互影响，如 :ref:`AT+CWDHCP <cmd-DHCP>`
  
  -  若启用静态 IPv4 地址，则自动禁用 DHCP.
  -  若启用 DHCP，则自动禁用静态 IPv4 地址.
  -  最后一次配置会覆盖上一次配置
  
示例
^^^^

.. code-block:: text

    AT+CIPSTA="192.168.6.100","192.168.6.1","255.255.255.0" 

.. _cmd-IPAP:

:ref:`AT+CIPAP <WiFi-AT>`：查询/设置 |Equipment-Name| SoftAP 的 IP 地址
-------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| SoftAP 的 IP 地址

**命令：**

::

    AT+CIPAP?

**响应：**

.. code-block:: text

    +CIPAP:ip:<"ip">
    +CIPAP:gateway:<"gateway">
    +CIPAP:netmask:<"netmask">
    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| SoftAP 的 IPv4 地址

**命令：**

.. code-block:: text

    AT+CIPAP=<"ip">[,<"gateway">,<"netmask">]

**响应：**

::

    OK

参数
^^^^

-  **<"ip">**     ：字符串参数，表示 |Equipment-Name| SoftAP 的 IPv4 地址
-  **<"gateway">**：网关
-  **<"netmask">**：子网掩码

说明
^^^^

-  若 :ref:`AT+SYSSTORE=1 <cmd-SYSSTORE>`，配置更改将保存到 NVS 分区

-  本设置命令仅适用于 IPv4 网络，不适用于 IPv6 网络
  
示例
^^^^

.. code-block:: text

    AT+CIPAP="192.168.5.1","192.168.5.1","255.255.255.0"

.. _cmd-STARTS:

:ref:`AT+CWSTARTSMART（暂不支持）<cmd-STARTS>`：开启 SmartConfig
-------------------------------------------------------------------------------

.. _cmd-STOPS:

:ref:`AT+CWSTOPSMART（暂不支持）<cmd-STOPS>`：停止 SmartConfig
-------------------------------------------------------------------------------

.. _cmd-WPS:

:ref:`AT+WPS（暂不支持）<cmd-WPS>`    ：设置 WPS 功能
-------------------------------------------------------------------------------

.. _cmd-MDNS:

:ref:`AT+MDNS（暂不支持）<cmd-MDNS>`  ：设置 mDNS 功能
-------------------------------------------------------------------------------

.. _cmd-JEAP:

:ref:`AT+CWJEAP（暂不支持）<cmd-JEAP>`：连接 WPA2 企业版 AP
-------------------------------------------------------------------------------

.. _cmd-HOSTNAME:

:ref:`AT+CWHOSTNAME <WiFi-AT>`：查询/设置 |Equipment-Name| Station 的主机名称
-------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 |Equipment-Name| Station 的主机名称

**命令：**

::

    AT+CWHOSTNAME?

**响应：**

::

    +CWHOSTNAME:<hostname>

    OK

设置命令
^^^^^^^^

**功能：**

设置 |Equipment-Name| Station 的主机名称

**命令：**

::

    AT+CWHOSTNAME=<hostname>

**响应：**

::

    OK

若没开启 Station 模式，则返回：

::

    CMD PARSE ERR

参数
^^^^

-  **<hostname>**：|Equipment-Name| Station 的主机名称，最大长度：32 字节

说明
^^^^

-  配置更改不保存到 flash

示例
^^^^

.. code-block:: text

    AT+CWMODE=3
    AT+CWHOSTNAME="WinnerMicro_WiFi"

.. _cmd-COUNTRY:

:ref:`AT+CWCOUNTRY <WiFi-AT>`：查询/设置 Wi-Fi 国家代码
--------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

查询 Wi-Fi 国家代码

**命令：**

::

    AT+CWCOUNTRY?

**响应：**

::

    +CWCOUNTRY:<country_code>,<start_channel>,<total_channel_count>
    OK

设置命令
^^^^^^^^

**功能：**

设置 Wi-Fi 国家代码

**命令：**

::

    AT+CWCOUNTRY=<country_code>,<start_channel>,<total_channel_count>

**响应：**

::

    OK

参数
^^^^

-  **<country_code>** ：国家码，最大长度：3 个字符，
    当前国家码只支持 CN,JP,US,CA,AL,AM,AT,AZ,BA,BE,BG,BY,CH,CY,CZ,DE,DK,EE,EL,ES,FI,FR,
    GE,HR,HU,IE,IS,IT,LI,LT,LU,LV,MD,ME,MK,MT,NL,NO,PL,PT,RO,RS,RU,SE,SI,SK,TR,UA,UK。
-  **<start_channel>**      ：起始信号道，范围：[1,14]
-  **<total_channel_count>**：信道总个数

说明
^^^^

-  各国国家码请参考 `ISO 3166-1 alpha-2 <https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2>`_ 标准。
-  配置更改不保存到 flash

示例
^^^^

.. code-block:: text

    AT+CWMODE=3
    AT+CWCOUNTRY="CN",1,13
