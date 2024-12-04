.. _bluetooth_mesh_srpl_srv:

请求 PDU RPL 配置服务器
#########################################

请求 PDU RPL 配置服务器模型是 Bluetooth mesh 规范定义的基础模型。如果节点已启用 :c:struct:`blueooth_mesh_od_srv` ，则启用模型。

请求 PDU RPL 配置服务器模型在 Bluetooth mesh 协议规范版本1.1中引入，
并管理保存在设备上的请求重播保护列表（SRPL）。SRPL 用于拒绝节点已处理的请求 PDU。
当节点成功处理有效的请求 PDU 消息时，消息的 SSRC 字段和 SSEQ 字段存储在节点的 SRPL 中。

请求 PDU RPL 配置服务器没有自己的 API，并且依赖于 :ref:`bluetooth_mesh_srpl_cli` 来控制它。
该模型仅接受使用配置客户端配置的应用程序密钥加密的消息。

如果存在，则只能在主元素上实例化请求 PDU RPL 配置服务器模型。

配置
**************

对于请求 PDU RPL 配置服务器模型，可以配置 ``CONFIG_BT_MESH_PROXY_SRPL_SIZE`` 选项来设置 SRPL 的大小。

API 参考
*************

.. doxygengroup:: bt_mesh_sol_pdu_rpl_srv
   :project: wm-iot-sdk-apis
   :members:
