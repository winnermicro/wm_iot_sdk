.. _bluetooth_mesh_models_cfg_cli:

配置客户端
####################

配置客户端模型是 Bluetooth Mesh 规范定义的基础模型。它提供用于配置网格节点的大多数参数的功能，包括加密密钥、模型配置和功能启用。

配置客户端模型使用目标节点的设备密钥与 :ref:`bluetooth_mesh_models_cfg_srv` 模型通信。配置客户端模型可以与其他节点上的服务器通信，也可以通过本地配置服务器模型进行自我配置。

配置客户端 API 中的所有配置函数都将 ``net_idx`` 和 ``addr`` 作为其第一个参数。这些应该设置为为目标节点提供的网络索引和主单播地址。

配置客户端模型是可选的，并且它必须仅在主元素上实例化（如果在合成数据中存在）。

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/cfg_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/cfg_cli.h"

API 参考
-----------

.. doxygengroup:: bt_mesh_cfg_cli
