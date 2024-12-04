.. _System-AT:
.. |Equipment-Name| replace:: w800

***********************
System AT Command Set
***********************

-  :ref:`AT <cmd-AT>`: Test AT startup
-  :ref:`AT+RST <cmd-AT+RST>`: Reset the module
-  :ref:`AT+RESTORE <cmd-RESTORE>`： Restore factory default settings of the module.
-  :ref:`AT+SYSSTORE <cmd-SYSSTORE>`： Set parameter storage mode

.. _cmd-AT:

:ref:`AT <System-AT>`: System AT Commands
------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT

**Response:**

::

    OK

Explanation
^^^^^^^^^^^^

- This is a test AT startup command, used to check if the device responds correctly. When this command is sent, the device should return `OK`, indicating the device is functioning properly.

Example
^^^^^^^^^^^^

::

    AT

.. _cmd-AT+RST:

:ref:`AT+RST <System-AT>`: Reset the Module
------------------------------------------------------------

Set Command
^^^^^^^^^^^^

**Command:**

::

    AT+RST

**Response:**

::

    OK

Explanation
^^^^^^^^^^^^

- This command is used to reset the device. After sending the `AT+RST` command, the device will reboot.

Example
^^^^^^^^^^^^

::

    AT+RST

.. _cmd-RESTORE:

:ref:`AT+RESTORE <System-AT>`： Restore Factory Default Settings
--------------------------------------------------------------------------------

Execute Command
^^^^^^^^^^^^^^^^

**Command:**

::

   AT+RESTORE

**Response:**

::

    OK

Explanation
^^^^^^^^^^^^

  -  The execution of this command will restore all parameters saved in flash to factory default settings of the module.
  -  The device will be restarted when this command is executed.

.. _cmd-SYSSTORE:

:ref:`AT+SYSSTORE <System-AT>`： Query/Set Parameter Store Mode
---------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^

**Command：**

::

    AT+SYSSTORE?

**Response：**

::

    +SYSSTORE:<store_mode>
    OK

Set Command
^^^^^^^^^^^^

**Response：**

::

    AT+SYSSTORE=<store_mode>

**Response：**

::

    OK  

Parameter
^^^^^^^^^^^
- **<store_mode>**：Parameter storage mode

  - 0：command configuration is not stored into flash.
  - 1：command configuration is stored into flash. (Default)

Explanation
^^^^^^^^^^^^

-  This command affects set commands only. Query commands are always fetched from RAM.
-  This command will affect the following commands:
-  :ref:`AT+CWMODE <cmd-MODE>`
-  :ref:`AT+CIPV6 <cmd-IPV6>`
-  :ref:`AT+CWJAP <cmd-JAP>`
-  :ref:`AT+CWSAP <cmd-SAP>`
-  :ref:`AT+CWRECONNCFG <cmd-RECONNCFG>`
-  :ref:`AT+CIPAP <cmd-IPAP>`
-  :ref:`AT+CIPSTA <cmd-IPSTA>`
-  :ref:`AT+CIPAPMAC <cmd-APMAC>`
-  :ref:`AT+CIPSTAMAC <cmd-STAMAC>`
-  :ref:`AT+CIPDNS <cmd-DNS>`
-  :ref:`AT+CIPSSLCCONF <cmd-SSLCCONF>`
-  :ref:`AT+CIPRECONNINTV <cmd-AUTOCONNINT>`
-  :ref:`AT+CWDHCPS <cmd-DHCPS>`
-  :ref:`AT+CWDHCP <cmd-DHCP>`

Example
^^^^^^^^^^^^

.. code-block:: text

    AT+SYSSTORE=0
    AT+CWMODE=1  // Not stored into flash
    AT+CWJAP="test","1234567890" // Not stored into flash

    AT+SYSSTORE=1
    AT+CWMODE=3  // Stored into flash
    AT+CWJAP="test","1234567890" // Stored into flash