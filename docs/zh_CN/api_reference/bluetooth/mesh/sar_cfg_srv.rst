.. _bluetooth_mesh_sar_cfg_srv:

SAR 配置服务器
########################

SAR 配置服务器模型是 Bluetooth Mesh 规范定义的基础模型。它是一种可选型号，通过 ``CONFIG_BT_MESH_SAR_CFG_SRV`` 配置选项启用。

SAR 配置服务器模型在 Bluetooth Mesh Protocol Specification 版本1.1中引入，它支持配置 :ref:`segmentation and reassembly (SAR) <bluetooth_mesh_sar_cfg>` Bluetooth Mesh 节点的。该模型为 SAR 配置定义了一组状态和消息。

SAR 配置服务器模型定义了两种状态，SAR 发射机状态和 SAR 接收机状态。有关这两个状态的更多信息，请参见 :ref:`bt_mesh_sar_cfg_states` 。

该模型还支持 SAR 发射机和 SAR 接收机获取和设置消息。

SAR 配置服务器模型没有自己的 API，但依赖于 :ref:`bluetooth_mesh_sar_cfg_cli` 来控制它。SAR 配置 Server 模型仅接受使用节点的设备密钥加密的消息。

如果存在，SAR 配置服务器模型必须仅在主元素上实例化。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/sar_cfg_srv.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/sar_cfg_srv.h"

API 参考
===============

.. doxygengroup:: bt_mesh_sar_cfg_srv
   :project: wm-iot-sdk-apis
   :members:
