
.. _OTA-AT:
.. |Equipment-Name| replace:: w800

*************************
OTA AT Command Set
*************************


-  :ref:`AT+OTA_HTTP <cmd-OTA_HTTP>`: Perform OTA upgrade using HTTP(S)

.. _cmd-OTA_HTTP:

:ref:`AT+OTA_HTTP <OTA-AT>`: Perform OTA upgrade using HTTP(S)
----------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^

**Command:**

::

    AT+OTA_HTTP=<"url">

**Response:**

::

    OK

    +OTA_HTTP:FINISHED

    or

    +OTA_HTTP:FAILED

Parameters
^^^^^^^^^^^^

-  **<"url">**: URL, supports http URL and https URL.

Description
^^^^^^^^^^^^
1. You can quickly set up an HTTP file server using the following method:

.. code:: sh

    python -m http.server [port]

2. Alternatively, you can use other methods to set up an HTTP file server.


Example
^^^^^^^^^^^^

::

    AT+OTA_HTTP="http://10.17.1.110:8080/atcmd_ota.img"

    AT+OTA_HTTP="https://10.17.1.110:8443/atcmd_ota.img"
