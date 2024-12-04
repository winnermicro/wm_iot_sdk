.. _bluetooth-arch:


蓝牙协议栈架构
******************

WM IoT SDK 蓝牙协议栈采用分层架构设计，其分层结构如下图所示：

.. figure:: ../../../_static/component-guides/bluetooth/bluetooth_arch.svg
    :align: center
    :alt: WM IoT SDK 蓝牙协议栈架构

* Applications

  * 应用程序层位于整个蓝牙协议栈最顶层，用户基于主机协议栈 API 及 Profiles 开发自己的应用程序。

* Profiles

  * Profiles 位于主机协议栈之上，它包含了蓝牙主机协议栈提供的标准 Profiles 和 WM IoT SDK 实现的通用Profiles。

    * :ref:`标准的 Profiles <bluetooth_services>` 包含了电池服务、心率计服务、即时告警服务、对象传输服务。
    * :ref:`BLE Mesh <bluetooth_mesh>` 包含了 Mesh 协议的基础功能和 WM IoT SDK 实现的多个通用模型实例。
    * :ref:`BLE Wi-Fi Prov <ble_wifi_provision>` 是 WM IoT SDK 实现的通过 BLE 方式为 Wi-Fi 入网提供配置参数，使得设备能够快速连接到路由器或者软 AP 的示例。实现参考 :ref:`Wi-Fi 配网协议 <bluetooth-wifiprov-proto>`。

* Host

  * WM IoT SDK 蓝牙主机协议栈提供完整的低功耗蓝牙（BLE）协议栈，以及经典蓝牙（BR/EDR）主机的一部分。

* Host Controller接口
  
  * Bluetooth Specification定义了 Host 和 Controller 通讯的 HCI 协议，HCI 协议可以在不同的物理传输通道上实现，比如 UART , SPI 。对于在单一芯片上实现了 Host、和 Controller 协议栈的系统 Host 和 Controller 的通讯常采用直接 API 调用方式。

* Controller

  * 蓝牙控制器位于整个架构底端，它集成了 Modem、基带、链路控制、链路管理及HCI管理。 WM IoT SDK 以库的形式发布蓝牙控制器。