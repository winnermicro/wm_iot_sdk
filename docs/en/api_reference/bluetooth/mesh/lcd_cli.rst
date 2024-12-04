.. _bluetooth_mesh_lcd_cli:

Large Composition Data Client
#############################

The Large Composition Data Client model is a foundation model defined by the Bluetooth Mesh
specification. The model is optional, and is enabled through the
``CONFIG_BT_MESH_LARGE_COMP_DATA_CLI`` option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, 
the Large Composite Data Client model supports the functionality of reading composite data pages that do not
fit into the Configuration Composite Data Status message, as well as reading metadata for model instances on nodes 
that support the :ref:`bluetooth_mesh_lcd_srv` model.

The Large Composition Data Client model communicates with the Large Composition Data Server model
using the device key of the node containing the target Large Composition Data Server model instance.

If present, the Large Composition Data Client model must only be instantiated on the primary element.

API Reference
*************

.. doxygengroup:: bt_mesh_large_comp_data_cli
   :project: wm-iot-sdk-apis
   :members:
