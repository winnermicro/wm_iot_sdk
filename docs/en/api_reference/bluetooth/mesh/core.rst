.. _bluetooth_mesh_core:

Core
####

The core provides functionality for managing the state of a generic Bluetooth Mesh.

.. _bluetooth_mesh_lpn:

Low-Power Node
**************

The Low Power Node (LPN) role allows battery-powered devices to participate in
a mesh network as leaf nodes. LPNs interact with the mesh network through
Friend nodes, which relay any messages directed to the LPN. LPNs save power 
by keeping their radios off and only waking up to send messages or poll Friend Nodes 
for any incoming messages.

The radio control and polling are managed automatically by the mesh stack, but
the LPN API allows the applications to trigger polling at any time through the function
:c:func:`bt_mesh_lpn_poll`. The LPN operation parameters, including polling
intervals, polling event timing, and Friend requirements, are controlled through the
``CONFIG_BT_MESH_LOW_POWER`` option and related configuration options.

When using the LPN feature with logging, it is strongly recommended to only use
the ``CONFIG_LOG_MODE_DEFERRED`` option. Log modes other than the
Deferred may cause unexpected delays during processing of log messages. 
This will affect receive delays and the scheduling of receive windows. 
The same limitation applies to the ``CONFIG_BT_MESH_FRIEND`` option.

Replay Protection List
**********************

The Replay Protection List (RPL) is used to store the sequence numbers of messages 
recently received from elements in the mesh network to perform protection against
replay attacks6n.

To protect the node from replay attacks after a restart, it needs to store the entire RPL in persistent storage before power is lost. 
Depending on the traffic in the mesh network, storing the recently seen sequence numbers can wear out flash memory sooner or later.
To mitigate this,the :c:macro:``CONFIG_BT_MESH_RPL_STORE_TIMEOUT`` can be used. This option postpones
storing of RPL entries in the persistent storage.

However, this option does not completely solve the problem, as the node may shut down before 
the timer that triggers the storage of the RPL is reached. To ensure that messages cannot be 
replayed, the node can initiate the storage of pending RPL entries at any time (or sufficient 
time before power is lost) by calling :c:func:`bt_mesh_rpl_pending_store`. This is up to the node to decide,
which RPL entries are to be stored in this case.

Setting :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT` to -1 allows completely
turning off the timer, which helps significantly reduce flash wear. 
This shifts the responsibility of storing the RPL to the user application and requires enough 
power backup from the time the API is called until all RPL entries are written to flash memory.

Finding the right balance between :c:macro:`CONFIG_BT_MESH_RPL_STORE_TIMEOUT`  and
calling :c:func:`bt_mesh_rpl_pending_store` may reduce a risk of security
vulnerability and flash wear out.

.. warning:

   Failing to enable ``CONFIG_BT_SETTINGS``, or setting
   :c:macro:``CONFIG_BT_MESH_RPL_STORE_TIMEOUT`` to -1 without storing
   the RPL between reboots, will make the device susceptible to replay attacks
   and will not perform the replay protection required by the specification.

.. _bluetooth_mesh_persistent_storage:

Persistent storage
******************

The mesh stack uses the :func:`Settings Subsystem <settings_api>` for storing the
device configuration persistently. When a stack configuration changes and
the change needs to be stored persistently, the stack schedules a work item.
The delay between scheduling the work item and submitting it to the work queue
is defined by the :c:macro: ``CONFIG_BT_MESH_STORE_TIMEOUT`` option. Once
the storage of data is scheduled, it can not be rescheduled until the work item is
processed. Exceptions are made in certain cases as described below.

When it is necessary to store the IV Index, Sequence Number, or CDB configuration, the work
item is submitted to the work queue without the delay. If a work item was
previously scheduled, it will be rescheduled without the delay.

The Replay Protection List uses the same work item to store RPL entries. If storing an RPL entry 
is requested and there are no other configurations pending storage, 
the delay is set by the  :c:macro:``CONFIG_BT_MESH_RPL_STORE_TIMEOUT``.
If other stack configuration has to be stored, the delay defined by
the :c:macro:``CONFIG_BT_MESH_STORE_TIMEOUT`` option is less than
:c:macro:``CONFIG_BT_MESH_RPL_STORE_TIMEOUT``, and the work item was
scheduled by the Replay Protection List, the work item will be rescheduled.

When the work item is being executed, the stack will store all pending configuration,
including the RPL entries.

Work Item Execution Context
===========================

The:c:macro:``CONFIG_BT_MESH_SETTINGS_WORKQ`` option configures the context from which work items are 
executed. This option is enabled by default and results in a dedicated cooperative thread being used for 
handling stack work items. This allows the stack to process other incoming and outgoing messages and work 
items submitted to the system work queue while storing stack configurations.

When this option is disabled, the work item is submitted to the system workqueue.
This means that the system workqueue is blocked for the time it takes to store
the stack's configuration. It is not recommended to disable this option as this
will make the device non-responsive for a noticeable amount of time.

.. _bluetooth_mesh_adv_identity:

Advertisement identity
**********************

All mesh stack bearers advertise data with the :c:macro:`BT_ID_DEFAULT` local identity to advertise data.
The value is preset in the mesh stack implementation. When Bluetooth® Low Energy (LE)
and Bluetooth Mesh coexist on the same device, the application should allocate and
configure another local identity for Bluetooth LE purposes before starting the communication.

API Reference
**************

.. doxygengroup:: bt_mesh
