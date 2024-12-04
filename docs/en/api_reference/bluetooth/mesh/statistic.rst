.. _bluetooth_mesh_stat:

Frame Statistics
##################

The frame statistic API allows for monitoring the number of received frames over
different interfaces,  as well as the number of planned and succeeded transmission and
relaying attempts.

This API aids users in estimating the efficiency of advertiser configuration parameters and the scanning capabilities of devices. 
The number of monitored parameters can be easily extended through client values.

Applications can read and clear statistics at any time.

API Reference
*************

.. doxygengroup:: bt_mesh_stat
