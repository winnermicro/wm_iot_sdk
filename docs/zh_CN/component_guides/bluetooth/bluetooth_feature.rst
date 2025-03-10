.. _bluetooth-feature:


支持的功能
******************

WM IoT SDK 集成了功能丰富且高度可配置的蓝牙协议栈。

* 蓝牙 v5.3 兼容。

  * 高度可配置。

      * 功能、缓冲区大小/计数、堆栈大小等。

* 符合蓝牙 SIG 标准。

* 蓝牙低功耗控制器支持。

  * 支持做主，做从，同时做主和做从。
  * 支持所有 v4.2 规范功能。
  * 支持做主可以连接6个外设。
  * 智能调度角色，最大限度减少重叠。
  * 便携式设计，适用于任何开放式 BLE 天线。
  * 支持在不同物理传输上构建控制器（ HCI ）。

* 蓝牙主机支持。

  * 具有所有可能的LE角色的通用访问配置文件（ GAP ）。

    * 外围设备和中央。
    * 观察员和广播者。
    * 支持多个 PHY （ 2Mbit/s ，编码）。
    * 扩展广告。
    * 定期广告（包括同步传输）。

  * GATT （通用属性配置文件）。

    * 服务器（作为外设角色）。
    * 客户端（作为主设备，用来连接外设）。
    * 增强型 ATT （ EATT ）。
    * GATT 数据库哈希。
    * GATT 多重通知。

  * 配对支持，包括蓝牙 4.2 的安全连接功能。

  * 非易失性存储支持，可永久存储特定于蓝牙的存储设置和数据。

  * 蓝牙网状网络支持。

    * 中继、友元节点、低功耗节点（ LPN ）和 GATT 代理功能。
    * 支持预配角色和持有者（ PB-ADV & PB-GATT ）。
    * 包括基础模型。
    * 高度可配置，适合小至 16k RAM 的设备。

  * 基础蓝牙 BR/EDR （ Classic ）支持。

    * 通用访问配置文件（ GAP ）。
    * 逻辑链路控制和适配协议 （ L2CAP ）。
    * 串行端口仿真（ RFCOMM 协议 ）。
    * 服务发现协议 （ SDP ）。
