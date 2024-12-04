.. _bluetooth_mesh_od_cli:

按需专用代理客户端
##############################

按需私有代理客户端模型是 Bluetooth Mesh 规范定义的基础模型。该模型是可选的，并通过 ``CONFIG_BT_MESH_OD_PRIV_PROXY_CLI`` 选项启用。

按需私有代理客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入，用于设置和检索按需私有 GATT 代理状态。
该状态定义节点在接收请求 PDU 后播发具有专用网络标识类型的 Mesh 代理服务的时间。

按需专用代理客户端模型使用包含目标按需专用 Proxy Server 模型实例的节点的设备密钥与按需私有代理服务器模型进行通信。

如果存在，则必须仅在主元素上实例化按需专用代理客户端模型。

配置
**************

可以使用传输超时选项 ``CONFIG_BT_MESH_OD_PRIV_Proxy_CLI_timeout`` 配置按需专用代理客户端模型行为。
``CONFIG_BT_MESH_OD_PRIV_PROXY_CLI_TIMEOUT`` 控制客户端等待状态响应消息到达的时间（以毫秒为单位）。
可以使用 :c:func:`bt_mesh_od_priv_proxy_cli_timeout_set` 在运行时更改该值。


API 参考
*************

.. doxygengroup:: bt_mesh_od_priv_proxy_cli
   :project: wm-iot-sdk-apis
   :members:
