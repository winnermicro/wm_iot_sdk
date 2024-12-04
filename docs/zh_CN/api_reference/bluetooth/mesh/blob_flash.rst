.. _bluetooth_mesh_blob_flash:

BLOB 闪存
##########

BLOB 闪存读写器实现对闪存映射 :func:`<Flash_map_api>` 中定义的闪存分区的 BLOB 读写。


BLOB闪存读取器
*****************

BLOB Flash Reader 与 BLOB Transfer Client 交互，以直接从闪存读取 BLOB 数据。在传递给 BLOB 传输客户端之前，必须通过调用 :func:`bt_mesh_blob_flash_rd_init（）` 对其进行初始化。每个 BLOB Flash Reader 一次仅支持一次传输

BLOB Flash Writer（BLOB 闪存编写器）
*******************************************

BLOB Flash Writer 与 BLOB 传输服务器交互，以将 BLOB 数据直接写入闪存。
在传递给 BLOB 传输服务器之前，必须通过调用 :func:`bt_mesh_blob_flash_rd_init` 对其进行初始化。
每个 BLOB Flash Writer 一次只支持一个传输，并且需要闪存页面大小的倍数的块大小。
如果在块大小小于闪存页面大小的情况下启动传输，则传输将被拒绝。

BLOB Flash Writer 将区块数据复制到缓冲区中，以容纳与闪存写入块大小未对齐的区块。如果块的开始或长度未对齐，则用 ``0xff`` 填充缓冲区数据。

API 参考
*************

.. doxygengroup:: bt_mesh_blob_io_flash
   :project: wm-iot-sdk-apis
