.. _bluetooth_mesh_provisioning:

Provisioning
############

Provisioning is the process of adding devices to a mesh network. It requires
two devices operating in the following roles:

* The *provisioner* represents the network owner, and is responsible for
  adding new nodes to the mesh network.
* The *provisionee* is the device that gets added to the network through the
  Provisioning process. Before the provisioning process starts, the
  provisionee is an *unprovisioned device*.

The Provisioning module in the WM IoT SDK Bluetooth Mesh stack supports both the advertiser and GATT bearer 
for the provisionee role and the Advanced Provisioning Bearer for the provisioner role.
 
Provisioning process
************************

All Bluetooth Mesh nodes must be provisioned before they can participate in a
Bluetooth Mesh network. The Provisioning API provides all the functionality
necessary for a device to become a provisioned mesh node.
Provisioning is a five-step process, involving the following steps:

* Beaconing
* Invitation
* Public key Exchange
* Authentication
* Provisioning data transfer

Beaconing
=========

To initiate the provisioning process, an unconfigured device must first start 
advertising an unprovisioned beacon. This makes it visible to nearby provisioners, 
who can initiate provisioning. To indicate that the device needs to be provisioned, 
call :c:func:`bt_mesh_prov_enable`. The device starts broadcasting the Unprovisioned 
Beacon with the device UUID and the``OOB information`` field, as specified in the 
``prov`` parameter passed to:c:func:`bt_mesh_init`. Additionally, a Uniform Resource Identifier (URI)
may be specified, which can point the provisioner to the location of some Out
Of Band information, such as the device's public key or an authentication
value database. The URI is advertised in a separate beacon, with a URI hash
included in the unprovisioned beacon, to tie the two together.


Uniform Resource Identifier
---------------------------

The Uniform Resource Identifier shall follow the format specified in the
Bluetooth Core Specification Supplement. The URI must start with a URI scheme,
encoded as a single utf-8 data point, or the special ``none`` scheme, encoded
as ``0x01``. The available schemes are listed on the `Bluetooth website
<https://www.bluetooth.com/specifications/assigned-numbers/>`_.

Examples of URI encoding :

.. list-table:: URI encoding examples

  * - URI
    - Encoded
  * - ``http://example.com``
    - ``\x16//example.com``
  * - ``https://www.zephyrproject.org/``
    - ``\x17//www.zephyrproject.org/``
  * - ``just a string``
    - ``\x01just a string``

Provisioning Invitation
=======================

The provisioner initiates the Provisioning process by sending a Provisioning
invitation. The invitations prompts the provisionee to call attention to
itself using the Health Server:ref:`bluetooth_mesh_models_health_srv_attention`, if available.

The Unprovisioned device automatically responds to the invite by presenting a
list of its capabilities, including the supported Out of Band Authentication
methods and algorithms.

Public key Exchange
===================

Before the provisioning process can begin, the provisioner and the unprovisioned
device exchange public keys, either in-band or Out of Band (OOB).

In-band public key exchange is a part of the provisioning process and always
supported by the unprovisioned device and provisioner.

If the application wishes to support public key exchange via OOB, it needs to
provide public and private keys to the mesh stack. The unprovisioned device
will reflect this in its capabilities. The provisioner obtains the public key
via any available OOB mechanism (e.g. the device may advertise a packet
containing the public key or it can be encoded in a QR code printed on the
device packaging). Note that even if the unprovisioned device has specified
the public key for the Out of Band exchange, the provisioner may choose to
exchange the public key in-band if it can't retrieve the public key via OOB
mechanism. In this case, a new key pair will be generated by the mesh stack
for each Provisioning process.

To enable support of OOB public key on the unprovisioned device side,
``CONFIG_BT_MESH_PROV_OOB_PUBLIC_KEY`` needs to be enabled. The
application must provide public and private keys before the Provisioning
process is started by initializing pointers to
:c:member:`bt_mesh_prov.public_key_be`
and :c:member:`bt_mesh_prov.private_key_be`. The keys needs to be
provided in big-endian bytes order.

To provide the device's public key obtained via OOB,
call :c:func:`bt_mesh_prov_remote_pub_key_set` on the provisioner side.

Authentication
==============

After the initial exchange, the provisioner selects an Out of Band (OOB)
Authentication method. This allows the user to confirm that the device the
provisioner connected to is actually the device they intended, and not a
malicious third party.

The Provisioning API supports the following authentication methods for the
provisionee:

* **Static OOB:** An authentication value is assigned to the device during 
  production, which the provisioner can query in some application-specific
  way.
* **Input OOB:** The user inputs the authentication value. The available input
  actions are listed in :c:enum:`bt_mesh_input_action_t`.
* **Output OOB:** An authentication value is displayed to the user. The available output
  actions are listed in :c:enum:`bt_mesh_output_action_t`.

The application must provide callbacks for the supported authentication
methods in :c:struct:`bt_mesh_prov`, as well as enabling the supported actions
in :c:member:`bt_mesh_prov.output_actions` and
:c:member:`bt_mesh_prov.input_actions`.

When an Output OOB action is selected, the authentication value should be
presented to the user when the output callback is called, and remain until the
:c:member:`bt_mesh_prov.input_complete` or :c:member:`bt_mesh_prov.complete`
callback is called. If the action is ``blink``, ``beep`` or ``vibrate``, the
sequence should be repeated after a delay of three seconds or more.

When an Input OOB action is selected, the user should be prompted when the
application receives the :c:member:`bt_mesh_prov.input` callback. The user
response should be fed back to the Provisioning API through
:c:func:`bt_mesh_input_string` or :c:func:`bt_mesh_input_number`. If
no user response is recorded within 60 seconds, the Provisioning process is
aborted.

If Provisionee wants to mandate OOB authentication, it must use
the ``BT_MESH_ECDH_P256_HMAC_SHA256_AES_CCM`` algorithm.

Data Transfer
=============

After successfully authenticating the device, the provisioner transfers the provisioning data:

* Unicast address
* A network key
* IV Index
* Network flags

  * Key refresh
  * IV update

Additionally, a device key is generated for the node. All this data is stored
by the mesh stack, and the provisioning :c:member:`bt_mesh_prov.complete`
callback gets called.

Provisioning Security
----------------------

Depending on the choice of public key exchange mechanism and authentication method,
the provisioning process can be secure or insecure.

On May 24th 2021, ANSSI `disclosed <https://kb.cert.org/vuls/id/799380>`_
a set of vulnerabilities in the Bluetooth Mesh provisioning protocol that showcased
how the low entropy provided by the Blink, Vibrate, Push, Twist and
Input/Output numeric OOB methods could be exploited in impersonation and MITM
attacks. In response, the Bluetooth SIG has reclassified these OOB methods as
insecure in the Bluetooth Mesh Profile Specification v1.0.1 `erratum 16350 <https://www.bluetooth.org/docman/handlers/DownloadDoc.ashx?doc_id=516072>`_,
as AuthValue may be brute forced in real time. To ensure secure provisioning, applications
should use a static OOB value and OOB public key transfer.

API Reference
----------------

.. doxygengroup:: bt_mesh_prov
