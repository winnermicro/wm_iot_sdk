.. _bluetooth-wifiprov-proto:

:orphan:

BLE 辅助 Wi-Fi 入网协议
#########################

概述
============

Wi-Fi Prov Over BLE 是一款基于低功耗蓝牙（BLE）通道，实现 Wi-Fi 网络配置功能的协议，适用于 W80X 芯片。它通过 BLE 将 Wi-Fi 配置和证书等信息传输给 W80X，使 W80X 可以根据这些信息连接到 AP 。
该协议支持手机和设备间的秘钥协商，配置信息可采用加密方式传输，是一款安全的配置协议。同时该协议支持数据分片、校验和重传，可提高配置效率和保证信息完整性。


配网流程
============

Wi-Fi Prov Over BLE 配置过程主要包括广播、连接、服务发现、秘钥协商、数据传输和 indication 结果等步骤。

.. figure:: ../../../_static/component-guides/bluetooth/wifiprov_flow.svg
    :align: center
    :alt: 配网流程

1.	W80X开启 GATT Server 功能，发送包含特定信息的广播包。
2.	手机 APP 扫描 BLE 设备，通过过滤广播包中的特定信息，发现 W80X 设备。
3.	手机 APP 作为 GATT Client 和作为 GATT Server 的 W80X 建立连接。
4.	GATT 连接建立成功后，手机 APP 和 W80X 进入到秘钥协商过程，如果配置过程采用明文传输，则可以直接跳过秘钥协商过程，跳转到6执行。首先，由手机 APP 发送非对称算法 RSA 公钥给 W80X。
5.	W80X 收到 RSA 公钥后，首先随机产生得到一个 16Bytes 作为对称算法 AES-128 的 Key，然后用 RSA 公钥对此 Key 进行加密，并将加密的结果返回给手机 APP。
6.	如果需要加密传输，则手机 APP 在收到 W80X 返回的秘钥后，首先用 RSA 私钥解密得到 Key，然后用 AES-128 加密配置信息的 PAYLOAD 部分，发送给 W80X；如果采用明文传输，则手机 APP 直接将配置信息的明文作为 PAYLOAD，发送给 W80X。
7.	W80X 收到配置信息后，将最终结果通过 indication 的方式返回给手机 APP。

帧定义
============

.. list-table:: 
   :widths: 25 25 25 25 25 25
   :header-rows: 0
   :align: left

   * - cmd
     - seq-no
     - flag
     - frag-no
     - payload
     - crc

- **<cmd>**：operation code，占用1字节。
- **<seq-no>**：序列号，1字节，从0开始。帧发送时，序列号自动加 1，用来防止重放攻击 (Replay Attack)。每次重新连接后，序列清零，位溢出后重新从0开始。
- **<flag>**： 具体含义如下表：

.. list-table:: 
   :widths: 25 125
   :header-rows: 0
   :align: left

   * - BIT
     - 描述
	 
   * - [7]
     - 是否加密。1 表示加密，0 表示未加密。
	 
   * - [6]
     - 是否需要回复ack。1 表示需要，0 表示不需要。
	 
   * - [5]
     - 是否还存在下一个帧分片。	

   * - [4:0]
     - 保留。	

	 
- **<frag-no>**：帧分片序号，1字节， 从0开始。如果应用层重传，Sequence值加1，NO不变。
- **<payload>**：消息内容，采用TLV封装。
- **<crc>**：payload内容值CRC校验值，1字节。CRC =CRC8（cmd + seq-no + flag + frag-no + payload）。

指令定义
============

APP配置 STATION 入网参数指令
----------------------------

该指令用来配置W80X连接AP的参数，W80X在收到配置的全部数据后，启动联网操作。
  
- **<cmd>**：配置 station 指令 0x0A。
- **<payload>**：payload 内容定义：
 
  .. list-table:: 
    :widths: 25 25 25
    :header-rows: 0
    :align: left

    * - type-id
      - content
      - description
     
    * - 0x01
      - ssid
      - 路由器或者软 AP 的 SSID 信息
     
    * - 0x02
      - password
      - 路由器或者软 AP 的密码
	 
    * - 0x03
      - bssid	
      - 路由器或者软 AP 的 BSSID 信息	

W80X 上报 STATION 入网结果信息
-------------------------------

该指令用来把 STATION 入网结果上报给 APP。
  
- **<cmd>**：配置 station 指令 0x8A。
- **<payload>**：payload 内容定义：
 
  .. list-table:: 
    :widths: 25 25 25
    :header-rows: 0
    :align: left

    * - type-id
      - content
      - description
     
    * - 0x81
      - error-info
      - 0x00 入网成功，更多错误码定义，请参考‘wm_wifi_reason_code_t’
     
    * - 0x02
      - ip4-address
      - 获取到的IPV4地址
	 
    * - 0x03
      - mac	
      - W80X的WiFi MAC地址	

APP 向 W80X 发送秘钥协商指令
-------------------------------

手机APP通过发送该指令，将秘钥协商用的非对称算法公钥传输给W80X，此指令不支持加密传输。

- **<cmd>**：秘钥协议指令 0x0F。
- **<payload>**：payload 内容定义：
 
  .. list-table:: 
    :widths: 25 25 25
    :header-rows: 0
    :align: left

    * - type-id
      - content
      - description
     
    * - 0x09
      - public-key
      - 非对称算法公钥
     
W80X 上报秘钥协商结果
-------------------------------

W80X 通过发送该指令，将公钥加密的对称算法秘钥返回给APP，此指令不支持加密传输。

- **<cmd>**：秘钥协议结果指令 0x8F。
- **<payload>**：payload 内容定义：
 
  .. list-table:: 
    :widths: 25 25 25
    :header-rows: 0
    :align: left

    * - type-id
      - content
      - description
     
    * - 0x81
      - error-info
      - 0x00 成功；0x01 协商失败。
     
    * - 0x87
      - aes-key
      - 经过RSA公钥加密的AES秘钥。
	
APP 发送应答指令
-------------------------------

如果 W80X 发送的指令需要 APP 回复应答，APP 通过该指令回复应答。

- **<cmd>**：秘钥协议结果指令 0x10。
- **<flag>**: 固定为0。
- **<frag-no>**: 固定为0。
- **<payload>**: 一个字节的seq-no,来自于 W80X 发送指令中的seq-no。

W80X 发送应答指令
-------------------------------

如果 APP 发送的指令需要 W80X 回复应答，W80X 通过该指令回复应答。

- **<cmd>**：秘钥协议结果指令 0x90。
- **<flag>**: 固定为0。
- **<frag-no>**: 固定为0。
- **<payload>**: 一个字节的seq-no,来自于 APP 发送指令中的seq-no。


GATT 参数
============

- **<GATT Service UUID>**：0x1824。
- **<APP  -> W80X>**: 带应答写 Characteristic UUID 0x2ABC。
- **<W80X -> APP>**: Indication Characteristic UUID 0x2ABC。
