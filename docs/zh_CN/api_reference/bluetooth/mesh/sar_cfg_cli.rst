.. _bluetooth_mesh_sar_cfg_cli:

SAR 配置客户端
########################

SAR 配置客户端模型是 Bluetooth Mesh 规范定义的基础模型。它是一种可选型号，通过 ``CONFIG_BT_MESH_SAR_CFG_CLI`` 配置选项启用。

SAR 配置客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入，它支持配置支持 :ref:`Bluetooth_Mesh_SAR_cfg_srv` 模型的节点的较低传输层行为。

该模型可以使用 SAR 配置消息发送消息来查询或更改SAR配置服务器（SAR 发射机和 SAR 接收机）支持的状态。

SAR 发射机程序用于确定和配置 SAR 配置服务器的 SAR 发射机状态。函数调用 :func:`bt_mesh_sar_cfg_cli_transmitter_get` 和 :func:`bt_mesh _sar_cfg_cli_ttransmitter_set` 分别用于获取和设置目标节点的 SAR 发射机状态。

SAR 接收机过程用于确定和配置 SAR 配置服务器的 SAR 接收机状态。函数调用 :func:`bt_mesh_sar_cfg_cli_receiver_get` 和 :func:`bt_mesh _sar_cf _ cli_receivers_set` 分别用于获取和设置目标节点的 SAR 接收机状态。

有关这两个状态的详细信息，请参见 :ref:`bt_mesh_sar_cfg_states` 。

元素可以随时发送任何 SAR 配置客户端消息，以查询或更改对等节点的 SAR 配置服务器模型支持的状态。SAR 配置客户端模型仅接受使用支持 SAR 配置服务器模型的节点的设备密钥加密的消息。

如果存在，SAR 配置客户端模型必须仅在主元素上实例化。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/sar_cfg_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/sar_cfg_cli.h"

API 参考
===============

.. doxygengroup:: bt_mesh_sar_cfg_cli
   :project: wm-iot-sdk-apis
   :members:
