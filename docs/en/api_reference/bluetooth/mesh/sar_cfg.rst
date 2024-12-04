.. _bluetooth_mesh_sar_cfg:

Segmentation and Reassembly (SAR)
#################################

Segmentation and Reassembly (SAR) provides a way of handling larger upper transport layer messages
in a mesh network, with a purpose of enhancing the Bluetooth Mesh throughput.
The lower transport layer employs SAR mechanisms for segmenting and reassembling messages.

The lower transport layer defines how to segment upper transport layer PDUs (Protocol Data Units) into multiple lower transport PDUs and transmit them to the lower transport layer on peer devices. 
If the upper transport PDU fits, it will be sent in a single lower transport PDU. For longer packets that do not fit into a single lower transport PDU, the lower transport layer performs segmentation, 
splitting the upper transport PDU into multiple segments.

The lower transport layer on the receiving device reassembles the segments into a single Upper
Transport PDU before passing it up the stack. The delivery of segmented messages is acknowledged 
by the receiving node's lower transport layer, while the delivery of unsegmented messages is not. However, 
when acknowledgment from the lower transport layer is required, an upper transport PDU that fits into one lower transport PDU 
can also be sent as a single-segment segmented message.Set the ``send rel`` flag (see :c:struct:`bt_mesh_msg_ctx`) to use the reliable message
transmission and acknowledge single-segment segmented messages.

The transport layer is able to transport up to 32 segments with its SAR mechanism, with a maximum
message (PDU) size of 384 octets.  To configure message size for the Bluetooth Mesh stack, use the
following Kconfig options:

* ``CONFIG_BT_MESH_RX_SEG_MAX``,which sets the maximum number of segments in incoming messages.
* ``CONFIG_BT_MESH_TX_SEG_MAX``, which sets the maximum number of segments in outgoing messages.

The Kconfig options ``CONFIG_BT_MESH_TX_SEG_MSG_COUNT`` and
``CONFIG_BT_MESH_RX_SEG_MSG_COUNT`` define how many outgoing and incoming segmented
messages can be processed simultaneously. When more than one segmented message is sent to the same
destination, the messages are queued and sent one at a time.

Incoming and outgoing segmented messages share the same pool for allocation of their segments. This
pool size is configured through the ``CONFIG_BT_MESH_SEG_BUFS`` Kconfig option.
Both incoming and outgoing messages allocate segments at the start of the transaction. The outgoing
segmented message releases its segments one by one as soon as they are acknowledged by the receiver,
while the incoming message releases the segments first after the message is fully received.
Keep this in mind when defining the size of the buffers.

SAR does not impose extra overhead on the access layer payload for each segment.

Segmentation and Reassembly (SAR) Configuration Model
******************************************************

According to the Bluetooth Mesh Protocol Specification version 1.1, the SAR configuration model can be used to configure SAR behavior on the mesh network, 
such as intervals, timers, and retransmission counters.:

* :ref:`bluetooth_mesh_sar_cfg_cli`
* :ref:`bluetooth_mesh_sar_cfg_srv`

The following SAR behaviors apply regardless of whether a SAR configuration server is present on the node.

The transmission of segments is separated by segment transmission intervals (see the
`SAR Segment Interval Step`_ state). Other configurable time intervals and delays available for the
segmentation and reassembly are:

* Unicast retransmission interval (see the states `SAR Unicast Retransmissions Interval Step`_ and `SAR Unicast Retransmissions Interval Increment`_).
* Multicast retransmission interval (see the `SAR Multicast Retransmissions Interval Step`_ state).
* Segment reception interval (see the `SAR Receiver Segment Interval Step`_ state).
* Acknowledgment delay increment (see the `SAR Acknowledgment Delay Increment`_ state).

When transmitting the last segment marked as unacknowledged, the lower transport layer starts a
retransmissions timer.  The initial value of the SAR Unicast Retransmissions timer depends on the
value of the TTL field of the message. If the TTL field value is greater than ``0``, the initial
value for the timer is set according to the following formula:

.. math::

   unicast~retransmissions~interval~step + unicast~retransmissions~interval~increment \times (TTL - 1)


If the TTL field value is ``0``, the initial value of the timer is set to the unicast
retransmissions interval step.

The initial value of the SAR multicast retransmissions timer is set to the multicast retransmissions
interval.

When the lower transport layer receives a message segment, it starts a SAR Discard timer. The
Discard timer tells how long the lower transport layer  how long to wait before discarding the segmented 
the segment belongs to. The initial value of the SAR Discard timer is the discard timeout value
indicated by the `SAR Discard Timeout`_ state.

The SAR Acknowledgment timer holds the time before a Segment Acknowledgment message is sent for a
received segment. The initial value of the SAR Acknowledgment timer is calculated using the
following formula:

.. math::

   min(SegN + 0.5 , acknowledgment~delay~increment) \times segment~reception~interval


The ``SegN`` field value identifies the total number of segments for the Upper Transport PDU is
segmentation.

Four counters are related to SAR behavior:

* Two unicast retransmissions counts (see `SAR Unicast Retransmissions Count`_ state and `SAR Unicast Retransmissions Without Progress`_ state)
* Multicast retransmissions count (see `SAR Multicast Retransmissions Count`_ state)
* Acknowledgment retransmissions count (see `SAR Acknowledgment Retransmissions Count`_ state)

If the number of segments in the transmission is higher than the value of the
`SAR Segments Threshold`_ state, Segment 
the value of the `SAR Acknowledgment Retransmissions Count`_ stateis used to retransmit segment acknowledgment messages.

.. _bt_mesh_sar_cfg_states:

SAR States
**********

There are two states defined related to segmentation and reassembly:

* SAR Transmitter state
* SAR Receiver state

The SAR Transmitter state is a composite state that controls the number and timing of transmissions
of segmented messages. It includes the following states:

* SAR Segment Interval Step
* SAR Unicast Retransmission Count
* SAR Unicast Retransmission Count without Progress
* SAR Unicast Retransmission Interval Step
* SAR Unicast Retransmission Interval Increment
* SAR Multicast Retransmission Count
* SAR Multicast Retransmission Interval Step

The SAR Receiver state is a composite state that controls the number and timing of Segment
Acknowledgment transmissions and the discarding of reassembly of a segmented message. It includes
the following states:

* SAR Segments Threshold
* SAR Discard Timeout
* SAR Acknowledgment Delay Increment
* SAR Acknowledgment Retransmissions Count
* SAR Receiver Segment Interval Step
* SAR Receiver Segment Interval Step

SAR Segment Interval Step
=========================

SAR Segment Interval Step state holds a value that controls the interval between transmissions of
segments of a segmented message. The interval is measured in milliseconds.

Set the default value using the ``CONFIG_BT_MESH_SAR_TX_SEG_INT_STEP`` Kconfig option to set the default
value. Segment transmission interval is then calculated using the following formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_SEG\_INT\_STEP} + 1) \times 10~\text{ms}


SAR Unicast Retransmissions Count
=================================

The SAR Unicast Retransmissions Count holds a value that defines the maximum number of retransmissions
of a segmented message to a unicast destination.  Set the default value of this state using the
``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_COUNT`` Kconfig option .
value for this state.

SAR Unicast Retransmissions Without Progress 
==================================================

This state holds a value that defines the maximum number of retransmissions of a segmented message
to a unicast address that will be sent if no acknowledgment was received during the timeout, or if
an acknowledgment with already confirmed segments was received. Use the Kconfig option
``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_WITHOUT_PROG_COUNT`` to set the maximum number
of retransmissions.

SAR Unicast Retransmissions Interval Step
=========================================

The value of this state controls the interval step used for delaying the retransmissions of
unacknowledged segments of a segmented message to a unicast address. The interval step is measured
in milliseconds.

