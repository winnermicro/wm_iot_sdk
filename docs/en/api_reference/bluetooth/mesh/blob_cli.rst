.. _bluetooth_mesh_blob_cli:

BLOB Transfer Client 
####################

The Binary Large Object (BLOB) Transfer Client is the sender of the BLOB transfer. It supports sending BLOBs of any size to any number of destination nodes, 
whether in Push BLOB Transfer Mode or Pull BLOB Transfer Mode.

Usage
*****

Initialization
==============

The BLOB Transfer Client is instantiated on an element with a set of event handler callbacks:

.. code-block:: C

   static const struct bt_mesh_blob_cli_cb blob_cb = {
         /* Callbacks */
   };

   static struct bt_mesh_blob_cli blob_cli = {
         .cb = &blob_cb,
   };

   static const struct bt_mesh_model models[] = {
         BT_MESH_MODEL_BLOB_CLI(&blob_cli),
   };

Transfer Context
================

Both the retrieval process and BLOB transfer utilize an instance of :c:type:`bt_mesh_blob_cli_inputs` 
to determine how to perform the transfer. Before being used in the process, the BLOB Transfer Client input structure must be initialized with at least a target list, 
application key, and Time-To-Live (TTL) value:

.. code-block:: c

   static struct bt_mesh_blob_target targets[3] = {
           { .addr = 0x0001 },
           { .addr = 0x0002 },
           { .addr = 0x0003 },
   };
   static struct bt_mesh_blob_cli_inputs inputs = {
           .app_idx = MY_APP_IDX,
           .ttl = BT_MESH_TTL_DEFAULT,
   };

   sys_slist_init(&inputs.targets);
   sys_slist_append(&inputs.targets, &targets[0].n);
   sys_slist_append(&inputs.targets, &targets[1].n);
   sys_slist_append(&inputs.targets, &targets[2].n);

Note that all BLOB Transfer Servers in the transfer must be bound to the chosen application key.


Logical Address
-----------------

The application may additionally specify a group address in the context structure. If the group is
not :c:macro:`BT_MESH_ADDR_UNASSIGNED`, the messages in the transfer will be sent to the group
address, instead of being sent individually to each Target node. Mesh Manager must ensure that all
Target nodes having the BLOB Transfer Server model subscribe to this group address.

Transmitting BLOBs using a group address can generally increase transfer speed because the BLOB Transfer Client sends each message to all destination nodes simultaneously. 
However, sending large segmented messages to a group address in Bluetooth Mesh is generally less reliable than sending them to unicast addresses because groups lack a transport layer acknowledgment mechanism. 
This can result in longer recovery periods at the end of each block and increase the risk of losing destination nodes. Using a group address for BLOB transfers is usually only rewarding when the list of destination nodes is extensive, 
and the effectiveness of each addressing strategy can vary greatly between different deployments and block sizes.

Transfer Timeout
----------------

If a Target node fails to respond to an acknowledged message within the BLOB Transfer Client's time
limit, the Target node is dropped from the transfer. The application can provide additional time through the context structure to the BLOB Transfer Client, 
reducing this possibility. Besides the base time of 20 seconds, extra time can be set in increments of 10 seconds, up to a maximum of 182 hours. 
The waiting time is automatically adjusted based on the transfer TTL.

Note that the BLOB Transfer Client only moves forward with the transfer in following cases:

* All Target nodes have responded.
* A node has been removed from the list of Target nodes.
* The BLOB Transfer Client times out.

Increasing the wait time will increase this delay.

BLOB Transfer Capabilities Retrieval
====================================

It is generally recommended to retrieve BLOB transfer capabilities before starting a transfer. The
procedure populates the transfer capabilities from all target nodes with the most liberal set of
parameters that allows all Target nodes to participate in the transfer. Any destination nodes that fail
to respond, or respond with incompatible transfer parameters, will be dropped.

Destination nodes are prioritized based on their order in the list of Target nodes. If a destination node
is found to be incompatible with any of the previous destination nodes, for instance by reporting a
non-overlapping block size range, it will be dropped. Lost destination nodes will be reported through the
:c:member:`lost_target <bt_mesh_blob_cli_cb.lost_target>` callback.

The end of the procedure is signalled through the :c:member:`caps <bt_mesh_blob_cli_cb.caps>`
callback, and the resulting capabilities can be used to determine the required block and chunk sizes required
for the BLOB transfer.

BLOB Transfer
=============

The BLOB transfer is started by calling :c:func:`bt_mesh_blob_cli_send` function, which (in addition
to the aforementioned transfer inputs) requires a set of transfer parameters and a BLOB stream
instance. Transfer parameters include the 64-bit BLOB ID, BLOB size, transfer mode, logarithmically represented block size, and number of blocks. 
The BLOB ID is defined by the application but must match the BLOB ID that initiates the BLOB Transfer Server.

The transfer will run until the transfer to at least one destination node is successfully completed or the transfer is canceled. 
The end of the transfer is communicated to the application through the :c:member:`end <bt_mesh_blob_cli_cb.end>` callback. 
Lost Target nodes will be reported through the:c:member:`lost_target <bt_mesh_blob_cli_cb.lost_target>` callback.

API Reference
*************

.. doxygengroup:: bt_mesh_blob_cli
   :project: wm-iot-sdk-apis
   :members:
