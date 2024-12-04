
MQTT Client
==============

MQTT Client 编程模型
--------------------

W800 的 MQTT Client 编程模型如下图所示：

.. mermaid::
    :align: center

    graph LR

    A[Application task]-->|"API"|B[MQTT task]
    B[MQTT task]-->|"API"|A[Application task]
    B[MQTT task]-->|"API"|C[TCP/IP task]
    C[TCP/IP task]-->|"API"|B[MQTT task]
    B[MQTT task]-->|"event"|D[event task]
    D[event task]-->|"event"|A[Application task]

MQTT Client 程序可以看成一个无法感知上层代码（如 TCP/IP 协议栈、应用程序任务、事件任务等）的黑盒子。应用程序任务负责调用 MQTT Client 程序 API 来实现 MQTT Client 的初始化，MQTT Client 触发事件，应用程序通过触发的事件进行下一次 API 调用。



MQTT Client 概述
-----------------

MQTT Client 是 `MQTT <https://mqtt.org/>`_ 协议客户端的实现，MQTT 是一种基于发布/订阅模式的轻量级消息传输协议。

MQTT Client 特性
-----------------

    - 支持基于 TCP、SSL、Websocket 和 Websocket Secure 的 MQTT 实现。
    - 支持订阅、发布、遗嘱、保持心跳和3个服务质量(QoS)级别。
    - 通过 URI 简化配置流程。

MQTT Client 初始化
------------------

- 使用 MQTT Client 连接前必须进行初始化。调用 ``wm_mqtt_client_init()`` 函数完成对 ``wm_mqtt_client_config_t`` 结构体的初始化配置，生成一个句柄用于 MQTT Client 的开启。

- 使用 ``wm_mqtt_client_deinit()`` 函数销毁客户端的句柄。 

初始化配置
^^^^^^^^^^

对 ``wm_mqtt_client_config_t`` 结构体进行初始化配置，配置内容如下：

====================        ===================
字段                         描述
====================        ===================
uri                          统一资源定位符
client_id                    客户端 ID
username                     用户名
password                     用户密码
will_topic                   遗嘱主题
will_msg                     遗嘱消息 
will_qos                     遗嘱消息服务质量
will_retain                  消息保留标志
keep_alive                   保活周期
clean_session                清除会话
debug                        调试打印
auth_mode                    连接认证方式
====================        ===================

连接认证方式
------------

==============================================     =================
 WM_MQTT_CLIENT_AUTH_MODE_NOT_CERT                    无证书认证
 WM_MQTT_CLIENT_AUTH_MODE_AUTH_SERVER_CERT           验证服务器证书
 WM_MQTT_CLIENT_AUTH_MODE_PROVIDE_CLIENT_CERT        提供客户端证书
 WM_MQTT_CLIENT_AUTH_MODE_MUTUAL_AUTH                双向认证
==============================================     =================

消息服务质量
------------

- **QoS 0** (最多交付一次)

QoS 0 消息即发即弃，不需要等待确认，不需要存储和重传。所以也会导致消息会丢失。

- **QoS 1** (至少交付一次)   

为了保证消息到达，QoS 1 加入了应答与重传机制，发送方只有在收到接收方的 PUBACK 报文以后，才能认为消息投递成功，在此之前，发送方需要存储该 PUBLISH 报文以便下次重传。

- **QoS 2** (只交付一次)

每一次 QoS 2 消息的投递，都要求发送方与接收方进行至少两次请求/响应流程。解决了 QoS 1 消息可能会重复的问题。

连接地址
--------
MQTT 的连接地址通常包含 ：服务器 IP 或者域名、服务器端口、连接协议。

基于 TCP 的 MQTT 连接
^^^^^^^^^^^^^^^^^^^^^

- ``mqtt`` 是普通的 TCP 连接，端口一般为 1883。

- ``mqtts`` 是基于 TLS/SSL 的安全连接，端口一般为 8883。

::

    比如 mqtt://broker.emqx.io:1883 是一个基于普通 TCP 的 MQTT 连接地址。

基于 WebSocket 的连接
^^^^^^^^^^^^^^^^^^^^^

- ``ws`` 是普通的 WebSocket 连接，端口一般为 8083。

- ``wss`` 是基于 WebSocket 的安全连接，端口一般为 8084。

::

    比如 ws://broker.emqx.io:8083 是一个基于 WebSocket 的 MQTT 连接地址。

MQTT Client 连接
----------------

- 调用 ``wm_mqtt_client_connect()`` 函数，将初始化生成的 `句柄（handle）` 作为函数参数传入，完成 MQTT Client 的连接。

- 调用 ``wm_mqtt_client_disconnect()`` 函数，断开 MQTT Client 的连接。

MQTT Client 订阅
----------------

.. hint:: 客户端必须在连接后才能进行订阅动作。

通过调用 ``wm_mqtt_client_subscribe()`` 函数实现客户端订阅主题。以下是函数的参数说明：

=========       =========================
函数参数          含义
=========       =========================
handle          客户端连接初始化生成的句柄
topic           订阅的主题
qos             订阅的消息服务质量
=========       =========================

通过调用 ``wm_mqtt_client_unsubcribe()`` 函数实现客户端取消订阅主题。函数参数参考如上所示。

MQTT Client 发布
-----------------

通过调用 ``wm_mqtt_client_publish()`` 函数发布一条消息给消息代理( `MQTT broker` )。以下是函数的参数说明：

=========       =========================
函数参数          含义
=========       =========================
handle          客户端连接初始化生成的句柄
topic           订阅的主题
message         发布消息字符串
msg_len         发布消息的长度
qos             订阅的消息服务质量
retain          消息保留标志
=========       =========================

事件
-----

MQTT Client 可能会发布以下事件：

========================================= ===========================================================================================
事件                                         含义
========================================= ===========================================================================================
WM_EVENT_MQTT_CLIENT_CONNECTED              客户端已成功连接至服务器。
WM_EVENT_MQTT_CLIENT_DISCONNECTED           客户端已经断开服务器。
WM_EVENT_MQTT_CLIENT_DATA                   客户端已收到发布消息。
WM_EVENT_MQTT_CLIENT_PUBLISHED              服务器已收到客户端的发布消息。这个事件将仅针对 `QoS 1` 和 `QoS 2` 发布，因为 `QoS 0` 不会进行消息确认。
========================================= ===========================================================================================

错误码
------

MQTT Client 错误码有很多，这里进行说明：

.. list-table:: 
    :widths: 25 25 
    :header-rows: 0
    :align: center

    * - 错误码
      - 描述

    * - **WM_MQTT_CLIENT_ERROR_SUCCESS**
      - 表示没有错误，成功完成了MQTT客户端的操作。

    * - **WM_MQTT_CLIENT_ERROR_FAILURE**
      - 表示MQTT客户端操作失败。

    * - **WM_MQTT_CLIENT_ERROR_PERSISTENCE_ERROR**
      - 表示一个应用程序特定的持久性错误。
      
    * - **WM_MQTT_CLIENT_ERROR_DISCONNECTED**
      - 产生该错误的原因为MQTT客户端未连接导致。

    * - **WM_MQTT_CLIENT_ERROR_MAX_MESSAGES_INFLIGHT**
      - 该错误产生原因为已经达到允许同时发送消息的最大数量。

    * - **WM_MQTT_CLIENT_ERROR_BAD_UTF8_STRING**
      - 产生该错误的原因通常是传入的字符串检测到了无效的 ``UTF-8`` 字符串。

    * - **WM_MQTT_CLIENT_ERROR_NULL_PARAMETER**
      - 产生该错误的原因是传入的参数为空。
      
    * - **WM_MQTT_CLIENT_ERROR_TOPICNAME_TRUNCATED**
      - 主题已被截断，产生该错误的原因是主题字符串包含嵌入的NULL字符。

    * - **WM_MQTT_CLIENT_ERROR_BAD_STRUCTURE**
      - 产生该错误的原因是传入的参数结构体不正确。

    * - **WM_MQTT_CLIENT_ERROR_BAD_QOS**
      - 该错误产生原因为传入的QoS值超值指定范围（ `0` 、 `1` 、 `2` ）。

    * - **WM_MQTT_CLIENT_ERROR_SSL_NOT_SUPPORTED**
      - 产生该错误的原因尝试使用非SSL版本的库进行SSL连接。
      
    * - **WM_MQTT_CLIENT_ERROR_BAD_MQTT_VERSION**
      - 产生该错误的原因未识别MQTT版本
      
    * - **WM_MQTT_CLIENT_ERROR_BAD_PROTOCOL**
      - 产生该错误的原因为填写的访问协议前缀有误（ `mqtt` 、 `mqtt` 、 `ws` 、 `wss` ）。

    * - **WM_MQTT_CLIENT_ERROR_BAD_MQTT_OPTION**
      - 产生该错误的原因是传入的MQTT选项不适用于请求的MQTT版本。

    * - **WM_MQTT_CLIENT_ERROR_WRONG_MQTT_VERSION**
      - 产生该错误的原因是调用不适用于请求的MQTT版本。

    * - **WM_MQTT_CLIENT_ERROR_0_LEN_WILL_TOPIC**
      - 产生该错误的原因为WILL主题字符串长度为0。
