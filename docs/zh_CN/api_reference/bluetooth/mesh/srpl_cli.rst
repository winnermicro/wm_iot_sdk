.. _bluetooth_mesh_srpl_cli:

请求 PDU RPL 配置客户端
#########################################

请求 PDU RPL 配置客户端模型是由 Bluetooth mesh 规范定义的基础模型。该模型是可选的，并通过 ``CONFIG_BT_MESH_SOL_PDU_RPL_CLI`` 选项启用。

请求 PDU RPL 配置客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入，并支持从支持 :ref:`Bluetooth_Mesh_SRPL_srv` 模型的节点的请求重播保护列表（SRPL）中删除地址的功能。

请求 PDU RPL 配置客户端模型使用配置客户端配置的应用程序密钥与请求 PDU RPC 配置服务器模型通信。

如果存在，则只能在主元素上实例化请求 PDU RPL 配置客户端模型。

配置
**************

可以使用传输超时选项 ``CONFIG_BT_MESH_SOL_PDU_RPL_CLI_timeout`` 配置请求 PDU RPL 配置客户端模型行为。
``CONFIG_BT_MESH_SOL_PDU_RPL_CLI_TIMEOUT`` 控制请求 PDU RPL 配置客户端等待响应消息到达的时间（以毫秒为单位）。
可以在运行时使用 :c:struct:`bt_mesh_sol_pdu_rpl_cli_timeout_set` 更改该值。

API 参考
*************

.. doxygengroup:: bt_mesh_sol_pdu_rpl_cli
   :project: wm-iot-sdk-apis
   :members:
