.. _bluetooth_connection_mgmt:

Connection Management
#####################

The WM IoT SDK Bluetooth stack uses an abstraction called :c:struct:`bt_conn`
to represent connections to other devices. The internals of this struct
are not exposed to the application, but a limited amount of information
(such as the remote address) can be acquired using the
:func:`bt_conn_get_info` API. Connection objects are reference
counted, and the application is expected to use the
:func:`bt_conn_ref` API whenever  it stores a connection pointer for  an extended 
period of time, since this ensures that the object remains valid
(even if the connection would get disconnected). Similarly the
:func:`bt_conn_unref` API is to be used when releasing a reference
to a connection.

An application may track connections by registering a
:c:struct:`bt_conn_cb` struct using the :func:`bt_conn_cb_register`
or :c:macro:`BT_CONN_CB_DEFINE` APIs. This struct allows the application
to define callbacks for connection and disconnection events, as well as other
events related to a connection such as a changes in the security level or
connection parameters. When acting as a central device, the application will
also obtain the connection object through the return value of the
:func:`bt_conn_le_create` API.

API Reference
*************

.. doxygengroup:: bt_conn