Use the ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_STEP`` Kconfig option to set the
default value.  This value is then used to calculate the interval step using the following formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_UNICAST\_RETRANS\_INT\_STEP} + 1) \times 25~\text{ms}


SAR Unicast Retransmissions Interval Increment
==============================================

The SAR Unicast Retransmissions Interval Increment holds a value that controls the interval increment
used for delaying the retransmissions of unacknowledged segments of a segmented message to a unicast
address. The increment is measured in milliseconds.

Set the default value using the Kconfig option ``CONFIG_BT_MESH_SAR_TX_UNICAST_RETRANS_INT_INC`` to set the
default value.  The Kconfig option value is used to calculate the increment using the following
formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_UNICAST\_RETRANS\_INT\_INC} + 1) \times 25~\text{ms}


SAR Multicast Retransmissions Count
===================================

The state holds a value that controls the total number of retransmissions of a segmented message to
a multicast address. Use the Kconfig option
``CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_COUNT`` to set the total number of
retransmissions.

SAR Multicast Retransmissions Interval Step
===========================================

This state holds a value that controls the interval between retransmissions of all segments in a
segmented message to a multicast address.  The interval is measured in milliseconds.

Use the Kconfig option ``CONFIG_BT_MESH_SAR_TX_MULTICAST_RETRANS_INT`` to set the
default value that is used to calculate the interval using the following formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_TX\_MULTICAST\_RETRANS\_INT} + 1) \times 25~\text{ms}


SAR Discard Timeout
===================

The value of this state defines the time the lower transport layer waits before discarding a segmented message after receiving segments of it, 
measured in seconds. Set the default value using the Kconfig option ``CONFIG_BT_MESH_SAR_RX_DISCARD_TIMEOUT`` to set the default value. 
The discard timeout will be calculated using the following formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_RX\_DISCARD\_TIMEOUT} + 1) \times 5~\text{seconds}


SAR Acknowledgment Delay Increment
==================================

This state holds a value that controls the delay increment for the interval used to delay the transmission of acknowledgment messages after receiving a new segment. 
The increment is measured in segments.

The default value is set using the Kconfig option ``CONFIG_BT_MESH_SAR_RX_ACK_DELAY_INC`` to set the default
value. The increment value is calculated to be
:math:`\verb|CONFIG_BT_MESH_SAR_RX_ACK_DELAY_INC| + 1.5`.

SAR Segments Threshold
======================

SAR Segments Threshold state holds a value that defines a threshold in number of segments of a
segmented message for acknowledgment retransmissions.  Use the Kconfig option
``CONFIG_BT_MESH_SAR_RX_SEG_THRESHOLD`` to set the threshold.

When the number of segments of a segmented message is above this threshold, the stack will
additionally retransmit every acknowledgment message the number of times given by the value of
``CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT``.

SAR Acknowledgment Retransmissions Count
========================================

The SAR Acknowledgment Retransmissions Count state controls the number of retransmissions of Segment
Acknowledgment messages sent by the lower transport layer.  It gives the total number of
retranmissions of an acknowledgment message that the stack will additionally send when the size of
segments in a segmented message is above the ``CONFIG_BT_MESH_SAR_RX_SEG_THRESHOLD``
value.

Use the Kconfig option ``CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT`` to set the default
value for this state.  The maximum number of transmissions of a Segment Acknowledgment message is
:math:`\verb|CONFIG_BT_MESH_SAR_RX_ACK_RETRANS_COUNT| + 1`.

SAR Receiver Segment Interval Step
==================================

The SAR Receiver Segment Interval Step defines the segments reception interval step used for
delaying the transmission of an acknowledgment message after receiving a new segment. The interval
is measured in milliseconds.

Use the Kconfig option ``CONFIG_BT_MESH_SAR_RX_SEG_INT_STEP`` to set the default value
and calculate the interval using the following formula:

.. math::

   (\mathtt{CONFIG\_BT\_MESH\_SAR\_RX\_SEG\_INT\_STEP} + 1) \times 10~\text{ms}
