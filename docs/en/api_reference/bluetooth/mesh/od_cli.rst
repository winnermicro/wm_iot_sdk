.. _bluetooth_mesh_od_cli:

On-Demand Private Proxy Client
##############################

The On-Demand Private Proxy Client model is a foundation model defined by the Bluetooth Mesh
specification. The model is optional, and is enabled with the
``CONFIG_BT_MESH_OD_PRIV_PROXY_CLI`` option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, the On-Demand Private Proxy Client model is used to set and retrieve the On-Demand Private GATT Proxy state. 
This state defines when a node advertises the Mesh Proxy Service with a Private Network Identifier Type after receiving a request PDU.

The On-Demand Private Proxy Client model communicates with the On-Demand Private Proxy Server model
using the device key of the node that contains the target On-Demand Private Proxy Server model instance.

If present, the On-Demand Private Proxy Client model must only be instantiated on the primary
element.

Configuration
**************

The behavior of the On-Demand Private Proxy Client model can be configured using the transmission timeout option ``CONFIG_BT_MESH_OD_PRIV_PROXY_CLI_TIMEOUT``.  
This option controls the time (in milliseconds) that the client waits for a status response message to arrive. The value can be changed at runtime using
:c:func:`bt_mesh_od_priv_proxy_cli_timeout_set`.


API Reference
*************

.. doxygengroup:: bt_mesh_od_priv_proxy_cli
   :project: wm-iot-sdk-apis
   :members:
