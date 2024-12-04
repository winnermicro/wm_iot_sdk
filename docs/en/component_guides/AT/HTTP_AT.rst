
.. _HTTP-AT:
.. |Equipment-Name| replace:: W800

*********************
HTTP AT Command Set
*********************


-  :ref:`AT+HTTPCLIENT <cmd-HTTPCLIENT>`: Send HTTP client request
-  :ref:`AT+HTTPGETSIZE <cmd-HTTPGETSIZE>`: Get HTTP resource size
-  :ref:`AT+HTTPCGET <cmd-HTTPCGET>`: Get HTTP resource
-  :ref:`AT+HTTPCPOST <cmd-HTTPCPOST>`: Post HTTP data of specified length 
-  :ref:`AT+HTTPCPUT <cmd-HTTPCPUT>`: Put HTTP data of specified length  
-  :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>`: Set/Get a long HTTP URL
-  :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>`: Set/Get HTTP request headers



.. _cmd-HTTPCLIENT:

:ref:`AT+HTTPCLIENT <HTTP-AT>`: Send HTTP Client Request
------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^

**Command:**

.. code-block:: text

    AT+HTTPCLIENT=<opt>,<content-type>,<"url">,[<"host">],[<"path">],<transport_type>[,<"data">][,<"http_req_header">][,<"http_req_header">][...]

**Response:**

::

    +HTTPCLIENT:<size>,<data>

    OK

Parameters
^^^^^^^^^^^^

-  **<opt>**: HTTP client request method:
   
   -  1: HEAD
   -  2: GET
   -  3: POST
   -  4: PUT
   -  5: DELETE

-  **<content-type>**: Client request data type:

   -  0：``application/x-www-form-urlencoded``
   -  1：``application/json``
   -  2：``multipart/form-data``
   -  3：``text/xml``

-  **<"url">**: HTTP URL. If the subsequent ``<host>`` and ``<path>`` parameters are empty, this parameter will automatically override them.
-  **<"host">**: Domain name or IP address.
-  **<"path">**: HTTP path.。
-  **<transport_type>**: HTTP client transport type, default value is 1:

   -  1：``HTTP_TRANSPORT_OVER_TCP``
   -  2：``HTTP_TRANSPORT_OVER_SSL``

-  **<"data">**: When ``<opt>`` is a POST request, this parameter is the data sent to the HTTP server. If ``<opt>`` is not a POST request, this parameter does not exist (i.e., you do not need to input a comma to indicate this parameter).
-  **<"http_req_header">**: Multiple request headers can be sent to the server.

Description
^^^^^^^^^^^^
-  If the entire URL Command exceeds 256 bytes, use the :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>` command to pre-configure the URL, then set the ``<"url">`` parameter in this command to ``""``.
-  The ``url`` parameter cannot be empty, the HTTP client can use the ``host`` and ``path`` parameters, but they will not be effective.
-  The command does not support URL redirection. After getting the status code 301 (permanent redirection) or 302 (temporary redirection) from the server, You can use certain tools to obtain the actual URL to visit and then access it using this command.
-  If the entire command, including the ``<"data">`` parameter, exceeds 256 bytes, please use :ref:`AT+HTTPCPOST <cmd-HTTPCPOST>` command.
-  To set more HTTP request headers, use the :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>` command.

Example
^^^^^^^^^^^^

::

    // HEAD request
    AT+HTTPCLIENT=1,0,"http://httpbin.org/get","httpbin.org","/get",1

    // GET request
    AT+HTTPCLIENT=2,0,"http://httpbin.org/get","httpbin.org","/get",1

    // POST request
    AT+HTTPCLIENT=3,0,"http://httpbin.org/post","httpbin.org","/post",1,"field1=value1&field2=value2"


.. _cmd-HTTPGETSIZE:

:ref:`AT+HTTPGETSIZE <HTTP-AT>`: Get HTTP Resource Size
------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPGETSIZE=<"url">[,<tx size>][,<rx size>][,<timeout>]

**Response:**

::

    +HTTPGETSIZE:<size>

    OK

Parameters
^^^^^^^^^^^^
- **<"url">**: HTTP URL.
- **<tx size>**: HTTP send buffer size. Unit: bytes. Default: 2048. Range: [0,10240].
- **<rx size>**: HTTP receive buffer size. Unit: bytes. Default: 2048. Range: [0,10240].
- **<timeout>**: Network timeout. Unit: milliseconds. Default: 5000. Range: [0,180000].
- **<size>**: HTTP resource size.

Description
^^^^^^^^^^^^^^^^^^^^^^^^

-  If the entire command, including the URL, exceeds 256 bytes, use the :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>`  command to pre-configure the URL, then set the ``<"url">`` parameter in this command to ``""``.
-  To set HTTP request headers, use the :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>` command.

Example
^^^^^^^^^^^^

::

    AT+HTTPGETSIZE="http://www.baidu.com/img/bdlogo.gif"

.. _cmd-HTTPCGET:

:ref:`AT+HTTPCGET <HTTP-AT>`: Get HTTP Resource
----------------------------------------------------

Set Command
^^^^^^^^^^^

**Command:**

::

    AT+HTTPCGET=<"url">[,<tx size>][,<rx size>][,<timeout>]

**Response:**

::

    +HTTPCGET:<size>,<data>
    OK

Parameters
^^^^^^^^^^^^^^^^^^
- **<"url">**: HTTP URL.。
- **<tx size>**: HTTP send buffer size. Unit: bytes. Default: 2048. Range: [0,10240].
- **<rx size>**: HTTP receive buffer size. Unit: bytes. Default: 2048. Range: [0,10240].
- **<timeout>**: Network timeout. Unit: milliseconds. Default: 5000. Range: [0,180000].

Description
^^^^^^^^^^^^^^^

-  If the entire command, including the URL, exceeds 256 bytes, use the :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>` command to pre-configure the URL, then set the ``<"url">`` parameter in this command to ``""``.
-  To set HTTP request headers, use the :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>` command.

.. _cmd-HTTPCPOST:

:ref:`AT+HTTPCPOST <HTTP-AT>`: Post HTTP Data of Specified Length 
------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPCPOST=<"url">,<length>[,<http_req_header_cnt>][,<http_req_header>..<http_req_header>]

**Response:**

::

    OK

    >

The symbol ``>`` indicates that AT is ready to receive serial data. You can now enter data, and when the data length reaches the value of the ``<length>`` parameter, the transmission starts.

If the transmission is successful, it returns:

::

    SEND OK

If the transmission fails, it returns:

::

    SEND FAIL

Parameters
^^^^^^^^^^^^

- **<"url">**: HTTP URL.
- **<length>**: Length of the HTTP data to be POSTed. The maximum length is equal to the system's allocatable heap space size.
- **<http_req_header_cnt>**: Number of ``<http_req_header>`` parameters.
- **[<http_req_header>]**: HTTP request headers. Multiple headers can be sent to the server.

Description
^^^^^^^^^^^^

-  If the entire command, including the URL, exceeds 256 bytes, use the :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>` command to pre-configure the URL, then set the ``<"url">`` parameter in this command to ``""``.
-  The default type of ``content-type`` for this command is ``application/x-www-form-urlencoded``.
-  To set HTTP request headers, use the :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>` command.

.. _cmd-HTTPCPUT:

:ref:`AT+HTTPCPUT <HTTP-AT>`: Put HTTP data of specified length
-------------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPCPUT=<"url">,<length>[,<http_req_header_cnt>][,<http_req_header>..<http_req_header>]

**Response:**

::

    OK

    >

The symbol ``>`` indicates that AT is ready to receive serial data. You can now enter data, and when the data length reaches the value of the ``<length>`` parameter, the transmission starts.

If the transmission is successful, it returns:

::

    SEND OK

If the transmission fails, it returns:

::

    SEND FAIL

Parameters
^^^^^^^^^^^^^^^^^
- **<"url">**: HTTP URL.
- **<length>**: Length of the HTTP data to be PUT. The maximum length is equal to the system's allocatable heap space size.
- **<http_req_header_cnt>**: Number of ``<http_req_header>`` parameters.
- **[<http_req_header>]**: HTTP request headers. Multiple headers can be sent to the server.

Description
^^^^^^^^^^^^^^^^^

-  If the entire command, including the URL, exceeds 256 bytes, use the :ref:`AT+HTTPURLCFG <cmd-HTTPURLCFG>` command to pre-configure the URL, then set the ``<"url">`` parameter in this command to ``""``.
-  To set HTTP request headers, use the :ref:`AT+HTTPCHEAD <cmd-HTTPCHEAD>` command.

.. _cmd-HTTPURLCFG:

:ref:`AT+HTTPURLCFG <HTTP-AT>`: Set/Get long HTTP URL
-------------------------------------------------------------------

Query Command
^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPURLCFG?

**Response:**

::

    [+HTTPURLCFG:<url length>,<data>]
    OK

Set Command
^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPURLCFG=<url length>

**Response:**

::

    OK

    >

The symbol > indicates that the AT is ready to receive serial data. At this time, you can enter the URL. When the data length reaches the value of the parameter ``<url length>`` , the system returns

::

    SET OK

Parameters
^^^^^^^^^^^^^^^^^^
- **<url length>**: HTTP URL length. Unit: Byte.

  - 0: Clear HTTP URL configuration.
  - [8,8192]: Set HTTP URL configuration.

- **<data>** HTTP URL data.

.. _cmd-HTTPCHEAD:

:ref:`AT+HTTPCHEAD <HTTP-AT>`: Set/Get HTTP Request Headers
------------------------------------------------------------------

Set Command
^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPCHEAD?

**Response:**

::

    +HTTPCHEAD:<index>,<"req_header">

    OK

Set Command
^^^^^^^^^^^^^^^^^^^^^^

**Command:**

::

    AT+HTTPCHEAD=<req_header_len>

**Response:**

::

    OK

    >

The symbol ``>`` indicates that the AT is ready to receive data from the AT command port. At this point, you can input an HTTP request header (in the form of  ``key: value`` ). When the data length reaches the value of the parameter ``<req_header_len>`` , the AT returns:

::

    OK

Parameters
^^^^^^^^^^^^^^^^^^
- **<index>**: The index value of the HTTP request header.
- **<"req_header">**: HTTP request header.
- **<req_header_len>**: HTTP request header length. Unit: Byte.

  - 0: Clear all set HTTP request headers.
  - Other values: Set a new HTTP request header.

Description
^^^^^^^^^^^^^^^

- This command can only set one HTTP request header at a time, but it can be set multiple times and supports multiple different HTTP request headers, up to a maximum of 20.
- The HTTP request headers configured in this command are global, and once set, all HTTP commands will carry these request headers.
- If the ``key`` in the HTTP request header set by this command is the same as the  ``key`` in the request headers of other HTTP commands, the HTTP request header set in this command will be used.

Example
^^^^^^^^^^^^^^^

::

    // Set request header
    AT+HTTPCHEAD=18

    // After receiving the ">" symbol, enter the following Range request header to download the first 256 bytes of the resource.
    Range: bytes=0-255

    // Download HTTP resources
    AT+HTTPCGET="https://www.winnermicro.com/html/1/156/158/558.html"
