.. _bluetooth_mesh_dfd_srv:

固件分发服务器
############################

固件分发服务器模型为 :ref:`bluetooth_mesh_dfu` 子系统实现分发服务器角色。它扩展了 :ref:`bluetooth_mesh_blob_srv` ，
用于从启动器节点接收固件映像二进制文件。它还实例化了一个 :ref:`bluetooth_mesh_dfu_cli` ，用于在整个网状网络中分发固件更新。

.. note::

   目前，固件分发服务器仅支持通过 SMP 服务进行固件映像的带外（OOB）检索。

固件分发服务器没有自己的 API，但依赖于不同设备上的固件分发客户端模型来为其提供信息并触发映像分发和上传。

固件插槽
===============

固件分发服务器能够存储多个固件映像以进行分发。每个插槽包含具有元数据的单独固件映像，
并且可以以任何顺序分发到网络中的其他网状节点。固件映像的内容、格式和大小特定于供应商，
并且可能包含来自其他供应商的数据。应用程序不应尝试执行或修改它们。

插槽由固件分发客户端远程管理，该客户端既可以上载新插槽，也可以删除旧插槽。通过固件分发服务器的回调（ :cpp:type:`bt_mesh_fd_srv_cb` ）将插槽的更改通知应用程序。虽然每个固件插槽的元数据都存储在内部，但应用程序必须提供 :ref:`bluetooth_mesh_blob_stream` 以读取和写入固件映像。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/dfd_srv.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/dfd_srv.h"
   
API 参考
===============

.. doxygengroup:: bt_mesh_dfd_srv
   :project: wm-iot-sdk-apis
   :members:
