
.. |Equipment-Name| replace:: w800

.. _Bluetooth_AT:
.. _cmd-EINIT:

****************************
Bluetooth AT 命令集
****************************


-  :ref:`介绍 <cmd-bluetooth-intro>`      
-  :ref:`AT+BLEINIT <cmd-EINIT>`                     : Bluetooth LE 初始化
-  :ref:`AT+BLEVERS <cmd-EVERS>`                     : 查询Bluetooth LE 版本信息
-  :ref:`AT+BLEADDR <cmd-EADDR>`                     : 设置 Bluetooth LE 设备地址
-  :ref:`AT+BLENAME <cmd-ENAME>`                     : 查询/设置 Bluetooth LE 设备名称
-  :ref:`AT+BLESCANPARAM <cmd-ESCANPARAM>`           : 查询/设置 Bluetooth LE 扫描参数
-  :ref:`AT+BLESCAN <cmd-ESCAN>`                     : 使能 Bluetooth LE 扫描 
-  :ref:`AT+BLBLESCANRSPDATA <cmd-ESCANRSPDATA>`     : 设置 Bluetooth LE 扫描响应
-  :ref:`AT+BLEADVPARAM <cmd-EADVPARAM>`             : 查询/设置 Bluetooth LE 广播参数
-  :ref:`AT+BLEADVDATA <cmd-EADVDATA>`               : 设置 Bluetooth LE 广播数据
-  :ref:`AT+BLEADVDATAEX <cmd-EADVDATAEX>`           : 自动设置 Bluetooth LE 广播数据
-  :ref:`AT+BLEADVSTART <cmd-EADVSTART>`             : 开始 Bluetooth LE 广播
-  :ref:`AT+BLEADVSTOP <cmd-EADVSTOP>`               : 停止 Bluetooth LE 广播
-  :ref:`AT+BLECONN <cmd-ECONN>`                     : 建立 Bluetooth LE 连接
-  :ref:`AT+BLECONNPARAM <cmd-ECONNPARAM>`           ：查询/更新 Bluetooth LE 连接参数
-  :ref:`AT+BLEDISCONN <cmd-EDISCONN>`               ：断开 Bluetooth LE 连接
-  :ref:`AT+BLEDATALEN <cmd-EDATALEN>`               ：设置 Bluetooth LE 数据包长度
-  :ref:`AT+BLECFGMTU <cmd-ECFGMTU>`                 ：设置 Bluetooth LE MTU 长度
-  :ref:`AT+BLEGATTSSVC <cmd-EGATTSSVC>`             ：GATTS 创建/查询本地服务      
-  :ref:`AT+BLEGATTSSVCRMV <cmd-EGATTSSVCRMV>`       ：GATTS 删除本地服务
-  :ref:`AT+BLEGATTSNTFY <cmd-EGATTSNTFY>`           ：服务器 notify 服务特征值给客户端
-  :ref:`AT+BLEGATTSIND <cmd-EGATTSIND>`             ：服务器 indicate 服务特征值给客户端
-  :ref:`AT+BLEGATTSSETATTR <cmd-EGATTSSETATTR>`     ：GATTS 设置服务特征值
-  :ref:`AT+BLEGATTCSVC <cmd-EGATTCSVC>`             ：GATTC 发现服务列表 
-  :ref:`AT+BLEGATTCRD <cmd-EGATTCRD>`               ：GATTC 读取服务特征值
-  :ref:`AT+BLEGATTCWR <cmd-EGATTCWR>`               ：GATTC 写服务特征值
-  :ref:`AT+BLEGATTCSUBS <cmd-EGATTCSUBS>`           ：GATTC 订阅服务
-  :ref:`AT+BLESECPARAM <cmd-ESECPARAM>`             ：查询/设置加密参数
-  :ref:`AT+BLEENC <cmd-EENC>`                       ：发起 Bluetooth LE 加密请求
-  :ref:`AT+BLEENCRSP <cmd-EENCRSP>`                 ：回复对端设备发起的配对请求
-  :ref:`AT+BLEKEYREPLY <cmd-EKEYREPLY>`             ：给对方设备回复密钥
-  :ref:`AT+BLECONFREPLY <cmd-ECONFREPLY>`           ：给对方设备回复确认结果（传统连接阶段）
-  :ref:`AT+BLEENCDEV <cmd-EENCDEV>`                 ：查询绑定的 Bluetooth LE 加密设备列表
-  :ref:`AT+BLEENCCLEAR <cmd-EENCCLEAR>`             ：清除 Bluetooth LE 加密设备列表
-  :ref:`AT+BLEVERS <cmd-EVERS>`                     ：查询 Bluetooth LE 主机和控制器固件版本号
-  :ref:`AT+BLESPPSCFG <cmd-ESPPSCFG>`               ：配置 Bluetooth LE SPP 服务端透传参数
-  :ref:`AT+BLESPPCCFG <cmd-ESPPCCFG>`               ：配置 Bluetooth LE SPP 客户端透传参数
-  :ref:`AT+BLESPPSTART <cmd-ESPPSTART>`             ：开启 Bluetooth LE SPP 透传功能
-  :ref:`AT+BLESPPSTOP <cmd-ESPPSTOP>`               ：关闭 Bluetooth LE SPP 透传功能
-  :ref:`AT+BLEWIFIPROV <cmd-EWIFIPROV>`             ：查询/设置 Bluetooth LE WIFI 加网功能
-  :ref:`AT+BTTEST <cmd-EBTTEST>`                    ：使能/退出蓝牙测试模式

.. _cmd-bluetooth-intro:

介绍
------

当前， |Equipment-Name| 系列AT固件支持蓝牙核心规范4.2版本。

.. important::

  	默认的 AT 固件支持此页面下的所有 AT 命令。如果您需要修改 |Equipment-Name| 默认支持的命令，请自行 编译 BLUETOOTH-AT 工程，在第五步配置工程里选择（下面每项是独立的，根据您的需要选择）：

 -  禁用Bluetooth SPP 命令
 -  禁用WIFIPROV命令
 -  禁用Bluetooth LE命令

.. _cmd-BLEINIT:

:ref:`AT+BLEINIT <Bluetooth_AT>`：Bluetooth LE 初始化
------------------------------------------------------------------

查询命令
^^^^^^^^^^^^

**功能：**

检查 Bluetooth LE 的初始化状态。

**命令：**

::

   AT+BLEINIT?
    
**响应：**

若已初始化，AT 返回：

::

   ++BLEINIT:<role>
   OK

若未初始化，AT 返回：

::

    +BLEINIT:0
    OK

设置命令
^^^^^^^^^^^^

**功能：**

初始化 Bluetooth LE 的角色。

**命令：**

::

   AT+BLEINIT=<init>

**响应：**

::

    OK

参数
^^^^

-  **<初始化>**: 

-  0: 注销 Bluetooth LE 协议栈
-  1: 初始化 Bluetooth LE 协议栈

笔记
^^^^

- 使用蓝牙 LE 功能时，如果不需要使用 SoftAP 模式，建议使用 AT+CWMODE 将 Wi-Fi 模式设置为 NULL 或 station 模式。
- 在使用蓝牙 LE AT 命令之前，您应该先运行此命令以触发初始化过程。
- 在取消初始化蓝牙堆栈之前，需要停止广播、停止扫描并断开所有现有连接。
- 如果 Bluetooth LE 已初始化，则 AT+CIPMODE 不能设置为 1。

示例
^^^^

::

   AT+BLEINIT=1

.. _cmd-EADDR:

:ref:`AT+BLEADDR<Bluetooth_AT>`：查询/设置低功耗蓝牙设备地址
---------------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

::

   查询 Bluetooth LE 公共地址.

::

   AT+BLEADDR?

**响应：**

::

   +BLEADDR:<BLE_public_addr>
   OK 
                        
设置命令
^^^^^^^^

**功能**

::

  设置 Bluetooth LE 地址类型。

**命令**

::

   AT+BLEADDR=<addr_type>[,<random_addr>]

**响应：**

::

   OK

参数
^^^^

    -  **<addr_type>**  ：  
    - 0：公共广播 
    - 1：随机地址 

注意
^^^^

- 静态地址应满足以下要求：
- 地址的两个最高有效位应等于1。
- 地址的随机部分至少应有一位为 0。
- 地址的随机部分至少应有一位为 1。
- 配置更改会保存在 NVS 区域中。

示例
^^^^   

::

   AT+BLEADDR=1,"f8:7f:24:87:1c:7b"    // 设置随机设备使用静态地址，具体地址为："f8:7f:24:87:1c:7b"
   AT+BLEADDR=1                        // 设置随机设备使用静态地址，具体地址由主机协议栈自动生成
   AT+BLEADDR=0                        // 设置随机设备使用公共设备地址

.. _cmd-EVERS:

:ref:`AT+BLEVERS <Bluetooth_AT>`：查询Bluetooth LE版本信息
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

::

   查询 Bluetooth LE 固件版本号。  

**命令：**

::
 
   AT+BLEVERS?

**响应：**

::

   +BLEVERS:<h-major>,<h-minor>,<h-patch>.<c-major>,<c-minor>,<c-patch>
   OK

    
参数
^^^^^
 
-  **<"h-major">**：Bluetooth LE 主机协议栈主版本号。
-  **<"h-minor">**：Bluetooth LE 主机协议栈次版本号。
-  **<"h-patch">**：Bluetooth LE 主机协议栈补丁版本号。
-  **<"c-major">**：Bluetooth LE 控制器协议栈主版本号。
-  **<"c-major">**：Bluetooth LE 控制器协议栈次版本号。
-  **<"c-major">**：Bluetooth LE 控制器协议栈补丁版本号。

示例
^^^^

::

   AT+BLEVERS?

.. _cmd-ENAME:

:ref:`AT+BLENAME <Bluetooth_AT>`：查询/设置蓝牙 LE 设备名称
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

::

   查询 Bluetooth LE 设备名称。  

**命令：**

::
 
   AT+BLENAME?

**响应：**

::

   +BLENAME:<device_name>
   OK

设置命令
^^^^^^^^

**功能：**

::

   设置 Bluetooth LE 设备名称。  

**命令：**

::
 
   AT+BLENAME=<device_name>

**响应：**

::

   OK
    
参数
^^^^^
 
-  **<"device_name">**：Bluetooth LE 设备名称。最大长度为 32。默认值：“WM-AT”。

注意
^^^^
   
- 配置更改将保存在 NVS 区域中。
- 使用此命令设置设备名称后，建议您执行 AT+BLEADVDATA 命令，将设备名称添加到广告数据中。

示例
^^^^

.. code-block:: text

   AT+BLENAME="wm-iot-dev"
 
.. _cmd-ESCANPARAM:   
  
:ref:`AT+BLESCANPARAM <Bluetooth_AT>`：查询/设置 Bluetooth LE 扫描参数
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**

::

   查询 Bluetooth LE 扫描参数

**命令：**

::

   AT+BLESCANPARAM?

**响应：**

::

   +BLESCANPARAM:<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE 扫描参数

**命令：**

::

   AT+BLESCANPARAM=<scan_type>,<own_addr_type>,<filter_policy>,<scan_interval>,<scan_window>

**响应：**

::

   OK

参数
^^^^

-  **<"scan_type">**：扫描类型
-  0: 被动扫描
-  1: 主动扫描
-  **<"own_addr_type">**：地址类型
- 0: 公共地址
- 1: 随机地址
- 2: RPA 公共地址
- 3: RPA 随机地址
-  **<"filter_policy">**：扫描过滤方式
- 0: BLE_SCAN_FILTER_ALLOW_ALL
- 1: BLE_SCAN_FILTER_ALLOW_ONLY_WLST
-  **<"scan_interva">**：扫描间隔,本参数值应大于等于 <scan_window> 参数值。参数范围：[0x0004,0x4000]。扫描间隔是该参数乘以 0.625 毫秒，所以实际的扫描间隔范围为 [2.5,10240]毫秒。
-  **<"scan_window">**：扫描窗口,本参数值应小于等于 <scan_interval> 参数值。参数范围：[0x0004,0x4000]。扫描窗口是该参数乘以 0.625 毫秒，所以实际的扫描窗口范围为 [2.5,10240]毫秒。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   // 初始化Bluetooth LE协议栈
   AT+BLESCANPARAM=0,0,0,100,50

.. _cmd-ESCAN:   
 
:ref:`AT+BLESCAN <Bluetooth_AT>`：使能 Bluetooth LE 扫描
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   开始/停止 Bluetooth LE 扫描
 
**命令：**

::

   AT+BLESCAN=<enable>[,<duration>][,<filter_type>,<filter_param>]

**响应：**

::

   +BLESCAN:<addr>,<rssi>,<data_type>,<data>
   OK
   +BLESCANDONE

参数
^^^^
   
-  **<"enable">**：
-  1: 开始持续扫描
-  0: 停止持续扫描 
-  **[<"duration">]**：扫描持续时间，单位：秒。
-  若设置停止扫描，无需设置本参数；
-  若设置开始扫描，需设置本参数：
-  本参数设为 0 时，则表示开始持续扫描；
-  本参数设为非 0 值时，例如 AT+BLESCAN=1,3 则表示扫描 3 秒后自动结束扫描。
-  **[<"ilter_type">]**：过滤选项
-  1: “MAC”
-  2: “NAME”
-  3: “RSSI”
-  **<"filter_param">**：过滤参数，表示对方设备 MAC 地址或名称
-  **<"addr">**：Bluetooth LE 地址
-  **<"<rssi">**：信号强度
-  **<"<data_type">**：数据类型
-  0: “Scannable and connectable advertising”
-  1: “Directed connectable advertising”
-  2: “Non-connectable and scannable advertising”
-  3: “Non-connectable and non-scannable advertising”
-  4: “Additional advertising data requested by an active scanner”
-  5: “Extended advertising, see advertising properties”
-  **<"data">**：数据内容

示例
^^^^

::

   AT+BLEINIT=1    // 初始化 Bluetooth LE协议栈
   AT+BLESCAN=1    // 开始扫描
   AT+BLESCAN=0    // 停止扫描
   AT+BLESCAN=1,3,1,"28:6D:C5:96:E6:68"  // 开始扫描，过滤类型为 MAC 地址
   AT+BLESCAN=1,3,2,"wm-iot-dev"  // 开始扫描，过滤类型为设备名称
   AT+BLESCAN=1,3,3,-70 // 开始扫描 ，过滤类型为信号强度大于-70dBm

.. _cmd-ESCANRSPDATA:   
    
:ref:`AT+BLESCANRSPDATA <Bluetooth_AT>`：设置 Bluetooth LE 扫描响应
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE 扫描响应
 
**命令：**

::

   AT+BLESCANRSPDATA=<scan_rsp_data>

**响应：**

::

   OK
  
参数
^^^^^^^^

-**<"scan_rsp_data">**：扫描响应数据，为 HEX 字符串。例如，若想设置扫描响应数据为 “0x04 0x09 0x31 0x32 0x33”，则命令为 AT+BLESCANRSPDATA="0409313233"。

注意
^^^^

- 扫描数据长度最大为31字节 
- 扫描响应数据必须遵守 Bluetooth SIG 定义的 LTV 格式 

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   // 初始化 Bluetooth LE协议栈
   AT+BLESCANRSPDATA="0409313233"

.. _cmd-EADVPARAM:    
   
:ref:`AT+BLEADVPARAM <Bluetooth_AT>`：查询/设置 Bluetooth LE 广播参数
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询广播参数
 
**命令：**

::

   AT+BLEADVPARAM?

**响应：**

::

   +BLEADVPARAM:<adv_int_min>,<adv_int_max>,<adv_type>,<own_addr_type>,<channel_map>,<adv_filter_policy>,<peer_addr_type>,<peer_addr>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

   设置广播参数
 
**命令：**

::

   AT+BLEADVPARAM=<adv_int_min>,<adv_int_max>,<adv_type>,<own_addr_type>,<channel_map>[,<adv_filter_policy>][,<peer_addr_type>,<peer_addr>]

**响应：**

::

   OK

参数
^^^^

-  **<"adv_int_min">**：最小广播间隔。参数范围：[0x0020,0x4000]。广播间隔等于该参数乘以 0.625 毫秒，所以实际的最小广播间隔范围为 [20,10240] 毫秒。本参数值应小于等于 <adv_int_max> 参数值。
-  **<"adv_int_max">**：最大广播间隔。参数范围：[0x0020,0x4000]。广播间隔等于该参数乘以 0.625 毫秒，所以实际的最大广播间隔范围为 [20,10240] 毫秒。本参数值应大于等于 <adv_int_min> 参数值。
-  **<"adv_type">**：
-  0: ADV_TYPE_IND
-  1: ADV_TYPE_DIRECT_IND_HIGH
-  2: ADV_TYPE_SCAN_IND
-  3: ADV_TYPE_NONCONN_IND
-  4: ADV_TYPE_DIRECT_IND_LOW

-  **<"own_addr_type">**：Bluetooth LE 地址类型
-  0: BLE_ADDR_TYPE_PUBLIC
-  1: BLE_ADDR_TYPE_RANDOM
-  **<"channel_map">**：广播信道
-  1：ADV_CHNL_37
-  2：ADV_CHNL_38
-  4：ADV_CHNL_39
-  7: ADV_CHNL_ALL
-  **<"adv_filter_policy">**：广播过滤器规则
-  0: ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY
-  1: ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY
-  2: ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST
-  3:
   ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST
-  **<"peer_addr_type">**：对方 Bluetooth LE 地址类型
-  0: PUBLIC
-  1: RANDOM
-  **<"peer_addr">**：对方 Bluetooth LE 地址

说明
^^^^

- 如果从未设置过 peer_addr, 那么查询出来的结果会是全零。
- 对于不可连接的广播类型，广播参数的最小间隔应大于等于 0xA0，也就是 160 乘以 0.625 = 100，单位毫秒。

示例
^^^^

::

   AT+BLEINIT=1   // 初始化 Bluetooth LE协议栈
   AT+BLEADDR=1,"c2:34:45:78:66:89"
   AT+BLEADVPARAM=50,50,0,1,4,0,1,"12:34:45:78:66:88"
   // 此时 Bluetooth LE 客户端扫描到的 wm-iot-dev 设备的 MAC 地址为 "c2:34:45:78:66:89"

.. _cmd-EADVDATA:   
 
:ref:`AT+BLEADVDATA <Bluetooth_AT>`：设置 Bluetooth LE 广播数据
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

  设置广播数据
 
**命令：**

::

   AT+BLEADVDATA=<adv_data>

**响应：**

::

   OK

参数
^^^^

-  **<"adv_data">**：广播数据，为 HEX 字符串。例如，若想设置广播数据为 “0x04 0x09 0x31 0x32 0x33” ，则命令为 AT+ BLEADVDATA ="0409313233"。


说明
^^^^

- 如果之前已经使用命令 AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power> 设置了广播数据，则会被本命令设置的广播数据覆盖。
- 如果您想使用本命令修改设备名称，则建议在执行完该命令之后执行 AT+BLENAME 命令将设备名称设置为同样的名称。
- 如果需要设置更长的广播数据，请调用 AT+BLESCANRSPDATA 指令来设置。
- 广播数据必须遵守 Bluetooth SIG 定义的 LTV 格式。
- 广播数据最长为31字节。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEADVDATA="0409313233"

.. _cmd-EADVDATAEX:   
 
:ref:`AT+BLEADVDATAEX <Bluetooth_AT>`：自动设置 Bluetooth LE 广播数据
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询广播数据的参数
  
**命令：**

::

   AT+BLEADVDATAEX?

**响应：**

::

   +BLEADVDATAEX:<dev_name>,<uuid>,<manufacturer_data>,<include_power>
   OK
   
设置命令
^^^^^^^^^^^^^^^^

**功能：**
 
::

   设置广播数据并开始广播
  
**命令：**

::

   AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power>

**响应：**

::

   OK

参数
^^^^

-  **<"dev_name">**：字符串参数，表示设备名称
-  **<"uuid">**：字符串参数。
-  **<"manufacturer_data">**：制造商数据，为 HEX 字符串。
-  **<"include_power">**：若广播数据需包含 TX 功率，本参数应该设为 1；否则为 0。

说明
^^^^

- 如果之前已经使用命令 AT+BLEADVDATA=<adv_data> 设置了广播数据，则会被本命令设置的广播数据覆盖。
- 此命令会自动将之前使用 AT+BLEADVPARAM 命令设置的广播类型更改为 0。
- 广播数据配置的总长度为 31 字节。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEADVDATAEX="wm-iot-dev","1802","0102030405",1

.. _cmd-EADVSTART:

:ref:`AT+BLEADVSTART <Bluetooth_AT>`：开始 Bluetooth LE 广播
-----------------------------------------------------------------------------

执行命令
^^^^^^^^

**功能：**
 
::

   开始广播
  
**命令：**

::

   AT+BLEADVSTART

**响应：**

::

    OK

说明
^^^^

- 若未使用命令 AT+BLEADVPARAM=<adv_parameter> 设置广播参数，则使用默认广播参数。
- 若未使用命令 AT+BLEADVDATA=<adv_data> 设置广播数据，则发送带有名称的数据包。
- 若之前已经使用命令 AT+BLEADVDATA=<adv_data> 设置过广播数据，则会被 AT+BLEADVDATAEX=<dev_name>,<uuid>,<manufacturer_data>,<include_power> 设置的广播数据覆盖，相反，如果先使用 AT+BLEADVDATAEX，则会被 AT+BLEADVDATA 设置的广播数据覆盖。
- 开启 Bluetooth LE 广播后，如果没有建立 Bluetooth LE 连接，那么将会一直保持广播；如果建立了连接，则会自动结束广播。如果连接断开后，会自动开始广播。
- 开启可连接广播后，如果主设备发起连接操作。此时设备自动选择一个空闲的 conn index 作为从连接的 conn index。从设备（外设）发送需要带 conn index 的 AT 指令时，使用此索引值。

示例
^^^^^^^^

::

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEADVSTART

.. _cmd-EADVSTOP:
    
:ref:`AT+BLEADVSTOP <Bluetooth_AT>`：停止 Bluetooth LE 广播
-----------------------------------------------------------------------------

执行命令
^^^^^^^^

**功能：**
 
::

   停止广播
     
**命令：**

::

   AT+BLEADVSTOP

**响应：**

::

    OK

说明
^^^^

- 若开始广播后，成功建立 Bluetooth LE 连接，则会自动结束 Bluetooth LE 广播，无需调用本命令。

示例
^^^^

::
    
   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEADVSTART
   AT+BLEADVSTOP

.. _cmd-ECONN:

:ref:`AT+BLECONN <Bluetooth_AT>`：建立 Bluetooth LE 连接
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询 Bluetooth LE 连接
  
**命令：**

::

   AT+BLECONN?

**响应：**

::

   +BLECONN:<conn_index>,<remote_address>,<role>
   OK

   若未建立连接，则响应不显示 <conn_index> 和 <remote_address> 参数。

设置命令
^^^^^^^^

**功能：**
 
::

   建立 Bluetooth LE 连接
  
**命令：**

::

   AT+BLECONN=<conn_index>,<remote_address>[,<addr_type>,<timeout>]

**响应：**

若建立连接成功，则提示：

::

   +BLECONN:<conn_index>,<remote_address>
   OK

若建立连接失败，则提示：

::

   +BLECONN:<conn_index>,-1
   ERROR
   
若是因为参数错误或者其它的一些原因导致连接失败，则提示：

::

   ERROR

参数
^^^^

-  **<"conn_index">**：Bluetooth LE 连接号，范围：[0,5]。
-  **<"remote_address">**：对方 Bluetooth LE 设备地址。
-  **<"addr_type">**：广播设备地址类型：
-  0: 公共地址 (Public Address)
-  1: 随机地址 (Random Address)
-  **<"timeout">**：连接超时时间，单位：秒。范围：[3,30]，默认值为3秒。
-  **<"role">**：连接角色，CENTRAL 或者 PERIPHERAL。

说明
^^^^

-  建议在建立新连接之前，先运行 AT+BLESCAN 命令扫描设备，确保目标设备处于广播状态。
-  最大连接超时为 30 秒。

注意
^^^^

-  对于外围设备使用随机地址，并且和该外围设备 BOND 成功。连接断开后，**不支持** 再次向该外围设备发起连接操作。


示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:0a:c4:09:34:23",0,10

.. _cmd-ECONNPARAM:

:ref:`AT+BLECONNPARAM <Bluetooth_AT>`：查询/更新 Bluetooth LE 连接参数
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询 Bluetooth LE 连接参数
  
**命令：**

::

   AT+BLECONNPARAM?

**响应：**

::

   +BLECONNPARAM:<conn_index>,<min_interval>,<max_interval>,<cur_interval>,<latency>,<timeout>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

   更新 Bluetooth LE 连接参数
    
**命令：**

::

   AT+BLECONNPARAM=<conn_index>,<min_interval>,<max_interval>,<latency>,<timeout>

**响应：**

::

   OK

若设置失败，则提示以下信息：

::

   +BLECONNPARAM: <conn_index>,-1

参数
^^^^

-  **<"conn_index">**：Bluetooth LE 连接号，范围：[0,5]。
-  **<"min_interval">**：最小连接间隔。本参数值应小于等于 <max_interval> 参数值。参数范围： [0x0006,0x0C80]。连接间隔等于该参数乘以 1.25 毫秒，所以实际的最小连接间隔范围为 [7.5,4000]毫秒。
-  **<"max_interval">**：最大连接间隔。本参数值应大于等于 <min_interval> 参数值。参数范围： [0x0006,0x0C80]。连接间隔等于该参数乘以 1.25 毫秒，所以实际的最大连接间隔范围为 [7.5,4000]毫秒。
-  **<"cur_interval">**：当前连接间隔。
-  **<"latency">**：延迟。参数范围：[0x0000,0x01F3]。
-  **<"timeout">**：超时。参数范围：[0x000A,0x0C80]。超时等于该参数乘以 10 毫秒，所以实际的超时范围为 [100,32000] 毫秒。

注意
^^^^

-  对于**<"timeout">**,**<"latency">**和**<"max_interval">**还需要满足如下条件：
	    （timeout x 4）> (1+latency) x max_interval


说明
^^^^

- 本命令要求先建立连接，client 或者 server 角色都支持更新连接参数。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLECONNPARAM=0,12,14,1,500

.. _cmd-EDISCONN:
 
:ref:`AT+BLEDISCONN <Bluetooth_AT>`：断开 Bluetooth LE 连接
-----------------------------------------------------------------------------

执行命令
^^^^^^^^

**功能：**
 
::

   断开 Bluetooth LE 连接
  
**命令：**

::

   AT+BLEDISCONN=<conn_index>

**响应：**

::
  
   OK  // 收到 AT+BLEDISCONN 命令
   +BLEDISCONN:<conn_index>,<remote_address>  // 运行命令成功

参数
^^^^

-  **<"conn_index">**：Bluetooth LE 连接号，范围：[0,5]。
-  **<"remote_address">**：对方 Bluetooth LE 设备地址。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLEDISCONN=0

.. _cmd-EDATALEN:

:ref:`AT+BLEDATALEN <Bluetooth_AT>`：设置 Bluetooth LE 数据包长度
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE 数据包长度
  
**命令：**

::

  AT+BLEDATALEN=<conn_index>,<pkt_data_len>

**响应：**

::
 
   OK

参数
^^^^

-  **<"conn_index">**：Bluetooth LE 连接号，范围：[0,5]。
-  **<"pkt_data_len">**：数据包长度，范围：[0x001B,0x00FB]。

说明
^^^^

- 需要先建立 Bluetooth LE 连接，才能设置数据包长度。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLEDATALEN=0,30

.. _cmd-ECFGMTU:

:ref:`AT+BLECFGMTU <Bluetooth_AT>`：设置 Bluetooth LE MTU 长度
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询 MTU（maximum transmission unit，最大传输单元）长度
  
**命令：**

::

  AT+BLECFGMTU?

**响应：**

::

   +BLECFGMTU:<conn_index>,<mtu_size>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

   设置 MTU 的长度
  
**命令：**

::

   AT+BLECFGMTU=<conn_index>,<mtu_size>

**响应：**

::

   OK  // 收到本命令

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<mtu_size>**：MTU 长度，单位：字节，范围：[23,517]。

说明
^^^^

.. parsed-literal::

   - 本命令要求先建立 Bluetooth LE 连接。
   - 仅支持客户端运行本命令设置 MTU 的长度。
   - 设备连接后，固件自动尝试使用最大值517和对端设备协商 MTU 值。
   - MTU 的实际长度需要协商，响应 ``OK`` 只表示尝试协商 MTU 长度，因此设置长度不一定生效，建议调用 :ref:`AT+BLECFGMTU? <cmd-ECFGMTU>` 查询实际 MTU 长度。

示例
^^^^

.. code-block:: text

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:0a:c4:09:34:23"
   AT+BLECFGMTU=0,244

.. _cmd-EGATTSSVC:
 
