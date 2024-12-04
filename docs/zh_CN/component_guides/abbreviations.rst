:orphan:

.. |Equipment-Name| replace:: w800

缩写词索引
=======================

.. glossary::

    A2DP
      高级音频分发框架

    ADC
      模拟数字转换器
    
    ALPN
      应用层协议协商

    AT
      AT 是 attention 的缩写。
    
    AT 命令端口
      也称为 AT 命令口，用于发送 AT 命令和接收响应的端口。更多介绍请参考 :term:`AT 端口`。

    AT 日志端口
      也称为 AT 日志口，用于输出 AT 日志的端口。更多介绍请参考 :term:`AT 端口`。

    AT 端口
      AT 端口是 AT 日志端口（用于输出日志）和 AT 命令端口（用于发送 AT 命令和接收响应）的总称。请参考  `待做 硬件连接 <https://#>`_  了解默认的 AT 端口管脚，参考  `待做  如何设置 AT 端口管脚 <https://#>`_ 了解如何自定义 AT 端口管脚。

    Bluetooth LE
      低功耗蓝牙
    
    BluFi
      BluFi 是一款基于蓝牙通道的 Wi-Fi 网络配置功能

    命令模式
      AT 的默认工作模式。在命令模式下，AT 命令端口收到的任何字符都会被当作 AT 命令进行处理，同时 AT 会在命令端口回复命令执行结果。
      AT 在下列情况下，会从 :term:`命令模式` 进入 :term:`数据模式`。

      .. parsed-literal::

        * 发送 :ref:`AT+CIPSEND <cmd-SEND>` 设置命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSEND <cmd-SEND>` 执行命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSENDL <cmd-SENDL>` 设置命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSENDEX <cmd-SENDEX>` 设置命令成功，回复 `OK` 之后
        |Equipment-Name| * 发送 :ref:`AT+BTSPPSEND <cmd-BTSPPSEND>` 执行命令成功，回复 `OK` 之后
        |Equipment-Name| * 发送 :ref:`AT+BLESPP <cmd-BLESPP>` 执行命令成功，回复 `OK` 之后

      在数据模式下，发送 :ref:`+++ <cmd-PLUS>` 命令，会从 :term:`数据模式` 退出，进入 :term:`命令模式`。

    数据模式
      在数据模式下，AT 命令端口收到的任何字符都会被当作数据（除了特殊的 :ref:`+++ <cmd-PLUS>`），而不是 AT 命令，这些数据会无修改的发往对端。
      AT 在下列情况下，会从 :term:`命令模式` 进入 :term:`数据模式`。

      .. parsed-literal::

        * 发送 :ref:`AT+CIPSEND <cmd-SEND>` 设置命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSEND <cmd-SEND>` 执行命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSENDL <cmd-SENDL>` 设置命令成功，回复 `OK` 之后
        * 发送 :ref:`AT+CIPSENDEX <cmd-SENDEX>` 设置命令成功，回复 `OK` 之后
        |Equipment-Name| * 发送 :ref:`AT+BTSPPSEND <cmd-BTSPPSEND>` 执行命令成功，回复 `OK` 之后
        |Equipment-Name| * 发送 :ref:`AT+BLESPP <cmd-BLESPP>` 执行命令成功，回复 `OK` 之后

      在数据模式下，发送 :ref:`+++ <cmd-PLUS>` 命令，会从 :term:`数据模式` 退出，进入 :term:`命令模式`。

    DHCP
      动态主机配置协议
    
    DNS
      域名系统
    
    DTIM
      延迟传输指示映射 
    
    GATTC
      GATT 客户端
    
    GATTS
      GATT 服务器
    
    HID
      人机接口设备
    
    I2C
      集成电路总线
    
    ICMP
      因特网控制报文协议
    
    LwIP
      一个轻量级的 TCP/IP 协议栈

    LWT
      遗嘱
    
    MAC
      MAC 地址
    
    mDNS
      多播 DNS

    manufacturing nvs
      一个适用于量产的 NVS。manufacturing nvs 中存储了 AT 固件默认所用到的所有证书、私钥、GATTS 数据、模组信息、Wi-Fi 配置、UART 配置等。这些配置信息，默认值在  `待做 raw_data <https://#>`_ 里，最终生成了 ``mfg_nvs.bin``，烧录到 ``at_customize.csv`` 中定义的位置。

    MSB
      最高有效位
    
    MTU
      最大传输单元
    
    NVS
      非易失性存储器
    
    普通传输模式
      默认传输模式

      在普通传输模式下，用户可以发送 AT 命令。
      例如，用户可以通过 :ref:`AT+CIPSEND <cmd-SEND>` 命令，发送 AT 命令口收到的 MCU 数据到传输对端。从传输对端收到的数据，会通过 AT 命令口返回给 MCU，同时会附带 `+IPD` 信息。

      普通传输模式时，如果连接断开，|Equipment-Name| 不会重连，并提示连接断开。

      更多介绍请参考 :term:`传输模式时序`。

    OWE
      机会性无线加密。OWE 是一种 Wi-Fi 标准，它确保每对端点之间的通信受到保护，不受其他端点的影响。

      更多介绍请参考 `维基百科 <https://en.wikipedia.org/wiki/Opportunistic_Wireless_Encryption>`__。

    透传模式
      也称为 “透传发送接收模式”。

      在透传模式下，用户不能发送其它 AT 命令，除了特别的 :ref:`+++ <cmd-PLUS>` 命令。AT 命令口收到的所有的 MCU 数据都将无修改地，发送到传输对端。从传输对端收到的数据也会通过 AT 命令口无修改地，返回给 MCU。

      Wi-Fi 透传模式传输时，如果连接断开，|Equipment-Name| 作为客户端时，会不停地尝试重连，此时单独输入 :ref:`+++ <cmd-PLUS>` 退出透传，则停止重连；|Equipment-Name| 作为服务器时，会关闭连接同时监听新的连接，此时单独输入 :ref:`+++ <cmd-PLUS>` 退出透传。

      更多介绍请参考 :term:`传输模式时序`。

    传输模式时序
      .. figure:: ../../_static/abbreviations/intro_tt_mode.png
        :align: center
        :alt: 传输模式时序
        :figclass: align-center
    
        传输模式时序

      更多细节见下面的介绍。

      * :term:`普通传输模式` (Normal Transmission Mode)
      * :term:`透传接收模式` (Passthrough Receiving Mode)
      * :term:`透传模式` (Passthrough Mode)
      * :ref:`AT+CIPMODE <cmd-IPMODE>`
      * :ref:`AT+CIPSEND <cmd-SEND>`
      * :ref:`+++ <cmd-PLUS>`

    透传接收模式
      在 :term:`普通传输模式` 和 :term:`透传模式` 之间的一个临时模式。

      在透传接收模式，AT 不能发送数据到传输对端；但 AT 可以收到来自传输对端的数据，通过 AT 命令口无修改地返回给 MCU。更多介绍请参考 :term:`传输模式时序`。

    PBC
      按钮配置
    
    PCI Authentication
      PCI 认证，在 WinnerMicro-AT 工程中指的是除 OPEN 和 WEP 以外的 Wi-Fi 认证模式。

    PKI
      公开密钥基础建设。公开密钥基础建设（PKI）是一组由硬件、软件、参与者、管理政策与流程组成的基础架构，其目的在于创造、管理、分配、使用、存储以及撤销数字证书。

      更多介绍请参考 `公开密钥基础建设 <https://zh.wikipedia.org/zh-cn/%E5%85%AC%E9%96%8B%E9%87%91%E9%91%B0%E5%9F%BA%E7%A4%8E%E5%BB%BA%E8%A8%AD>`__。

    PLCP
      PLCP 协议，即物理层会聚协议
    
    PMF
      受保护的管理帧
    
    PSK
      预共享密钥
    
    PWM
      脉冲宽度调制
    
    QoS
      服务质量
    
    RTC
      实时控制器，为 SoC 中的一组电路，在任何芯片模式下都能随时保持工作。
    
    SMP
      安全管理协议
    
    SNI
      服务器名称指示
    
    SNTP
      简单网络时间协议
    
    SPI
      串行外设接口
    
    SPP
      SPP 协议，即串口协议
    
    SSL
      SSL 协议，即安全套接字协议

    系统消息
      AT 命令口发往 MCU 的数据。每条系统消息通常以 ``\r\n`` 结尾。详细的系统消息说明见 :ref:`AT 消息 <at-messages>`。

    TLS
      TLS 协议，即传输层安全性协议 
    
    URC
      非请求结果码，一般为模组给 MCU 的串口返回
    
    UTC
      协调世界时
    
    UUID
      通用唯一识别码
    
    WEP
      WEP 加密方式，即有线等效加密
    
    WPA
      Wi-Fi 保护访问
    
    WPA2
      Wi-Fi 保护访问 II
    
    WPS
      Wi-Fi 保护设置