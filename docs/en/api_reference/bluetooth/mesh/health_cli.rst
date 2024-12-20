.. _bluetooth_mesh_models_health_cli:

Health Client
#############

The Health Client model interacts with a Health Server model to read 
diagnostics and control the attention state of a node.

All message functions in the Health Client API have ``cli`` as
their first parameter. This is a pointer to the client model instance to be
used in this function call. The second parameter is the ``ctx`` or message
context. Message context contains netkey index, appkey index and unicast
address that the target node uses.

The Health Client model is optional and may be instantiated on any element.
However, if a Health Client model is instantiated on an element other than the
primary, an instance must also exist on the primary element.

See :ref:`bluetooth_mesh_health_faults` for a list of specification defined
fault values.

API Reference
*************

.. doxygengroup:: bt_mesh_health_cli
