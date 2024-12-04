
.. _OTA-AT:
.. |Equipment-Name| replace:: w800

*********************
OTA AT 命令集
*********************


-  :ref:`AT+OTA_HTTP <cmd-OTA_HTTP>`：使用HTTP(S)进行OTA升级

.. _cmd-OTA_HTTP:

:ref:`AT+OTA_HTTP <OTA-AT>`：使用HTTP(S)进行OTA升级
------------------------------------------------------------

设置命令
^^^^^^^^

**命令：**

.. code-block:: text

    AT+OTA_HTTP=<"url">

**响应：**

.. code-block:: text

    OK

    +OTA_HTTP:FINISHED

    or

    +OTA_HTTP:FAILED

参数
^^^^

-  **<"url">**：URL，支持 http URL 和 https URL。

说明
^^^^
1. HTTP 文件服务器搭建可使用以下方法进行快速搭建：

.. code:: sh

    python -m http.server [port]

2. 也可以采用其他方式搭建 HTTP 文件服务器


示例
^^^^

.. code-block:: text

    AT+OTA_HTTP="http://10.17.1.110:8080/atcmd_ota.img"

    AT+OTA_HTTP="https://10.17.1.110:8443/atcmd_ota.img"