:ref:`AT+BLEGATTSSVC <Bluetooth_AT>`：GATTS 创建/查询本地服务
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   GATTS (Generic Attributes Server) 查询本地服务列表
  
**命令：**

::

   AT+BLEGATTSSVC?

**响应：**

::

   +BLESVCLIST(local):
   <description>,<uuid>,<handle>,<prop>,<perm>
   ......
   <description>,<uuid>,<handle>,<prop>,<perm>
   OK

参数
^^^^

-  **<description>**：描述，可能的内容为 service， characteristic 或者 descriptor。
-  **<uuid>**：uuid, 为 Bluetooth SIG 定义的标准 uuid 或者用户自定义的 uuid，16进制显示。
-  **<handle>**：句柄值，十进制显示。
-  **<prop>**：对应 characteristic 的 property，具体property的bit定义为：

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Bit 7
     - Bit 6
     - Bit 5
     - Bit 4
     - Bit 3
     - Bit 2
     - Bit 1
     - Bit 0

   * - 扩展
     - 认证
     - Indicate
     - Notify
     - 带应答写
     - 不带应答写
     - 读
     - 广播

-  **<perm>**：对应 **<description>** 的 permission，具体 permission 的bit定义为：

.. list-table:: 
   :widths: 25 25 25 25 25 25 25 25 25
   :header-rows: 0
   :align: center

   * - Bit 8
     - Bit 7
     - Bit 6
     - Bit 5
     - Bit 4
     - Bit 3
     - Bit 2
     - Bit 1
     - Bit 0

   * - Le Security 写
     - Le Security 读
     - 预备写
     - 认证写
     - 认证读
     - 加密写
     - 加密读
     - 写
     - 读
	 
注意
^^^^

-   当 **<prop>** 或者 **<perm>** 显示为 **----** 时，表示该项无效。特别说明的是 characteristic 的权限值被映射到随后 descriptor 的权限值。 

设置命令
^^^^^^^^

**功能：**
 
::

   GATTS (Generic Attributes Server) 创建本地服务列表
  
**命令：**

::

   AT+BLEGATTSSVC=<prim_service_uuid>,<character_uuid>,<property>,<permission>
					   [,<character_uuid>,<property>,<permission>]
					   […]
					   [,<character_uuid>,<property>,<permission>]


**响应：**

::

   OK

参数
^^^^

-   **<prim_service_uuid>**：创建 Primary servcie 的 uuid，16进制格式输入。
-   **<character_uuid>**：characteristic 的 uuid，16进制格式输入。
-   **<property>**：characteristic 的属性值，16进制格式输入，具体 :ref:`property <cmd-EGATTSSVC>` 的 bit 定义参见上述表格。
-   **<permission>**：characteristic 的权限值，16进制格式输入。具体 :ref:`permission <cmd-EGATTSSVC>` 的 bit 定义参见上述表格。


注意
^^^^

-   当某个 characteristic 的 property 配置为不带应答的写操作时，permission 不可配置为加密写，授权写或者 Le security 写（除非此连接已经为加密模式）。  

说明
^^^^

-   指令仅支持创建基本服务列表,最大服务列表可以动态创建12个
-   指令允许创建 characteristic 列表，单个服务支持最大 characteristic 数目为6
-   指令创建时，如果characteristic的property为indicate或者notify，自动追加uuid为2902的描述符，否则追加 uuid 为2901的描述符
-   创建的服务列表实时生效，如果创建时，已经和对方建立连接，对方设备通过刷新服务列表即可获取该服务列表；
-   characteristic 的 property 和 perm的配置对应常见规则描述:
      1)	characteristic 的 property 为读，则对应的 permission 必须设置任意一项读的权限;
      2)	characteristic 的 property 为写，则对应的 permission 必须设置任意一项写的权限;
      3)	characteristic 的 property 为 indicate，则对应的 permission 必须读和写权限;
      4)	permission 权限设置为 Le security 读和写时，需同时配置授权的读和写或者加密的读和写。


示例
^^^^

::

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEGATTSSVC=FFF0,FFF1,02,01 //创建 FFF1 characteristic 属性只读，权限读
   AT+BLEGATTSSVC=FFF0,FFF1,06,03 //创建FFF1 characteristic, 属性为读和不带应答的写，权限为读和写
   AT+BLEGATTSSVC=FFF1,FFF2,10,03 //创建FFF4 characteristic, 属性为 Notify，权限为读和写
   AT+BLEGATTSSVC=FFF3,FFF4,10,0C //创建FFF4 characteristic, 属性为 Notify，权限为加密读和加密写
   AT+BLEGATTSSVC=FFF5,FFF6,02,01,FFF7,10,18C //创建FFF4 characteristic, 属性为 Notify，权限为加密读和加密写，并且为加密等级为 Le security

.. _cmd-EGATTSSVCRMV:
 
:ref:`AT+BLEGATTSSVCRMV <Bluetooth_AT>`：GATTS 删除本地服务服务
-----------------------------------------------------------------------------

执行命令
^^^^^^^^

**功能：**
 
::

   GATTS 删除本地动态创建服务
  
**命令：**

::

   AT+BLEGATTSSVCRMV=<uuid>

**响应：**

::

   OK

参数
^^^^

-   **<uuid>**：创建服务时的 primary service uuid。

说明
^^^^

-   服务删除后实时生效，如果已创建连接，主设备可以通过刷新服务列表查看。

示例
^^^^

::

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEGATTSSVC=FFF0,FFF1,10,01 //动态创建FFF0的服务列表
   AT+BLEGATTSSVCRMV=FFF0 //删除动态创建的FFF0服务列表

.. _cmd-EGATTSNTFY:

:ref:`AT+BLEGATTSNTFY <Bluetooth_AT>`：服务器 notify 服务特征值给客户端
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   服务器 notify 服务特征值给客户端
  
**命令：**

::

   AT+BLEGATTSNTFY=<conn_index>,<handle>,<length>

**响应：**

::

   >
  
符号 > 表示 AT 准备好接收串口数据，此时您可以输入数据，当数据长度达到参数 <length> 的值时，执行 notify 操作。

若数据传输成功，则提示：

::

   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<handle>**：服务特征的句柄值，可运行AT+BLEGATTSSVC? 查询。
-  **<length>**：数据长度，最大长度：（ :ref:`MTU <cmd-ECFGMTU>` - 3）。

示例
^^^^

:: 

   AT+BLEINIT=1      //初始化 Bluetooth LE协议栈
   AT+BLEGATTSSVC=FFF0,FFF1,10,03 //创建支持notify的characteristic
   AT+BLEADVSTART    //开始广播，当 client 连接后，必须配置 :ref:`订阅 <cmd-EGATTCSUBS>` 
   AT+BLEGATTSSVC?   //查询发送 notify 的句柄
   // 例如 ，使用句柄值21 通过notify发送长度为4字节的数据，使用如下命令 ： AT+BLEGATTSNTFY=0,21,4提 示 ">" 符号后，输入4字节的数据，如 "1234"， 然后数据自动传输


.. _cmd-EGATTSIND:

:ref:`AT+BLEGATTSIND <Bluetooth_AT>`：服务器 indicate 服务特征值给客户端
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   服务器 indicate 服务特征值给客户端
  
**命令：**

::

   AT+BLEGATTSIND=<conn_index>,<handle>,<length>

**响应：**

::

   >

符号 > 表示 AT 准备好接收串口数据，此时您可以输入数据，当数据长度达到参数 <length> 的值时，执行 indicate 操作。

若数据传输成功，则提示：

::

   OK
   
参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<handle>**：服务特征的句柄值，可运行 AT+BLEGATTSSVC? 查询。
-  **<length>**：数据长度，最大长度：（ :ref:`MTU <cmd-ECFGMTU>` - 3）。

示例
^^^^

:: 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEGATTSSRV=FFF0,FFF1,20,03 //创建支持 indicate 的 characteristic，uuid 为 FFF1
   AT+BLEADVSTART // 开始广播，当 client 连接后，必须配置接收indication
   AT+BLEGATTSSVC? // 查询客户端可以接收 indication 的特征句柄值
   // 例如，使用 handle 值3 发送4字节的数据，命令如下 ：
   AT+BLEGATTSIND=0,3,4
   // 提 示 ">" 符号后，输入4字节的数据，如 "1234"。然后数据自动传输


.. _cmd-EGATTSSETATTR:

:ref:`AT+BLEGATTSSETATTR <Bluetooth_AT>`：GATTS 设置服务特征值
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   GATTS 设置服务特征值或描述符值
    
**命令：**

::

   AT+BLEGATTSSETATTR=<handle>,<length>
    
**响应：**

::

    >

符号 > 表示 AT 准备好接收串口数据，此时您可以输入数据，当数据长度达到参数 <length> 的值时，执行设置操作。

若数据传输成功，则提示：

::

   OK

参数
^^^^

-  **<handle>**：handle值，可运行 AT+BLEGATTSSVC? 查询。
-  **<length>**：数据长度，范围[1,512]。

说明
^^^^

-  如果 <length> 参数值大于支持的最大长度，则设置会失败。

示例
^^^^

:: 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLEGATTSSVC=FFF0,FFF1,06,03 //创建支持读写的characteristic
   AT+BLEGATTSSVC?                //查询handle值
   //例如，向handle值5写入长度为3字节的数据，命令如下 ：
   AT+BLEGATTSSETATTR=5,3
   //提示 ">" 符号后，输入3字节的数据即可，例如"123"，然后设置开始


.. _cmd-EGATTCSVC:

:ref:`AT+BLEGATTCSVC <Bluetooth_AT>`：GATTC 发现服务列表
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   GATTC (Generic Attributes Client) 发现服务器端服务列表
    
**命令：**

::

   AT+BLEGATTCSVC=<conn_index>
    
**响应：**

::

   +BLESVCLIST(remote):<conn_index>,
    <attribute description>,<uuid>,<handle>,<prop>
    ……
    <attribute description>,<uuid>,<handle>,<prop>
   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<attribute description>**：attribute描述，可能的内容有 primary service，secondary service, characteristic, descriptor。 
-  **<uuid>**：attribute的 UUID，16进制显示。
-  **<handle>**：attribute的句柄值，10进制显示。
-  **<prop>**：attribute的 :ref:`property <cmd-EGATTSSVC>` 值，16进制显示。

说明
^^^^

- 使用本命令，需要先建立 Bluetooth LE 连接。
- 本指令会依次查询服务端的 service，characteristic 和 descriptor 信息，如果服务端提供的服务较多时，耗时较长。

示例
^^^^

.. code-block:: text 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0


.. _cmd-EGATTCRD:

:ref:`AT+BLEGATTCRD <Bluetooth_AT>`：GATTC 读取服务特征值
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   GATTC 读取服务特征值或描述符值
    
**命令：**

::

   AT+BLEGATTCRD=<conn_index>,<handle>
    
**响应：**

::

   +BLEGATTCRD:<conn_index>,<len>,<value>
   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<handle>**：服务特征句柄值，可运行 AT+BLEGATTCSVC=<conn_index> 查询。
-  **<len>**：数据长度。
-  **<value>**：数据内容。例如，若响应为 +BLEGATTCRD:0,25,2,0123，则表示数据长度为 2，内容为 0x01 0x23。

说明
^^^^

- 使用本命令，需要先建立 Bluetooth LE 连接。
- 若目标服务特征不支持读操作，则返回 “ERROR”。

示例
^^^^

:: 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCRD=0,25 // 根据前一条命令的查询结果，指定句柄查询

.. _cmd-EGATTCWR:

:ref:`AT+BLEGATTCWR <Bluetooth_AT>`：GATTC 写服务特征值
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   GATTC 写服务特征值或描述符值
  
**命令：**

::

   AT+BLEGATTCWR=<conn_index>,<handle>,<length>
   
**Response：**

::

  >  

符号 > 表示 AT 准备好接收串口数据，此时您可以输入数据，当数据长度达到参数 <length> 的值时，执行写入操作。

若数据传输成功，则提示：

::

   OK
    
参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<handle>**：服务句柄，可运行AT+BLEGATTCSVC=<conn_index> 查询。
-  **<char_index>**：服务特征序号，可运行 AT+BLEGATTCCHAR=<conn_index>,<srv_index> 查询。
-  **<length>**：数据长度。最大长度：（ :ref:`MTU <cmd-ECFGMTU>` - 3）。

说明
^^^^

- 使用本命令，需要先建立 Bluetooth LE 连接。
- 若目标服务特征不支持写操作，则返回 “ERROR”。

示例
^^^^

:: 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCWR=0,25,6
   // 提示 ">" 符号后，输入6字节的数据即可，如"123456"。然后开始写入

.. _cmd-EGATTCSUBS:

:ref:`AT+BLEGATTCSUBS <Bluetooth_AT>`：GATTC 订阅
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   GATTC 订阅或取消服务端的 Indicate 或 Notification 功能
  
**命令：**

::

   AT+BLEGATTCSUBS=<conn_index>,<handle>,<value>
   
**Response：**

::

  OK 
    
参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<handle>**：服务句柄，可运行AT+BLEGATTCSVC=<conn_index> 查询。
-  **<value>**：0 取消；1订阅。

说明
^^^^

- 使用本命令，需要先建立 Bluetooth LE 连接。
- 若目标服务特征不支持 indication 或者 notification 操作，则返回 “ERROR”。
- 订阅成功后，接收并显示对方发送的数据格式为：

::

  +BLEGATTCNTFY:0,25,4,12232123 //index 为 0 的连接，在 handle 为25的句柄收到对方发送的4字节数据，数据内容为16进制格式显示 12232123，即 0x12,0x23,0x21,0x23

示例
^^^^

.. code-block:: text 

   AT+BLEINIT=1   //初始化 Bluetooth LE协议栈
   AT+BLECONN=0,"24:12:5f:9d:91:98"
   AT+BLEGATTCSVC=0
   AT+BLEGATTCSUBS=0,25,1

.. _cmd-ESECPARAM:

:ref:`AT+BLESECPARAM <Bluetooth_AT>`：查询/设置 Bluetooth LE 加密参数
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询 Bluetooth LE SMP 加密参数
  
**命令：**

::

   AT+BLESECPARAM?
    
**响应：**

::

   +BLESECPARAM:<auth_req>,<iocap>,<enc_key_size>,<init_key>,<rsp_key>,<auth_option>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE SMP 加密参数
  
**命令：**

::

   AT+BLESECPARAM=<auth_req>,<iocap>,<enc_key_size>,<init_key>,<rsp_key>[,<auth_option>]
    
**响应：**

::

 OK

参数
^^^^

-  **<auth_req>**：认证请求。
-  0: NO_BOND
-  1: BOND
-  4: MITM
-  8: SC_ONLY
-  9: SC_BOND
-  12: SC_MITM
-  13: SC_MITM_BOND
-  **<iocap>**：输入输出能力。
-  0: DisplayOnly
-  1: DisplayYesNo
-  2: KeyboardOnly
-  3: NoInputNoOutput
-  4: Keyboard display
-  **<enc_key_size>**：加密密钥长度。参数范围：[7,16]。单位：字节。
-  **<init_key>**：多个比特位组成的初始密钥。
-  **<rsp_key>**：多个比特位组成的响应密钥。
-  **<auth_option>**：安全认证选项：
-  0: 自动选择安全等级；
-  1: 如果无法满足之前设定的安全等级，则会断开连接。

说明
^^^^

-  <init_key> 和 <rsp_key> 参数的比特位组合模式如下：
-  Bit0: 用于交换初始密钥和响应密钥的加密密钥；
-  Bit1: 用于交换初始密钥和响应密钥的 IRK 密钥；
-  Bit2: 用于交换初始密钥和响应密钥的 CSRK 密钥；
-  Bit3: 用于交换初始密钥和响应密钥的 link 密钥（仅用于 Bluetooth LE 和 BR/EDR 共存模式）。

示例
^^^^

:: 

   AT+BLESECPARAM=1,4,16,3,3,0

.. _cmd-EENC:

:ref:`AT+BLEENC <Bluetooth_AT>`：发起 Bluetooth LE 加密请求
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

  发起配对请求
  
**命令：**

::

  AT+BLEENC=<conn_index>,<sec_act>
    
**响应：**

::

  OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<sec_act>**：
-  0: SEC_NONE；
-  1: SEC_ENCRYPT_NO_MITM；
-  2: SEC_ENCRYPT_MITM；
-  3: SEC_ENCRYPT_SC。
 
说明
^^^^

-  使用本命令前，请先设置安全参数、建立与对方设备的连接。

示例
^^^^

:: 

   AT+RESTORE
   AT+BLEINIT=1
   AT+BLESECPARAM=1,0,16,3,3
   AT+BLESETKEY=123456
   AT+BLEADVSTART
   // 使用 Bluetooth LE 调试 app 作为 client 与 wm-iot-dev 设备建立 Bluetooth LE 连接
   AT+BLEENC=0,3

.. _cmd-EENCRSP:

:ref:`AT+BLEENCRSP <Bluetooth_AT>`：回复对端设备发起的配对请求
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   回复对端设备发起的配对请求
  
**命令：**

::

   AT+BLEENCRSP=<conn_index>,<accept>
    
**响应：**

::

   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<accept>**：
-  0: 拒绝；
-  1: 接受。

说明
^^^^

-  使用本命令后，AT 会在配对请求流程结束后输出配对结果。

::

   +BLEAUTHCMPL:<conn_index>,<enc_result>
   
-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<enc_result>**：
-  0: 加密配对成功；
-  1: 加密配对失败。

示例
^^^^

:: 

   AT+BLEENCRSP=0,1

.. _cmd-EKEYREPLY:

:ref:`AT+BLEKEYREPLY <Bluetooth_AT>`：给对方设备回复密钥
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   回复配对密钥
  
**命令：**

::

   AT+BLEKEYREPLY=<conn_index>,<key>
    
**响应：**

::

   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<key>**：配对密钥。

示例
^^^^

:: 

   AT+BLEKEYREPLY=0,649784

.. _cmd-ECONFREPLY:

:ref:`AT+BLECONFREPLY <Bluetooth_AT>`:给对方设备回复确认结果（传统连接阶段）
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   回复配对结果
  
**命令：**

::

   AT+BLECONFREPLY=<conn_index>,<confirm>
    
**响应：**

::

   OK

参数
^^^^

-  **<conn_index>**：Bluetooth LE 连接号，范围：[0,5]。
-  **<confirm>**：
-  0: 否
-  1: 是

示例
^^^^

:: 

   AT+BLECONFREPLY=0,1

.. _cmd-EENCDEV:

:ref:`AT+BLEENCDEV <Bluetooth_AT>`:查询绑定的 Bluetooth LE 加密设备列表
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询绑定的 Bluetooth LE 加密设备列表
  
**命令：**

::

   AT+BLEENCDEV?
    
**响应：**

::

   +BLEENCDEV:<enc_dev_index>,<mac_address>
   OK

参数
^^^^

-  **<enc_dev_index>**：已绑定设备的连接号。该参数不一定等于命令 AT+BLECONN 查询出的 Bluetooth LE 连接列表中的 conn_index 参数。范围：[0,7]。
-  **<mac_address>**：MAC 地址。

说明
^^^^

-  wm-iot-dev 最多允许绑定 8 个设备。如果绑定的设备数量超过 8 个，那么新绑定的设备信息会根据绑定顺序从 0 到 7 号依次覆盖之前的设备信息。

示例
^^^^

::

   AT+BLEENCDEV?

.. _cmd-EENCCLEAR:

:ref:`AT+BLEENCCLEAR <Bluetooth_AT>`:清除 Bluetooth LE 加密设备列表
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   从安全数据库列表中删除某一连接号的设备
  
**命令：**

::

   AT+BLEENCCLEAR=<enc_dev_index>
    
**响应：**

::

  OK

执行命令
^^^^^^^^

**功能：**
 
::

   删除安全数据库所有设备
  
**命令：**

::

   AT+BLEENCCLEAR

**响应：**

::

   OK

参数
^^^^

-  **<enc_dev_index>**：已绑定设备的连接号。

示例
^^^^

::

   AT+BLEENCCLEAR

.. _cmd-ESETKEY:

:ref:`AT+BLESETKEY <Bluetooth_AT>`:设置 Bluetooth LE 静态配对密钥
-----------------------------------------------------------------------------

查询命令
^^^^^^^^

**功能：**
 
::

   查询 Bluetooth LE 静态配对密钥，若未设置，则 AT 返回 -1
  
**命令：**

::

   AT+BLESETKEY?
    
**响应：**

::

   +BLESETKEY:<static_key>
   OK

设置命令
^^^^^^^^

**功能：**
 
::

  为所有 Bluetooth LE 连接设置一个 Bluetooth LE 静态配对密钥
  
**命令：**

::

   AT+BLESETKEY=<static_key>
    
**响应：**

::

   OK

参数
^^^^

-  **<static_key>**：Bluetooth LE 静态配对密钥。

示例
^^^^

::

   AT+BLESETKEY=123456

.. _cmd-ESPPSCFG:   
 
:ref:`AT+BLESPPSCFG <Bluetooth_AT>`：配置 Bluetooth LE SPP 服务器参数
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE SPP传输功能服务器端的参数信息
 
**命令：**

::

   AT+BLESPPSCFG=<svc_uuid>,<tx_uuid>,<tx_prop>,<tx_perm>,<rx_uuid>,<rx_prop>,<rx_perm>


**响应：**

::

   OK

参数
^^^^

-  **<"svc_uuid">**：透传基本服务 uuid。
-  **<"tx_uuid">**：透传服务发送数据 characteristic 的 uuid。
-  **<"tx_prop">**：透传服务发送数据 characteristic 的属性，十六进制格式读取。
-  **<"tx_perm">**：透传服务发送数据 characteristic 的权限，十六进制格式读取。
-  **<"rx_uuid">**：透传服务接收数据 characteristic 的 uuid。
-  **<"rx_prop">**：透传服务接收数据 characteristic 的属性，十六进制格式读取。
-  **<"rx_perm">**：透传服务接收数据 characteristic 的权限，十六进制格式读取。

说明
^^^^

-  透传服务发送数据采用 notify 或者 indicate 方式，所以 tx_prop 必须包含配置为 10 或者 20，即0x10，notify 方式或者 0x20,indicate 方式。
-  透传服务接收数据，也就是客户端向服务端发送数据，采用应答写或者不带应答的写，所以 rx_prop 必须配置为04或者08，即0x04，不带应答写方式或者0x08,带应答的写方式。
-  指令执行后，会开启广播功能，广播数据字段包含：设备名称和服务 UUID 字段 svc_uuid，客户端可根据该字段进行扫描并发起连接操作。
-  客户端连接SPP服务端后，订阅发送特征值的 indicate 或者 notify 功能后，控制台界面打印 +BLESPP READY 等待用户开启透传。


示例
^^^^

::

   AT+BLEINIT=1    // 初始化 Bluetooth LE协议栈
   AT+BLESPPSCFG=FFF0,FFF1,10,3,FFF2,4,2 //配置SPP服务端，服务UUID为FFF0,使用FFF1特征值发送数据，发送方式为notify。使用FFF2特征值接收数据，对方发送数据为不带应答写
   
.. _cmd-ESPPCCFG:   
 
:ref:`AT+BLESPPCCFG <Bluetooth_AT>`：配置 Bluetooth LE SPP 客户端参数
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   设置 Bluetooth LE SPP传输功能客户端的参数信息
 
**命令：**

::

   AT+BLESPPCCFG=<filter_type>,<filter_value>,<svc_uuid>,<tx_uuid>,<rx_uuid>


**响应：**

::

   OK

参数
^^^^

-  **<"filter_type">**：范围[0,2],指定扫描过滤类型
-   1) "NAME"
-   2) "MAC"
-   3) "UUID"
-  **<"filter_value">**：对应 filter_type 的内容。
-  **<"svc_uuid">**：服务端开启透传服务 uuid。
-  **<"tx_uuid">**：服务端开启透传服务发送数据的特征值 UUID。
-  **<"rx_uuid">**：服务端开启透传服务接收数据的特征值 UUID。

说明
^^^^

-  本指令执行后，开启扫描功能。当扫描到的设备满足过滤条件时，发起连接操作。连接成功后，查询对方是否提供了 svc_uuid 的基本透传服务，该透传服务是否提供了 tx_uuid 的特制值发送功能和提供了 rx_uuid 的特制值接收数据功能。如果上述条件均满足，客户端会订阅发送tx_uuid特征值的notify或者indicate功能。订阅成功后，控制台打印+BLESPP READY等待用户开始透传。


示例
^^^^

::

   AT+BLEINIT=1    // 初始化 Bluetooth LE协议栈
   AT+BLESPPCCFG=0,”WM-AT”,FFF0,FFF2,FFF1 //配置SPP客户端参数，过滤类型为名称，名称为”WM-AT”，服务端SPP服务UUID为FFF0,FFF2客户端发送数据UUID,对应服务端接收数据UUID，接收数据特制值为FFF1，对应服务端发送数据UUID
   
.. _cmd-ESPPSTART:   
 
:ref:`AT+BLESPPSTART <Bluetooth_AT>`：开启 Bluetooth LE SPP 透传
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   启动 Bluetooth LE SPP传输功能
 
**命令：**

::

   AT+BLESPPSTART


**响应：**

::

   >


说明
^^^^

-   对于 SPP 客户端或者服务端，当系统提示 +BLESPP READY 后，使用此命令开启透传功能，否则开启失败，返回 ERROR
-   在透传模式下，当系统收到只含有 +++ 的包时，设备返回到普通命令模式，控制台打印 +BLESPP EXIT MANUAL. 请至少等待一秒再发送下一个AT命令

注意
^^^^

-   如果透传服务端配置了加密的操作，请在开启透传功能之前必须使用 AT+BLEENC 指令完成配对操作（在进入透传模式后，控制台不在响应 AT 指令，导致在配对过程中无法输入配对密码和配对确认消息）

示例
^^^^

::

   AT+BLESPPSTART
   
.. _cmd-ESPPSTOP:   
 
:ref:`AT+BLESPPSTOP <Bluetooth_AT>`：关闭 Bluetooth LE SPP 透传
-----------------------------------------------------------------------------

设置命令
^^^^^^^^

**功能：**
 
::

   关闭 Bluetooth LE SPP传输功能
 
**命令：**

::

   AT+BLESPPSTOP


**响应：**

::

   OK


说明
^^^^

-   注意，由于在透传期间，系统不响应AT指令。用户使能该指令之前，必须退出透传模式。
-   指令执行后，如果已经和客户端建立连接，会断开对方的连接；如果未建立，对于客户端会关闭扫描功能，对于服务器端会关闭广播功能。

注意
^^^^

-   关闭透传指令，对于服务器端不会注销动态创建的SPP服务。如果想注销 SPP 服务，可以使用 :ref:`删除本地服务 <cmd-EGATTSSVCRMV>` 指令删除

示例
^^^^

::

   AT+BLESPPSTOP
   
.. _cmd-EWIFIPROV:   
 
:ref:`AT+BLEWIFIPROV <Bluetooth_AT>`：配置 WiFi 加网功能
-----------------------------------------------------------------------------
查询命令
^^^^^^^^

**功能：**
 
::

   查询通过 Bluetooth LE 辅助 WiFi 功能是否启用
 
**命令：**

::

   AT+BLEWIFIPROV?


**响应：**

::

   +BLEWIFIPROV=<enable>

参数
^^^^

-  **<"enable">**：开启或者关闭 WiFi 加网功能


设置命令
^^^^^^^^

**功能：**
 
::

   开启/关闭 通过 Bluetooth LE 辅助 WiFi 连接路由器或者软 AP 功能
 
**命令：**

::

   AT+BLEWIFIPROV=<enalbe>


**响应：**

::

   OK

参数
^^^^

-  **<"enable">**：开启或者关闭 WiFi 配网功能
-   1) 启用 WiFi 配网功能
-   2) 停止 WiFi 配网功能 

示例
^^^^

::

   AT+BLEWIFIPROV=1

.. _cmd-EBTTEST:   
 
:ref:`AT+BTTEST <Bluetooth_AT>`：使能/退出蓝牙测试模式
-----------------------------------------------------------------------------
退出命令
^^^^^^^^

**功能：**
 
::

   退出蓝牙测试模式
 
**命令：**

::

   AT+BTTEST=0


**响应：**

::

   OK



使能蓝牙测试模式
^^^^^^^^^^^^^^^^^

**功能：**
 
::

   设置蓝牙测试模式。设置成功后，对应的串口将响应 HCI 指令
 
**命令：**

::

   AT+BTTEST=<1>,<uart_idx>


**响应：**

::

   OK

参数
^^^^

-  **<"uart_idx">**：使用哪个串口响应 HCI 指令
-   1) 串口索引值范围 1~4 ，对应串口 1 到 串口 4
-   2) 仅支持默认串口配置，波特率：115200，无校验，1 位停止位，8 位数据位 

示例
^^^^

::

   AT+BTTEST=1,1

