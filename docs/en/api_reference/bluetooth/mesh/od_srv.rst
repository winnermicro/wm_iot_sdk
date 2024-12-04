.. _bluetooth_mesh_od_srv:

On-Demand Private Proxy Server
##############################

The On-Demand Private Proxy Server model is a foundation model defined by the Bluetooth Mesh
specification. It is enabled with the ``CONFIG_BT_MESH_OD_PRIV_PROXY_SRV`` option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, the On-Demand Private Proxy Server 
model supports configuring the advertisement of a node's Private Network Identifier 
Type as a receiver of request PDUs by managing its On-Demand Private GATT Proxy state.

When enabled, the :ref:`bluetooth_mesh_srpl_srv` is also enabled. The On-Demand Private Proxy Server
is dependent on the :ref:`bluetooth_mesh_models_priv_beacon_srv` to be present on the node.

The On-Demand Private Proxy Server does not have its own API , and relies on 
:ref:`bluetooth_mesh_od_cli` to control it. The On-Demand Private Proxy Server model only accepts
messages encrypted with the node's device key.

If present, the On-Demand Private Proxy Server model must only be instantiated on the primary
element.

API Reference
*************

.. doxygengroup:: bt_mesh_od_priv_proxy_srv
   :project: wm-iot-sdk-apis
   :members:
