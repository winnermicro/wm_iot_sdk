
.. GENERATED by help2rst.py.  DO NOT EDIT DIRECTLY.

.. program:: nghttpd

nghttpd(1)
==========

SYNOPSIS
--------

**nghttpd** [OPTION]... <PORT> [<PRIVATE_KEY> <CERT>]

DESCRIPTION
-----------

HTTP/2 server

.. describe:: <PORT>

    Specify listening port number.

.. describe:: <PRIVATE_KEY>

    
    Set  path  to  server's private  key.   Required  unless
    :option:`--no-tls` is specified.

.. describe:: <CERT>

    Set  path  to  server's  certificate.   Required  unless
    :option:`--no-tls` is specified.

OPTIONS
-------

.. option:: -a, --address=<ADDR>

    The address to bind to.  If not specified the default IP
    address determined by getaddrinfo is used.

.. option:: -D, --daemon

    Run in a background.  If :option:`-D` is used, the current working
    directory is  changed to '*/*'.  Therefore  if this option
    is used, :option:`-d` option must be specified.

.. option:: -V, --verify-client

    The server  sends a client certificate  request.  If the
    client did  not return  a certificate, the  handshake is
    terminated.   Currently,  this  option just  requests  a
    client certificate and does not verify it.

.. option:: -d, --htdocs=<PATH>

    Specify document root.  If this option is not specified,
    the document root is the current working directory.

.. option:: -v, --verbose

    Print debug information  such as reception/ transmission
    of frames and name/value pairs.

.. option:: --no-tls

    Disable SSL/TLS.

.. option:: -c, --header-table-size=<SIZE>

    Specify decoder header table size.

.. option:: --encoder-header-table-size=<SIZE>

    Specify encoder header table size.  The decoder (client)
    specifies  the maximum  dynamic table  size it  accepts.
    Then the negotiated dynamic table size is the minimum of
    this option value and the value which client specified.

.. option:: --color

    Force colored log output.

.. option:: -p, --push=<PATH>=<PUSH_PATH,...>

    Push  resources <PUSH_PATH>s  when <PATH>  is requested.
    This option  can be used repeatedly  to specify multiple
    push  configurations.    <PATH>  and   <PUSH_PATH>s  are
    relative  to   document  root.   See   :option:`--htdocs`  option.
    Example: :option:`-p`\/=/foo.png :option:`-p`\/doc=/bar.css

.. option:: -b, --padding=<N>

    Add at  most <N>  bytes to a  frame payload  as padding.
    Specify 0 to disable padding.

.. option:: -m, --max-concurrent-streams=<N>

    Set the maximum number of  the concurrent streams in one
    HTTP/2 session.

    Default: ``100``

.. option:: -n, --workers=<N>

    Set the number of worker threads.

    Default: ``1``

.. option:: -e, --error-gzip

    Make error response gzipped.

.. option:: -w, --window-bits=<N>

    Sets the stream level initial window size to 2\*\*<N>-1.

.. option:: -W, --connection-window-bits=<N>

    Sets  the  connection  level   initial  window  size  to
    2\*\*<N>-1.

.. option:: --dh-param-file=<PATH>

    Path to file that contains  DH parameters in PEM format.
    Without  this   option,  DHE   cipher  suites   are  not
    available.

.. option:: --early-response

    Start sending response when request HEADERS is received,
    rather than complete request is received.

.. option:: --trailer=<HEADER>

    Add a trailer  header to a response.   <HEADER> must not
    include pseudo header field  (header field name starting
    with ':').  The  trailer is sent only if  a response has
    body part.  Example: :option:`--trailer` 'foo: bar'.

.. option:: --hexdump

    Display the  incoming traffic in  hexadecimal (Canonical
    hex+ASCII display).  If SSL/TLS  is used, decrypted data
    are used.

.. option:: --echo-upload

    Send back uploaded content if method is POST or PUT.

.. option:: --mime-types-file=<PATH>

    Path  to file  that contains  MIME media  types and  the
    extensions that represent them.

    Default: ``/etc/mime.types``

.. option:: --no-content-length

    Don't send content-length header field.

.. option:: --ktls

    Enable ktls.

.. option:: --no-rfc7540-pri

    Disable RFC7540 priorities.

.. option:: --version

    Display version information and exit.

.. option:: -h, --help

    Display this help and exit.



The <SIZE> argument is an integer and an optional unit (e.g., 10K is
10 * 1024).  Units are K, M and G (powers of 1024).

SEE ALSO
--------

:manpage:`nghttp(1)`, :manpage:`nghttpx(1)`, :manpage:`h2load(1)`
