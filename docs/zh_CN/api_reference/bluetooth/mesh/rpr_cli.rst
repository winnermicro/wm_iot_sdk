.. _bluetooth_mesh_models_rpr_cli:

远程配置客户端
##########################

远程配置客户端模型是由 Bluetooth mesh 规范定义的基础模型。它通过 ``CONFIG_BT_MESH_RPR_CLI`` 选项启用。

远程配置客户端模型在 Bluetooth Mesh 协议规范版本1.1中引入。
该模型提供了将设备远程配置到网格网络中的功能，
并通过与支持 :ref:`bluetooth_mesh_models_rpr_srv` 模型的网格节点进行交互来执行节点配置协议接口过程。

远程配置客户端模型使用包含目标远程配置服务器模型实例的节点的设备密钥与远程配置服务器模式进行通信。

如果存在，则必须在主元素上实例化远程配置客户端模型。

扫描
===============

扫描过程用于扫描远程配置服务器附近的未配置设备。远程配置客户端通过使用 :c:struct:`bt_mesh_rpr_scan_start` 调用启动扫描过程：

.. code-block:: C

      static void rpr_scan_report(struct bt_mesh_rpr_cli *cli,
                  const struct bt_mesh_rpr_node *srv,
                  struct bt_mesh_rpr_unprov *unprov,
                  struct net_buf_simple *adv_data)
      {

      }

      struct bt_mesh_rpr_cli rpr_cli = {
         .scan_report = rpr_scan_report,
      };

      const struct bt_mesh_rpr_node srv = {
         .addr = 0x0004,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      struct bt_mesh_rpr_scan_status status;
      uint8_t *uuid = NULL;
      uint8_t timeout = 10;
      uint8_t max_devs = 3;

      bt_mesh_rpr_scan_start(&rpr_cli, &srv, uuid, timeout, max_devs, &status);

上面的示例显示了用于在目标远程配置服务器节点上启动扫描过程的伪代码。
此过程将启动10秒钟的多设备扫描，其中生成的扫描报告最多包含三个未配置的设备。
如果指定了 UUID 参数，则相同的过程将仅扫描具有相应 UUID 的设备。过程完成后，
服务器发送将在客户端的 :c:struct:`bt_mesh_rpr_cli.scan_report` 回调中处理的扫描报告。

此外，远程配置客户端模型还支持使用 :c:struct:`bt_mesh_rpr_scan_start_ext` 调用进行扩展扫描。
扩展扫描通过允许远程配置服务器报告特定设备的其他数据来补充常规扫描。
远程配置服务器将使用活动扫描来请求来自未配置设备的扫描响应（如果未配置设备支持）。

预配置
===============

远程配置客户端通过使用 :c:struct:`bt_mesh_provision_Remote` 调用启动配置过程：

.. code-block:: C

      struct bt_mesh_rpr_cli rpr_cli;

      const struct bt_mesh_rpr_node srv = {
         .addr = 0x0004,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      uint8_t uuid[16] = { 0xaa };
      uint16_t addr = 0x0006;
      uint16_t net_idx = 0;

      bt_mesh_provision_remote(&rpr_cli, &srv, uuid, net_idx, addr);

上面的示例显示了通过远程配置服务器节点远程配置设备的伪代码。
此过程将尝试为设备提供相应的 UUID，并使用位于索引零的网络密钥将地址 0x0006 分配给其主要元素。

.. note::
   在远程配置期间，将触发与普通配置相同的 :c:struct:`bt_mesh_prov` 回调。有关详细信息，请参阅节 :ref:`bluetooth_mesh_provisioning` 。

重新配置
===============

除了扫描和配置功能外，远程配置客户端还提供了在支持 :ref:`bluetooth_mesh_models_rpr_srv` 模型。
这是通过节点配置协议接口（NPPI）提供的，该接口支持以下三个过程：

* 设备密钥刷新过程：用于更改目标节点的设备密钥，而不需要重新配置节点。
* 节点地址刷新过程：用于更改节点的设备密钥和单播地址。
* 节点组成刷新过程：用于更改节点的设备密钥，以及添加或删除节点的模型或功能。

可以使用 :c:struct:`bt_mesh_reprovision_remote` 调用启动三个 NPPI 过程：

.. code-block:: C

      struct bt_mesh_rpr_cli rpr_cli;
      struct bt_mesh_rpr_node srv = {
         .addr = 0x0006,
         .net_idx = 0,
         .ttl = BT_MESH_TTL_DEFAULT,
      };

      bool composition_changed = false;
      uint16_t new_addr = 0x0009;

      bt_mesh_reprovision_remote(&rpr_cli, &srv, new_addr, composition_changed);

上面的示例显示了用于在目标节点上触发节点地址刷新过程的伪代码。
具体程序不是直接选择的，而是通过输入的其他参数选择的。在该示例中，
我们可以看到 Target 的当前单播地址是 0x0006，而新地址被设置为 0x0009。
如果这两个地址相同，并且 ``composition_changed`` 标志设置为 true，
则该代码将触发节点合成刷新过程。如果这两个地址相同，并且 ``composition_changed`` 标志设置为 false，
则该代码将触发设备密钥刷新过程。

头文件
===============

- wm_iot_sdk/components/bluetooth/include/bluetooth/mesh/rpr_cli.h
- This header file can be included with:

.. code-block:: c
   :emphasize-lines: 1

   #include "bluetooth/mesh/rpr_cli.h"

API 参考
===============

.. doxygengroup:: bt_mesh_rpr_cli
   :project: wm-iot-sdk-apis
   :members:
