.. _bt_mesh_proxy:

Proxy
#####

The Proxy feature allows legacy devices such as phones to access the Bluetooth Mesh network through
GATT. The Proxy  functionality is compiled into the system only when the ``CONFIG_BT_MESH_GATT_PROXY``
option is set. The Proxy feature state is controlled by the :ref:`bluetooth_mesh_models_cfg_srv`,
and its initial value can be set with :c:member:`bt_mesh_cfg_srv.gatt_proxy`.

Nodes with the Proxy feature enabled can advertise using Network Identity and Node Identity,
which is controlled by the :ref:`bluetooth_mesh_models_cfg_cli`.

The GATT Proxy state indicates whether the Proxy feature is supported.

Dedicated Proxy Server
*************************

Nodes that support the proxy feature and the :func:`blueooth_mesh_models_priv_beacon_srv` model can
advertise using dedicated network and node identifier types, controlled by :func:`blueooth _mesh_models_prif_beacon_cli` . By advertising with this set of identification types,
the node allows the legacy device to connect to the network over GATT while maintaining the
privacy of the network.

The Private GATT Proxy state indicates whether the Private Proxy functionality is supported.

Proxy Solicitation
******************

When GATT Proxy and Dedicated GATT Proxy states are disabled on a node, traditional devices cannot connect to it. 
However, nodes supporting the :ref:`bluetooth_mesh_od_srv` can be requested to advertise connectable advertising events without enabling the Dedicated GATT Proxy state. 
To request a node, traditional devices can send a Solicitation PDU by calling the 
:func:`bt_mesh_proxy_solicit` function.  To enable this feature, the device must to be compiled with
the ``CONFIG_BT_MESH_PROXY_SOLICITATION`` option set.

The Solicitation PDU is a non-mesh, non-connectable, undirected advertising message containing the Proxy Solicitation UUID,
encrypted using the network key of the subnet that the traditional device intends to connect to. 
The PDU includes the source address and sequence number of the traditional device. 
The sequence number is maintained by the traditional device and increments for each new Solicitation PDU sent.

Each node supporting the receipt of Solicitation PDUs maintains its own Solicitation Replay Protection List (SRPL). 
The SRPL protects the Solicitation mechanism from replay attacks by storing pairs of Solicitation Sequence Numbers (SSEQ) 
and Solicitation Source Addresses (SSRC) for valid Solicitation PDUs processed by the node. 
The delay between updating the SRPL and storing changes to persistent storage is defined by``CONFIG_BT_MESH_RPL_STORE_TIMEOUT``.

The Solicitation PDU RPL Configuration models, :ref:`bluetooth_mesh_srpl_cli` and
:ref:`bluetooth_mesh_srpl_srv`, provide the functionality of saving and clearing SRPL entries.  A
node that supports the Solicitation PDU RPL Configuration Client model can clear a section of the
SRPL on the target by calling the :func:`bt_mesh_sol_pdu_rpl_clear` function.  Communication between
the Solicitation PDU RPL Configuration Client and Server is encrypted using the application key,
therefore, the Solicitation PDU RPL Configuration Client can be instantiated on any device in the
network.

When the node receives the Solicitation PDU and successfully authenticates it, it will start
advertising connectable advertisements with the Private Network Identity type. The duration of the
advertisement can be configured by the On-Demand Private Proxy Client model.

API Reference
*************

.. doxygengroup:: bt_mesh_proxy
