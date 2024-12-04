.. _bluetooth_mesh_dfu:

设备固件更新（DFU）
############################

Bluetooth Mesh 支持在 Mesh 网络上分发固件映像。Bluetoosh Mesh DFU 子系统实现 Bluetooth Mesh 设备固件更新模型规范版本1.0。该实现处于实验状态。

Bluetooth Mesh DFU 实现了固件映像的分发机制，并且对映像的大小、格式或使用没有任何限制。子系统的主要设计目标是提供 Bluetooth Mesh DFU 规范的合格部分，并将使用、固件验证和部署留给应用程序。

DFU 规范在 WM IoT SDK Bluetooth Mesh DFU 子系统中实现为三个单独的型号：

.. toctree::
   :maxdepth: 1

   dfu_srv
   dfu_cli
   dfd_srv

概述
********

DFU 角色
===========

Bluetooth Mesh DFU 子系统定义了 Mesh 节点在固件映像分发中必须承担的三种不同角色：

目标节点
   目标节点是传输的固件映像的接收器和用户。其所有功能都由 :ref:`bluetooth_mesh_dfu_srv` 模型实现。传输可以以任意数量的目标节点为目标，并且它们都将同时更新。

分发
   分发服务器角色在 DFU 过程中有两个目的。首先，它在 Upload Firmware 过程中充当目标节点，
   然后作为分发服务器将上传的映像分发给其他目标节点。分发服务器不选择传输的参数，
   而是依赖发起程序为其提供目标节点和传输参数的列表。分发服务器功能在两个模型中实现，
   即 :ref:`bluetooth_mesh_dfd_srv` 和 :ref:`bluetooth_mesh_dfu_cli` 。 :ref:`bluetooth_mesh_dfd_srv` 负责与发起程序通信， :ref:`bluetooth_mesh_dfu_cli` 负责将映像分发到目标节点。

发起者
   发起程序角色通常由实现 Bluetooth Mesh :ref:`Provisioner<bluetooth_Mesh_provisioning>` 和  :ref:`Configurator<Bluetooth_Mesh_models_cfg_cli>` 角色的同一设备实现。
   启动器需要对潜在目标节点及其固件的完整概述，并将控制（和启动）所有固件更新。WM IoT SDK Bluetooth Mesh DFU 子系统中未实现发起程序角色。

.. figure:: ../../../../_static/component-guides/bluetooth/api/mesh/dfu_roles_mesh.png
   :align: center
   :alt: Graphic overview of the DFU roles mesh nodes can have during the process of image
         distribution

   DFU 角色和相关的 Bluetooth Mesh 型号

Bluetooth Mesh 应用程序可以以任何方式组合 DFU 角色，甚至可以通过在单独的元素上实例化模型来接受同一角色的多个实例。例如，可以通过在启动器节点上实例化 :ref:`bluetooth_mesh_dfu_cli` 并直接调用其 API 来组合分发服务器和启动器角色。

还可以将启动器和分发服务器设备组合到单个设备中，并用将直接访问固件更新客户端模型的专有机制（例如通过串行协议）替换固件分发服务器模型。

.. note::
   所有 DFU 模型都实例化一个或多个 :ref:`bluetooth_mesh_blob` ， 并且对于某些角色组合，可能需要分布在多个元素上。

阶段
======

Bluetooth Mesh DFU 过程设计为分三个阶段工作：

上传阶段
   首先，图像由外部实体（如电话或网关（发起方））上传到网状网络中的分发服务器。在上传阶段，
   发起程序将固件映像及其所有元数据传输到网状网络内的分发服务器节点。
   分发服务器永久存储固件映像及其元数据，并等待发起程序的进一步指示。
   完成上传过程所需的时间取决于图像的大小。上传完成后，发起方可以在更耗时的分发阶段断开与网络的连接。
   一旦固件上传到分发服务器，发起程序可以随时触发分发阶段。

固件功能检查阶段（可选）
  在启动分发阶段之前，启动器可以选择检查目标节点是否可以接受新固件。
  不响应或响应为无法接收新固件的节点将被排除在固件分发过程之外。

分配阶段
   在可以分发固件映像之前，发起程序将目标节点列表及其指定的固件映像索引传输到分发服务器。
   接下来，它告诉分发服务器启动固件分发进程，该进程在后台运行，同时启动器和网状网络执行其他职责。
   一旦固件映像传输到目标节点，分发服务器可以要求他们立即应用固件映像，并报告其状态和新的固件 ID。

固件映像
===============

网格节点固件的所有可更新部分都应表示为固件映像。每个目标节点都保存固件映像的列表，每个映像都应该是可独立更新和可识别的。

固件映像表示为 BLOB（固件本身），并附带以下附加信息：

固件 ID
   固件 ID 用于标识固件映像。启动器节点可以请求目标节点提供其当前固件 ID 的列表，以确定固件的较新版本是否可用。
   固件 ID 的格式是特定于供应商的，但通常，它应包含足够的信息，以供具有格式知识的启动器节点确定映像类型及其版本。
   固件 ID 是可选的，其最大长度由 ``CONFIG_BT_MESH_DFU_FWID_MAXLEN`` 确定。

固件元数据
   目标节点使用固件元数据来确定是否应接受传入的固件更新，以及更新的效果。
   元数据格式特定于供应商，并且应包含目标节点验证映像所需的所有信息，以及目标节点在应用映像之前必须进行的任何准备。
   典型的元数据信息可以是图像签名、节点合成数据的更改和 BLOB 的格式。
   目标节点可以在接受传入传输之前执行元数据检查，以确定是否应启动传输。
   在元数据检查后，目标节点可以丢弃固件元数据，因为其他节点永远不会从目标节点请求元数据。
   固件元数据是可选的，其最大长度由 ``CONFIG_BT_MESH_DFU_METADATA_MAXLEN`` 确定。

   WM IoT SDK 中的 Bluetooth Mesh DFU 子系统提供自己的元数据格式（:c:struct:`bt_mesh_dfu_metadata`）以及一组可由最终产品使用的相关函数。使用 ``CONFIG_BT_MESH_DFU_METADATA`` 选项启用对它的支持。元数据的格式如下表所示。

+------------------------+--------------+----------------------------------------+
| Field                  | Size (Bytes) | Description                            |
+========================+==============+========================================+
| New firmware version   | 8 B          | 1 B: Major version                     |
|                        |              | 1 B: Minor version                     |
|                        |              | 2 B: Revision                          |
|                        |              | 4 B: Build number                      |
+------------------------+--------------+----------------------------------------+
| New firmware size      | 3 B          | Size in bytes for a new firmware       |
+------------------------+--------------+----------------------------------------+
| New firmware core type | 1 B          | Bit field:                             |
|                        |              | Bit 0: Application core                |
|                        |              | Bit 1: Network core                    |
|                        |              | Bit 2: Applications specific BLOB.     |
|                        |              | Other bits: RFU                        |
+------------------------+--------------+----------------------------------------+
| Hash of incoming       | 4 B          | Lower 4 octets of AES-CMAC             |
| composition data       | (Optional)   | (app-specific-key, composition data).  |
|                        |              | This field is present, if Bit 0 is set |
|                        |              | in the New firmware core type field.   |
+------------------------+--------------+----------------------------------------+
| New number of elements | 2 B          | Number of elements on the node         |
|                        | (Optional)   | after firmware is applied.             |
|                        |              | This field is present, if Bit 0 is set |
|                        |              | in the New firmware core type field.   |
+------------------------+--------------+----------------------------------------+
| Application-specific   | <variable>   | Application-specific data to allow     |
| data for new firmware  | (Optional)   | application to execut some             |
|                        |              | vendor-specific behaviors using        |
|                        |              | this data before it can respond        |
|                        |              | with a status message.                 |
+------------------------+--------------+----------------------------------------+

固件 URI
   固件 URI 为启动器提供有关可以在何处找到映像的固件更新的信息。URI 指向发起程序可以与之交互以获取固件的新版本的在线资源。
   这允许发起程序通过与 URI 中指向的 web 服务器交互来对网状网络中的任何节点执行更新。URI 必须使用 ``http`` 或 ``https`` 方案指向资源，
   并且目标 web 服务器必须根据规范定义的固件检查 HTTPS 过程进行操作。固件 URI 是可选的，其最大长度由 ``CONFIG_BT_MESH_DFU_URI_MAXLEN`` 确定。

   .. note::

      不支持带外分发机制。

.. _bluetooth_mesh_dfu_firmware_effect:

固件效果
---------------

新图像可以具有与在目标节点上分配的合成数据页面0不同的合成数据页。
这可能会影响节点的配置数据以及分发服务器终结 DFU 的方式。
根据新旧映像上远程配置服务器型号的可用性，设备可能在应用新固件后在未配置的情况下启动，
也可能需要重新配置。可用选项的完整列表在 :c:enum:`bt_mesh_dfu_effect` 中定义：

:c:enum:`BT_MESH_DFU_EFFECT_NONE`
   编程新固件后，设备保持供应状态。如果新固件的组成数据没有更改，则选择此效果。
:c:enum:`BT_MESH_DFU_EFFECT_COMP_CHANGE_NO_RPR`
   当合成数据更改且设备不支持远程供应时，选择此效果。新的合成数据仅在重新配置后发生。
:c:enum:`BT_MESH_DFU_EFFECT_COMP_CHANGE`
   当合成数据更改并且设备支持远程供应时，选择此效果。在这种情况下，设备将保持供应状态，新的合成数据将在使用远程供应模型重新供应后发生。
:c:enum:`BT_MESH_DFU_EFFECT_UNPROV`
  如果新固件中的合成数据发生更改，设备不支持远程配置，并且新的合成数据在应用固件后生效，则选择此效果。

当目标节点接收到固件更新固件元数据检查消息时，固件更新服务器模型调用 :c:member:`bt_mesh_dfu_srv_cb.check` 回调。
如果效果为 :c:enum:`BT_MESH_DFU_EFFECT_COMP_CHANGE` ，
则应用程序必须调用函数 :c:func:`bt_mesh_comp_change_prepare` 和 :c:func:`bt_mesh_models_metadata_change_prepare` 。
有关详细信息，请参见 :ref:`bluetooth_mesh_dfu_srv_comp_data_and_models_metadata` 。

DFU 程序
**************

DFU 协议被实现为一组必须按特定顺序执行的过程。

发起程序控制 DFU 协议的 Upload 阶段，上传子过程的所有分发服务器端处理都在 :ref:`bluetooth_mesh_dfd_srv` 中实现。

分发阶段由分发服务器控制，由 :ref:`bluetooth_mesh_dfu_cli` 。目标节点在 :ref:`bluetooth_mesh_dfu_srv` 中实现对这些过程的所有处理，并通过一组回调通知应用程序。

.. figure:: ../../../../_static/component-guides/bluetooth/api/mesh/dfu_stages_procedures_mesh.png
   :align: center
   :alt: Overview of DFU stages and procedures

   从分发处看到的 DFU 阶段和程序

上传固件
======================

上传固件过程使用 :ref:`bluetooth_mesh_blob` 将固件映像从启动器传输到分发服务器。上传固件过程分为两个步骤：

1. 启动器生成 BLOB ID，并将其与固件信息和 BLOB 传输的其他输入参数一起发送到分发服务器的固件分发服务器。固件分发服务器存储信息，并为传入传输准备其 BLOB 传输服务器，然后用状态消息响应发起程序。
#. 启动器的 BLOB 传输客户端模型将固件映像传输到分发服务器的 BLOB- 传输服务器，该服务器将映像存储在预定的闪存分区中。

当 BLOB 传输完成时，固件映像已准备好分发。启动器可以将多个固件映像上载到分发服务器，并要求其以任何顺序或随时分发它们。其他过程可用于从分发服务器查询和删除固件映像。

可以使用配置选项配置以下与固件映像相关的分发服务器功能：

* ``CONFIG_BT_MESH_DFU_SLOT_CNT`` ：设备上可用的映像插槽数量。
* ``CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE``: 每个图像允许的最大大小。
* ``CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE``: 所有图像的可用空间。

填充分发服务器的接收者列表
===========================================

分发服务器可以开始分发固件映像之前，它需要将映像发送到的目标节点列表。
发起程序通过直接查询潜在目标或通过某些外部授权来获取目标节点的完整列表。
启动器使用此信息将每个目标节点的地址和相关固件映像索引填充分发服务器的接收器列表。
发起程序可以发送一个或多个固件分发接收器添加消息来构建分发服务器的接收器列表，并发送固件分发接收器删除所有消息来清除该列表。

可以添加到分发服务器的最大接收机数量是通过 ``CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX`` 配置选项配置的。

启动分发
===========================

分发服务器存储固件映像并接收到目标节点列表后，发起程序可以启动分发过程。
分发的 BLOB 传输参数与更新策略一起传递给分发服务器。更新策略决定分发服务器是否应请求在目标节点上应用固件。
分发服务器存储传输参数，并开始将固件映像分发到其目标节点列表。

固件分发
---------------------

分发服务器的固件更新客户端模型使用其 BLOB 传输客户端模型的广播子系统与所有目标节点通信。通过以下步骤执行固件分发：

1. 分发服务器的固件更新客户端模型生成 BLOB ID，并将其发送到每个目标节点的固件更新服务器模型，以及其他 BLOB 传输参数、目标节点固件映像索引和固件映像元数据。每个目标节点执行元数据检查，并为传输准备其 BLOB 传输服务器模型，然后向固件更新客户端发送状态响应，指示固件更新是否会对节点的 Bluetooth Mesh 状态产生任何影响。
#. 分发服务器的 BLOB 传输客户端模型将固件映像传输到所有目标节点。
#. 一旦接收到 BLOB 传输，目标节点的应用程序将通过对映像元数据执行签名验证或映像校验和等检查来验证固件是否有效。
#. 分发服务器的固件更新客户端模型查询所有目标节点，以确保它们都已验证固件映像。

如果分发过程完成，并且至少有一个目标节点报告已接收并验证图像，则分发过程被认为成功。

.. note::
   仅当所有目标节点都丢失时，固件分发过程才会失败。发起方可以从分发服务器请求失败的目标节点的列表，
   并在当前尝试完成后发起其他尝试来更新丢失的目标节点。

暂停分发
---------------------------

发起程序还可以请求分发服务器挂起固件分发。在这种情况下，分发服务器将停止向目标节点发送任何消息。当固件分发恢复时，分发服务器将继续从上次成功传输的块发送固件。

应用固件映像
===========================

如果发起程序请求，分发服务器可以在成功接收和验证固件映像的所有目标节点上启动在目标节点上应用固件过程。
在目标节点上应用固件过程不需要参数，为了避免模棱两可，应在启动新传输之前执行该过程。在目标节点上应用固件过程由以下步骤组成：

1. 分发服务器的固件更新客户端模型指示已验证固件映像的所有目标节点应用它。目标节点的固件更新服务器模型在调用其应用程序的 ``apply`` 回调之前以状态消息响应。
#. 目标节点的应用程序在应用传输之前执行所需的任何准备，例如存储合成数据的快照或清除其配置。
#. 目标节点的应用程序将当前固件与新映像交换，并使用新的固件 ID 更新其固件映像列表。
#. 分发服务器的固件更新客户端型号请求来自每个目标节点的固件映像的完整列表，并扫描该列表以确保新固件 ID 已替换旧固件 ID。

.. note::
   在分发过程中的元数据检查期间，目标节点可能报告在应用固件映像后它将变为未配置。
   在这种情况下，分发服务器的固件更新客户端模型将发送对完整固件映像列表的请求，并且预期没有响应。

正在取消分发
===========================

启动器可以随时取消固件分发。在这种情况下，分发服务器通过向所有目标节点发送取消消息来启动取消过程。
分发服务器等待来自所有目标节点的响应。一旦所有目标节点都已回复，或请求超时，分发过程将被取消。
在此之后，可以从 ``Firmware distribution`` 部分再次启动分发过程。


API 参考
*************

本节列出了设备固件更新网格模型的常见类型。

.. doxygengroup:: bt_mesh_dfd
   :project: wm-iot-sdk-apis
   :members:

.. doxygengroup:: bt_mesh_dfu
   :project: wm-iot-sdk-apis
   :members:

.. doxygengroup:: bt_mesh_dfu_metadata
   :project: wm-iot-sdk-apis
   :members:
