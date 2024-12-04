.. _bluetooth_mesh_blob:

BLOB 传输模型
####################

二进制大型对象（BLOB）传输模型实现了蓝牙 Mesh 二进制大对象传输模型规范1.0版，并提供了通过 Bluetooth Mesh 网络将大型二进制对象从单个源发送到多个目标节点的功能。
它是 :ref:`bluetooth_mesh_dfu` 的底层传输方法，但可以用于其他对象传输目的。该实现处于实验状态。

BLOB 传输模型支持高达 4 GB（2 \ :sup:`32` bytes）的连续二进制对象的传输。BLOB 传输协议具有内置的数据包丢失恢复过程，并设置检查点以确保所有目标在继续操作之前接收到所有数据。数据传输顺序不受保证。

BLOB 传输受到底层网状网络的传输速度和可靠性的限制。在理想条件下，
BLOB 可以以高达 1 kbps 的速率传输，允许在 10-15 分钟内传输 100 kB 的 BLOB。
然而，网络条件、传输能力和其他限制因素可以容易地将数据速率降低几个数量级。
根据应用程序和网络配置调整传输的参数，并将其调度到网络流量较低的时段，
将显著提高协议的速度和可靠性。然而，在实际部署中，实现接近理想速率的传输速率是不可能的。

有两种 BLOB 传输模型:

.. toctree::
   :maxdepth: 1

   blob_srv
   blob_cli

BLOB Transfer Client 在发送方节点上实例化，BLOB 传输服务器在接收方节点上实例化。

概念
********

BLOB 传输协议引入了几个新的概念来实现 BLOB 传送。

BLOBs
=====

BLOB 是大小高达 4 GB 的二进制对象，可以包含应用程序希望通过网状网络传输的任何数据。
BLOB 为连续数据对象，分为块和块，以使传输可靠且易于处理。BLOB 的内容或结构没有任何限制，
应用程序可以自由地对数据本身定义他们想要的任何编码或压缩。

BLOB 传输协议不提供 BLOB 数据的任何内置完整性检查、加密或身份验证。然而，
Bluetooth Mesh 协议的底层加密提供了数据完整性检查，并使用网络和应用程序级加密保护 BLOB 的内容免受第三方的攻击。

阻碍
------

二进制对象被划分为块，通常大小从几百字节到几千字节。
每个块都是单独传输的，BLOB 传输客户机确保所有 BLOB 转移服务器在转移到下一个块之前都已收到完整的块。
块大小由传输的 ``block_size_log`` 参数确定，并且对于传输中的所有块都相同，但最后一个块除外，
后者可能更小。对于存储在闪存中的 BLOB，块大小通常是目标设备的闪存页面大小的倍数。

块结构
---------

每个块被划分为块。区块是 BLOB 传输中的最小数据单元，并且必须适合单个 Bluetooth Mesh 访问消息（操作码除外）（379字节或更少）。传输块的机制取决于传输模式。

在推送 BLOB 传输模式下操作时，区块将作为未确认的数据包从 BLOB Transfer Client 发送到所有目标 BLOB 转移服务器。
一旦发送了块中的所有块，BLOB 传输客户端就会询问每个 BLOB Transfer Server 是否缺少任何块，
然后重新发送它们。重复此操作，直到所有 BLOB 传输服务器都接收到所有区块，或者 BLOB 转移客户端放弃。

在 Pull BLOB 传输模式下操作时，BLOB Transfer Server 将一次从 BLOB 转移客户端请求少量块，并等待 BLOB 传送客户端发送它们，
然后再请求更多块。重复此操作，直到所有区块都已传输，或者 BLOB 传输服务器放弃。

在 :ref:`bluetooth_mesh_blob_transfer_modes` 部分中阅读有关传输模式的更多信息。

.. _bluetooth_mesh_blob_stream:

BLOB 流
============

在 BLOB 传输模型的 API 中，BLOB 数据处理与高级传输处理分离。这种拆分允许为不同的应用程序重用不同的 BLOB 存储和传输策略。虽然高级传输直接由应用程序控制，但 BLOB 数据本身是通过 BLOB 流访问的。

BLOB 流与标准库文件流相当。通过打开、关闭、读取和写入，BLOB 传输模型可以完全访问 BLOB 数据，
无论它保存在闪存、RAM 还是外围设备上。BLOB 流在使用之前以访问模式（读或写）打开，
BLOB 传输模型将在 BLOB 的数据中以块和块的形式移动，使用 BLOB 数据流作为接口。

相互作用
-----------

在读取或写入 BLOB 之前，通过调用其 :c:member:`open<bt_mesh_blob_io.open>` 回调。当与 BLOB 传输服务器一起使用时，BLOB 流始终以写入模式打开，当与 BLIB 传输客户端一起使用时则始终以读取模式打开。

对于 BLOB 中的每个块，BLOB 传输模型通过调用 :c:member:`block_start<bt_mesh_blob_io.block_sstart>` 。然后，根据访问模式，
重复调用 BLOB 流的 :c:member:`wr<bt_mesh_BLOB_io.wr>` 或 :c:member:`rd<bt_mesh_blop_io.rd>` 回调以将数据移动到 BLOB 或从 BLOB 移动。
当模型完成处理块时，它调用 :c:member:`block_end<bt_mesh_blob_io.block_end>` 。
传输完成后，通过调用 :c:member:`close<bt_mesh_BLOB_io.close>` 关闭 BLOB 流。

启动位置
---------------

应用程序可以实现自己的 BLOB 流，或使用 WM IoT SDK 提供的实现：

.. toctree::
   :maxdepth: 2

   blob_flash


传输能力
=====================

每个 BLOB 传输服务器可能具有不同的传输功能。通过以下配置选项控制每个设备的传输能力：

* ``CONFIG_BT_MESH_BLOB_SIZE_MAX``
* ``CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN``
* ``CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MAX``
* ``CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX``

``CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX`` 选项也由 BLOB 传输客户端使用，并影响 BLOB 转移客户端模型结构的内存消耗。

为了确保传输可以被尽可能多的服务器接收，BLOB 传输客户端可以在开始传输之前检索每个 BLOB transfer Server 的功能。客户端将传输具有最大可能块和区块大小的 BLOB。

.. _bluetooth_mesh_blob_transfer_modes:

传输模式
==============

BLOB 可以使用两种传输模式进行传输，即 Push BLOB 传输模式和 Pull BLOB 转换模式。在大多数情况下，传输应在 Push BLOB 传输模式下进行。

在 Push-BLOB 传输模式下，发送速率由 BLOB 传输客户端控制，该客户端将在没有任何高级流控制的情况下推送每个块的所有块。推送 BLOB 传输模式支持任意数量的目标节点，并且应该是默认传输模式。

在 Pull BLOB 传输模式下，BLOB Transfer Server 将以自己的速率从 BLOB 转移客户端“拉取”区块。
Pull BLOB 传输模式可以与多个 Target 节点一起执行，用于将 BLOB 传送到充当 :ref:`bluetooth_mesh_lpn` . 
在 Pull BLOB 传输模式下操作时，BLOB Transfer Server 将小批量地从 BLOB 转移客户端请求区块，
并等待它们全部到达，然后再请求更多区块。重复此过程，直到 BLOB 传输服务器接收到块中的所有块。
然后，BLOB Transfer Client 启动下一个块，而 BLOB 传输服务器请求该块的所有块。


.. _bluetooth_mesh_blob_timeout:

传输超时时间
================

BLOB 传输的超时基于 timeout Base 值。客户端和服务器都使用相同的 Timeout Base 值，但它们计算超时的方式不同。

BLOB 传输服务器使用以下公式来计算 BLOB 传送超时::

  10 * (Timeout Base + 1) seconds


对于 BLOB 传输客户端，使用以下公式::

  (10000 * (Timeout Base + 2)) + (100 * TTL) milliseconds

其中 TTL 是传输中设置的生存时间值。

API 参考
*************

本节包含 BLOB 传输模型通用的类型和定义。

.. doxygengroup:: bt_mesh_blob
   :project: wm-iot-sdk-apis
   :members:
