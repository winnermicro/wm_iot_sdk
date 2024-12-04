.. _bluetooth_mesh_lcd_srv:

Large Composition Data Server
#############################

The Large Composition Data Server model is a foundation model defined by the Bluetooth Mesh
specification. The model is optional, and is enabled through the
``CONFIG_BT_MESH_LARGE_COMP_DATA_SRV`` option.

Introduced in Bluetooth Mesh Protocol Specification version 1.1, the Large Composite Data Server model supports 
the functionality of exposing composite data pages that do not fit into Configuration Composite Data Status messages 
and exposing metadata for model instances.

The Large Composition Data Server does not have an API of its own and relies on a
:ref:`bluetooth_mesh_lcd_cli` to control it.  The model only accepts messages encrypted with the
node's device key.

If present, the Large Composition Data Server model must only be instantiated on the primary
element.

Models Metadata
===============

The Large Composition Data Server model allows each model to have a list of model's specific
metadata that can be read by the Large Composition Data Client model.  The metadata list can be
associated with the :c:struct:`bt_mesh_model` through the :c:member:`bt_mesh_model.metadata` field.
The metadata list consists of one or more entries defined by the
:c:struct:`bt_mesh_models_metadata_entry` structure. Each entry contains the length and ID of the
metadata, and a pointer to the raw data.  Entries can be created using the
:c:macro:`BT_MESH_MODELS_METADATA_ENTRY` macro. The :c:macro:`BT_MESH_MODELS_METADATA_END` macro
marks the end of the metadata list and must always be present. If the model has no metadata, the
helper macro :c:macro:`BT_MESH_MODELS_METADATA_NONE` can be used instead.

API Reference
*************

.. doxygengroup:: bt_mesh_large_comp_data_srv
   :project: wm-iot-sdk-apis
   :members:
