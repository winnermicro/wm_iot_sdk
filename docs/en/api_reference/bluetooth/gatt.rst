.. _bt_gatt:


Generic Attribute Profile (GATT)
################################

The GATT layer manages a service database and provide APIs for service registration
and attribute declaration.

Services can be registered using :c:func:`bt_gatt_service_register` API
which takes a :c:struct:`bt_gatt_service` structure that describes the 
attributes included in the service. The helper macro :c:macro:`BT_GATT_SERVICE()`
can be used to declare a service.

Attributes can be declared using the :c:struct:`bt_gatt_attr` struct or using
one of the helper macros:

    :c:macro:`BT_GATT_PRIMARY_SERVICE`
        Declares a Primary Service.

    :c:macro:`BT_GATT_SECONDARY_SERVICE`
        Declares a Secondary Service.

    :c:macro:`BT_GATT_INCLUDE_SERVICE`
        Declares a Include Service.

    :c:macro:`BT_GATT_CHARACTERISTIC`
        Declares a Characteristic.

    :c:macro:`BT_GATT_DESCRIPTOR`
        Declares a Descriptor.

    :c:macro:`BT_GATT_ATTRIBUTE`
        Declares a Generic Attribute.

    :c:macro:`BT_GATT_CCC`
        Declares a Client Characteristic Configuration.

    :c:macro:`BT_GATT_CEP`
        Declares a Characteristic Extended Properties.

    :c:macro:`BT_GATT_CUD`
        Declares a Characteristic User  Description.

Each attribute contains a ``uuid``, which describes their type, a ``read``
callback, a ``write`` callback and a set of permissions. The read and write 
callbacks are not invoked if the attribute's permissions do not allow the respective operation.

.. note::
   GATT does not support 32-bit UUIDs. All 32-bit UUIDs should be converted to 
   128-bit UUIDs when included in ATT PDUs.

.. note::
  The ``read`` and ``write`` callbacks for attributes are called directly from the RX thread, 
  so it is not advisable to block for long periods within them.

The :c:func:`bt_gatt_notify` API,can be used to notify changes in attribute values, 
or :c:func:`bt_gatt_notify_cb` can be used to pass a callback that is invoked when it is known exactly 
when the data has been transmitted over the air. Indications are supported by
:c:func:`bt_gatt_indicate` API.

Client procedures can be enabled using the configuration option`CONFIG_BT_GATT_CLIENT`

The discovery process can be initiated using the
:c:func:`bt_gatt_discover` API which takes the
:c:struct:`bt_gatt_discover_params` struct which describes the type of
discovery. The parameters also serves as a filter when setting the ``uuid``
field only attributes that match the specified field, while setting it to NULL allows for the discovery of all attributes.

.. note::
  Caching discovered attributes is not supported.

Read procedures are supported by :c:func:`bt_gatt_read` API which takes the
:c:struct:`bt_gatt_read_params` struct as parameters. More attributes can be set in parameter 1, 
although setting multiple handles requires the  ``CONFIG_BT_GATT_READ_MULTIPLE``

Write procedures are supported by :c:func:`bt_gatt_write` API and takes
:c:struct:`bt_gatt_write_params` struct as parameters. In case the write
operation don't require a response :c:func:`bt_gatt_write_without_response`
or :c:func:`bt_gatt_write_without_response_cb` APIs can be used, with the
later working similarly to :c:func:`bt_gatt_notify_cb`.

Subscriptions to notifications and indications can be initiated with using the 
:c:func:`bt_gatt_subscribe` API which takes
:c:struct:`bt_gatt_subscribe_params` as parameters. Multiple subscriptions to
the same attribute are supported , so multiple ``notify`` callbacks
can be triggered for the same attribute.Subscriptions can be removed with using the 
:c:func:`bt_gatt_unsubscribe` API.

.. note::
  When removing a subscription, the ``notify`` callback is called with the data set to NULL.

API Reference
*************

.. doxygengroup:: bt_gatt

GATT Server
===========

.. doxygengroup:: bt_gatt_server

GATT Client
===========

.. doxygengroup:: bt_gatt_client
