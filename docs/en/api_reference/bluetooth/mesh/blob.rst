.. _bluetooth_mesh_blob:

BLOB Transfer Model 
####################

The Binary Large OBject (BLOB) transfer model implements the Bluetooth Mesh Binary Large Object Transfer Model specification version 1.0, 
providing the capability to send large binary objects from a single source to multiple destination nodes over a Bluetooth Mesh network. 
It serves as the underlying transfer method for Device Firmware Updates (DFU) but can also be utilized for other object transfer purposes. 
This implementation is considered experimental.

The BLOB transfer model supports the transmission of continuous binary objects up to 4 GB (2^32 bytes) in size. 
The BLOB transfer protocol includes a built-in packet loss recovery process and checkpoints to ensure all destinations receive all data before proceeding. 
The order of data transmission is not guaranteed.

BLOB transfers are constrained by the transmission speed and reliability of the underlying Mesh network. 
Under ideal conditions, BLOBs can be transferred at rates up to 1 kbps, allowing for the transfer of a 100 kB BLOB within 10-15 minutes. 
However, network conditions, transmission capabilities, and other limiting factors can easily reduce the data rate by several orders of magnitude. 
Adjusting the transmission parameters according to the application and network configuration, and scheduling them during periods of low network traffic, can significantly improve the speed and reliability of the protocol. 
Nevertheless, achieving transfer rates close to the ideal in real-world deployments is impractical.

There are two BLOB Transfer models:

.. toctree::
   :maxdepth: 1

   blob_srv
   blob_cli

The BLOB Transfer Client is instantiated on the sender node, and the BLOB Transfer Server is
instantiated on the receiver nodes.

Concepts
********

The BLOB transfer protocol introduces several new concepts to implement the BLOB transfer.


BLOBs
=======

A BLOB is a binary object up to 4 GB in size, which can contain any data that an application wishes to transmit over the Mesh network.
A BLOB is a continuous data object divided into blocks and segments for reliable and manageable transmission. 
There are no restrictions on the content or structure of a BLOB, and applications are free to define any encoding or compression they desire for the data itself.

The BLOB transfer protocol does not provide any built-in integrity checks, encryption or
authentication of the BLOB data. However, the underlying encryption of the Bluetooth Mesh protocol
provides data integrity checks and protects the contents of the BLOB from third parties using
network and application-level encryption.

Segmentation
----------------

Binary objects are divided into blocks, typically ranging in size from a few hundred to a few thousand bytes. 
Each block is transmitted separately, and the BLOB Transfer Client ensures that all BLOB Transfer Servers have received a complete block before moving to the next one. 
The block size is determined by the transmitted  ``block_size_log``  parameter and is the same for all blocks in a transfer, except for the last block, which may be smaller. 
For BLOBs stored in flash memory, the block size is usually a multiple of the target device's flash page size.

Block Structure
------------------

Each block is divided into segments. A segment is the smallest unit of data in BLOB transfer and must fit within a single Bluetooth Mesh Access Message (excluding opcodes) (379 bytes or less). 
The mechanism for transmitting segments depends on the transfer mode.

When operating in Push BLOB transfer mode, segments are sent as unacknowledged packets from the BLOB Transfer Client to all target BLOB Transfer Servers. 
Once all segments in a block have been sent, the BLOB Transfer Client queries each BLOB Transfer Server for any missing segments and resends them. 
This process repeats until all BLOB Transfer Servers have received all segments, or the BLOB Transfer Client abandons the transfer.

When operating in Pull BLOB transfer mode, the BLOB Transfer Server requests a small number of segments from the BLOB Transfer Client at a time and waits for the BLOB Transfer Client to send them before requesting more segments. 
This process repeats until all segments have been transferred or the BLOB Transfer Server abandons the transfer.

Read more about the transfer modes in :ref:`bluetooth_mesh_blob_transfer_modes` section.

.. _bluetooth_mesh_blob_stream:

BLOB Streams
==============

In the API of the BLOB transfer model, BLOB data processing is separated from high-level transfer handling. 
This separation allows for the reuse of different BLOB storage and transfer strategies for different applications. 
While high-level transfers are directly controlled by the application, BLOB data itself is accessed through a *BLOB stream*.

The BLOB stream is comparable to a standard library file stream. Through opening, closing, reading
and writing, the BLOB Transfer model gets full access to the BLOB data, whether it's kept in flash,
RAM, or on a peripheral. The BLOB stream is opened with an access mode (read or write) before it's
used, and the BLOB Transfer models will move around inside the BLOB's data in blocks and chunks,
using the BLOB stream as an interface.

Interactions
-------------------

Before reading or writing a BLOB, the stream is opened by calling its
:c:member:`open <bt_mesh_blob_io.open>` callback. When used with a BLOB Transfer Server, the BLOB
stream is always opened in write mode, and when used with a BLOB Transfer Client, it's always opened
in read mode.

For each block in the BLOB, the BLOB Transfer model starts by calling
:c:member:`block_start <bt_mesh_blob_io.block_start>`. Then, depending on the access mode, the BLOB
stream's :c:member:`wr <bt_mesh_blob_io.wr>` or :c:member:`rd <bt_mesh_blob_io.rd>` callback is
called repeatedly to move data to or from the BLOB. When the model is done processing the block, it
calls :c:member:`block_end <bt_mesh_blob_io.block_end>`. When the transfer is complete, the BLOB
stream is closed by calling :c:member:`close <bt_mesh_blob_io.close>`.

Starting Position
-------------------

Applications can implement their own BLOB streams or use implementations provided by the WM IoT SDK:

.. toctree::
   :maxdepth: 2

   blob_flash


Transfer Capabilities
=====================

Each BLOB Transfer Server may have different transfer capabilities. The transmission capabilities of each device are controlled through 
the following configuration options:

* ``CONFIG_BT_MESH_BLOB_SIZE_MAX``
* ``CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN``
* ``CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MAX``
* ``CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX``

The ``CONFIG_BT_MESH_BLOB_CHUNK_COUNT_MAX`` option is also used by the BLOB Transfer
Client and affects memory consumption by the BLOB Transfer Client model structure.

To ensure that the transfer can be received by as many servers as possible, the BLOB Transfer Client
can retrieve the capabilities of each BLOB Transfer Server before starting the transfer. The client
will transfer the BLOB with the highest possible block and segment size.

.. _bluetooth_mesh_blob_transfer_modes:

Transfer Modes
==============

BLOBs can be transferred using two transfer modes: Push BLOB Transfer Mode and Pull BLOB Transfer
Mode. In most cases, the transfer should be conducted in Push BLOB Transfer Mode.

In Push BLOB Transfer Mode, the sending rate is controlled by the BLOB Transfer Client, which will push
all the chunks of each block without any high level flow control. Push BLOB Transfer Mode supports
any number of Target nodes, and should be the default transfer mode.

In Pull BLOB Transfer Mode, the BLOB Transfer Server will "pull" the chunks from the BLOB Transfer
Client at its own rate. Pull BLOB Transfer Mode can be conducted with multiple Target nodes, and is
intended for transferring BLOBs to Target nodes acting as :ref:`bluetooth_mesh_lpn`. When operating
in Pull BLOB Transfer Mode, the BLOB Transfer Server will request chunks from the BLOB Transfer
Client in small batches, and wait for them all to arrive before requesting more chunks. This process
is repeated until the BLOB Transfer Server has received all chunks in a block. Then, the BLOB
Transfer Client starts the next block, and the BLOB Transfer Server requests all chunks of that
block.


.. _bluetooth_mesh_blob_timeout:

Transfer Timeout
==================

The timeout of the BLOB transfer is based on a Timeout Base value. Both client and server use the
same Timeout Base value, but they calculate timeout differently.

The BLOB Transfer Server uses the following formula to calculate the BLOB transfer timeout::

  10 * (Timeout Base + 1) seconds


For the BLOB Transfer Client, the following formula is used::

  (10000 * (Timeout Base + 2)) + (100 * TTL) milliseconds

where TTL is time to live value set in the transfer.

API Reference
***************

This section contains types and defines common to the BLOB Transfer models.

.. doxygengroup:: bt_mesh_blob
   :project: wm-iot-sdk-apis
   :members:
