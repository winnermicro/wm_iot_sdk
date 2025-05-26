.. _bluetooth_mesh_models_cfg_srv:

配置服务器
####################

配置服务器模型是 Bluetooth Mesh 规范定义的基础模型。配置服务器模型控制网格节点的大多数参数。它没有自己的 API，但依赖于 :ref:`bluetooth_mesh_models_cfg_cli` 来控制它。

.. note::
   :func:`bt_mesh_cfg_srv` 结构已弃用。中继、信标、朋友、网络传输和中继重传的初始值应通过 Kconfig 设置，
   心跳功能应通过 :ref:`bluetooth_mesh_heartbeat` API。 

配置服务器模型在所有 Bluetooth Mesh 节点上都是必需的，并且只能在主元素上实例化。

头文件
-----------

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/cfg_srv.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/cfg_srv.h"

API 参考
-----------

.. doxygengroup:: bt_mesh_cfg_srv
