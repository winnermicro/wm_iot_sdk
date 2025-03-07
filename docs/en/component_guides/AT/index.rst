.. _AT:

.. Pending AT command
.. _cmd-UARTD:
.. _cmd-SAVET:
.. .. _cmd-httpurlcfg:
.. _cmd-VETRANSLINK:
.. _cmd-transintvl:
.. _cmd-userota:
.. _using-passive-mode:
.. _cmd-btsppsend:
.. _cmd-blespp:

***************
AT Command Set
***************

This chapter will specifically introduce how to use various AT commands.

.. toctree::
   :maxdepth: 1
  
   System_AT
   Wi-Fi_AT
   TCP-IP_AT
   MQTT_AT
   HTTP_AT
   Bluetooth_AT
   OTA_AT
   FS_AT
   PWM_AT
   SPI_Master_AT
   I2C_AT

It is strongly recommended to read the following content before using commands to understand some basic information about AT commands.

-  :ref:`Classification of AT Commands <cmd-2>` 
-  :ref:`AT Commands with Parameters Stored in Flash <cmd-2>` 
-  :ref:`AT Messages <cmd-3>` 
  
AT commands default to using UART1 with a baud rate of 115200, 8 data bits, 1 stop bit, no parity, and no flow control. It can be selected to use other serial ports in the menuconfig menu. When selecting other serial ports, it is recommended not to mix them with the LOG serial port to avoid interaction interference.
.. _cmd-1: 

:ref:`Classification of AT Commands <AT>`
--------------------------------------------

There are four types of universal AT commands:

.. list-table::
   :header-rows: 1
   :widths: 20 30 55

   * - Type 
     - Command format 
     - Description
   * - Test command
     - AT+<Command Name>=?
     - Queries internal parameters and example command and for setting commands
   * - Query command
     - AT+<Command Name>?
     - Return the current parameter value
   * - Set command
     - AT+<Command Name>=<...> 
     - Set user-defined parameter values and run the commands
   * - Execute command
     - AT+<Command Name>
     - Run commands without user-defined parameters

-  Not every AT command has the four types of commands mentioned above.
-  AT commands are case-sensitive and must be in uppercase letters.
-  Input parameters in the command, currently only string parameters and integer parameters are supported.
-  The parameters within the angle brackets < > cannot be omitted.
-  The parameters within square brackets [ ] can be omitted, and default values will be used when omitted. For example, when running the command :ref:`AT+CWJAP <cmd-JAP>` , some parameters are omitted:
   ::

         AT+CWJAP="WinnerMicro","12345678"
         AT+CWJAP="WinnerMicro","12345678","11:22:33:44:55:66"

-  When there are still parameters to be filled in after omitted parameters, ``,`` must be used to separate them, for example:
   ::
   
      AT+CWJAP="ssid","password",,1

- String parameters are indicated using double quotation marks, such as:
   ::

      AT+CWSAP="WinnerMicro","12345678",1,4

-  Special characters need to be escaped, such as ``,``、 ``"``、 ``\`` , etc.

   -  ``\\``: Escape the back slash.
   -  ``\,``:Escape the commas,the commas separating parameters do not need to be escaped.
   -  ``\"``: Escape the quotation mark.The double quotation marks indicating string parameters do not need to be escaped.
   -  ``\<any>``: Escaping the ``<any>`` character, which means only the ``<any>`` character is used without the backslash.

-  Only special characters in the AT **command** need to be escaped, and others do not. For example, when the AT command port prints ``>`` and waits for input data, the data does not need to be escaped.
   ::
      
      AT+CWJAP="comma\,backslash\\ssid","1234567890"
      AT+MQTTPUB=0,"topic","\"{\"sensor\":012}\"",1,0

-  The length of each AT command should not exceed 256 bytes.
-  AT commands end with a new line (CR-LF), so the serial port tool should be set to "new line mode".
  
.. _cmd-2: 

:ref:`AT Commands with Parameters Stored in Flash <AT>`
---------------------------------------------------------------

The parameters of the following AT commands will always be saved in the NVS area of the flash, so they will be used directly after a restart.

-  :ref:`AT+UART_DEF <cmd-UARTD>`: ``AT+UART_DEF=115200,8,1,0,3``
-  :ref:`AT+CWAUTOCONN <cmd-AUTOC>`: ``AT+CWAUTOCONN=1``

.. Whether parameter changes for some other commands are saved to flash can be configured using the :ref:`AT+SYSSTORE <cmd-SYSSTORE>` command. Please refer to the detailed description of the command.

.. note::
  The parameter saving in AT commands is implemented through the `NVS library <https://#>`_ . Therefore, if a command configures the same parameter value, it will not write to the flash; if a command configures a different parameter value, the flash will not be frequently erased.

.. _at-messages:

.. _cmd-3: 

:ref:`AT Messages <AT>`
--------------------------------------------------

The WinnerMicro-AT messages returned from the WinnerMicro-AT command port are of two types: WinnerMicro-AT responses (passive) and WinnerMicro-AT message reports (active).

.. _at-messages-response:

- WinnerMicro-AT Responses (Passive)
  
  Each input WinnerMicro-AT command returns a response, informing the sender of the execution result of the WinnerMicro-AT command. The last message of the response will inevitably be ``OK`` or ``ERROR``.

  .. centered:: WinnerMicro-AT Responses

  .. list-table::
      :header-rows: 1
      :widths: 40 60

      * - AT Response
        - Description
      * - OK
        - The AT command has been processed and returns OK.
      * - ERROR
        - The AT command is incorrect or an error occurred during execution.
      * - SEND OK
        - Data has been sent to the protocol stack (for the :ref:`AT+CIPSEND <cmd-SEND>` and :ref:`AT+CIPSENDEX <cmd-SENDEX>` commands), but it does not mean the data has been sent to the peer.
      * - SEND FAIL
        - An error occurred while sending data to the protocol stack (for the :ref:`AT+CIPSEND <cmd-SEND>` and :ref:`AT+CIPSENDEX <cmd-SENDEX>` commands).
      * - +<Command Name>:``...``
        - Detailed description of the AT command processing result.
      * - CMD FORMAT ERR
        - The AT command does not exist.
      * - CMD PARSE ERR
        - Command parsing error, generally due to the AT parameters not being input in the required format
	  

.. _at-messages-report:

- WinnerMicro-AT Message Reports (Active)

  WinnerMicro-AT will report important status changes or messages in the system.

  .. centered:: WinnerMicro-AT Message Reports

  .. list-table:: 
     :header-rows: 1
     :widths: 60 60
  
     * - WinnerMicro-AT Message Report
       - Description
     * - busy ...
       - The system is busy processing the previous command and cannot process a new command.
     * - WIFI CONNECTED
       - Wi-Fi station interface has connected to the AP.
     * - WIFI GOT IP
       - The Wi-Fi station interface has obtained an IPv4 address.
     * - WIFI DISCONNECT
       - The Wi-Fi station interface has disconnected from the AP.
     * - [<conn_id>,]CONNECT
       - A network connection with ID ``<conn_id>`` has been established (default ID is 0).
     * - [<conn_id>,]CLOSED
       - A network connection with ID ``<conn_id>`` has been closed (default ID is 0).
     * - +STA_CONNECTED: <sta_mac>
       - A station has connected to the Wi-Fi SoftAP interface of WinnerMicro-AT.
     * - +DIST_STA_IP: <sta_mac>,<sta_ip>
       - The Wi-Fi SoftAP interface of WinnerMicro-AT has assigned an IP address to the station.
     * - +STA_DISCONNECTED: <sta_mac> 
       - The station has disconnected from the Wi-Fi SoftAP interface of WinnerMicro-AT.
     * - +IPD
       - WinnerMicro-AT has received data from the network in non-transparent mode. The message format is as follows:

         - If AT+CIPMUX=0 and AT+CIPRECVMODE=1, it prints: ``+IPD,<length>``
         - If AT+CIPMUX=1 and AT+CIPRECVMODE=1, it prints: ``+IPD,<link_id>,<length>``
         - If AT+CIPMUX=0 and AT+CIPRECVMODE=0 and AT+CIPDINFO=0, it prints: ``+IPD,<length>:<data>``
         - If AT+CIPMUX=1 and AT+CIPRECVMODE=0 and AT+CIPDINFO=0, it prints: ``+IPD,<link_id>,<length>:<data>``
         - If AT+CIPMUX=0 and AT+CIPRECVMODE=0 and AT+CIPDINFO=1, it prints: ``+IPD,<length>,<"remote_ip">,<remote_port>:<data>``
         - If AT+CIPMUX=1 and AT+CIPRECVMODE=0 and AT+CIPDINFO=1, it prints: ``+IPD,<link_id>,<length>,<"remote_ip">,<remote_port>:<data>``

         Where ``link_id`` is the connection ID, ``length`` is the data length, ``remote_ip`` is the remote IP address, ``remote_port`` is the remote port number, and ``data`` is the data.

         Note: When this is an SSL connection, in passive receiving mode (AT+CIPRECVMODE=1), the ``length`` returned by the AT command port may not match the actual readable SSL data length. The AT will first return the readable data length at the SSL layer; if the readable data length at the SSL layer is 0, the AT will return the readable data length at the socket layer.

Customize AT Commands
------------------------------------------------


This chapter will specifically introduce how to carry out AT secondary development.

Command Registration
^^^^^^^^^^^^^^^^^^^^^^^^

::

  typedef struct wm_atcmd_item {
      char *cmd_name;
      wm_atcmd_act cmd_handle;
  #if CONFIG_WM_ATCMD_USAGE_ENABLE
      char *cmd_usg;
  #endif /* CONFIG_WM_ATCMD_USAGE_ENABLE */
  } wm_atcmd_item_t;

  #define WM_ATCMD_SECTION                    ".wm_at_table"

  #if CONFIG_WM_ATCMD_USAGE_ENABLE
  #define WM_ATCMD_DEFINE(cmd, func, usage)                                                                  \
      static const struct wm_atcmd_item __atcmd_##func __attribute__((used, section(WM_ATCMD_SECTION))) =    \
      {                                                                                                      \
          .cmd_name  = cmd,                                                                                  \
          .cmd_handle = func,                                                                                \
          .cmd_usg = usage,                                                                                  \
      };
  #else /* CONFIG_WM_ATCMD_USAGE_ENABLE */
  #define WM_ATCMD_DEFINE(cmd, func, usage)                                                                  \
      static const struct wm_atcmd_item __atcmd_##func __attribute__((used, section(WM_ATCMD_SECTION))) =    \
      {                                                                                                      \
          .cmd_name  = cmd,                                                                                  \
          .cmd_handle = func,                                                                                \
      };
  #endif /* CONFIG_WM_ATCMD_USAGE_ENABLE */

-  cmd_name is a character pointer that can be used to store the name of a command.
-  cmd_handle is a member of type wm_atcmd_act, which is a function pointer that can be used to handle specific commands.
-  cmd_usg is a character pointer used to store help information for AT commands.

Device registration
^^^^^^^^^^^^^^^^^^^^^^^^

::

  typedef struct wm_atcmd_dev {
    wm_atcmd_dev_type_e type;
    wm_dev_ops_t ops;
  } wm_atcmd_dev_t;

::

 typedef enum wm_atcmd_dev_type {
    WM_ATCMD_DEV_TYPE_UART = 0,
    WM_ATCMD_DEV_TYPE_WIFI,
    WM_ATCMD_DEV_TYPE_BT,
    WM_ATCMD_DEV_TYPE_BLE,
    WM_ATCMD_DEV_TYPE_MAX
 } wm_atcmd_dev_type_e;

- In order to register different devices with AT, the above structure includes device types, device sending and receiving capabilities.
- wm_atcmd_dev_type: The type of device used to distinguish between different types of devices.
- wm_dev_ops_t: Operation interface used to define various operations or methods of the device.

Instruction Type
^^^^^^^^^^^^^^^^^^^^^^^

::

   AT_TYPE_QUERY = 0, //?    It is an AT command of inquiry or query type.
   AT_TYPE_TEST  = 1, //=?   It is an AT instruction of testing or verification type.
   AT_TYPE_EXEC  = 2,        It is an AT instruction of execution or operation type.
   AT_TYPE_MAX               Maximum value of enumeration type

AT Return Value
^^^^^^^^^^^^^^^^^^^^^^^^^^

::
         
   WM_AT_RESULT_OK = 0,              /* AT result Ok */  Indicates successful execution of AT instruction, with a value of 0
   WM_AT_RESUT_FAIL,                /* AT result have a generic error */  Indicates AT instruction execution failure, this is a general error
   WM_AT_RESULT_NULL,              /* AT result no string return */   Indicates that the AT command did not return any string result
   WM_AT_RESULT_CMD_ERR,          /* AT command can not find */     Indicates that the AT command cannot be found or does not exist.
   WM_AT_RESULT_PARSE_FAIL,      /* AT command parse  error */     Indicates an AT instruction parsing error.
   AT_RESULT_MAX     List the maximum value

The meunconfig configuration of the AT command
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The main configurations are as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration Name
    - Config Discriptors
    - Default Values

  * - CONFIG_COMPONENT_WM_ATCMD_ENABLED
    - Whether to enable the AT command component
    - N

  * - CONFIG_WM_ATCMD_BT_ENABLE
    - Whether to enable the Bluetooth AT command
    - N

  * - CONFIG_WM_ATCMD_SYS_ENABLE
    - Whether to enable the system AT command
    - Y

  * - CONFIG_WM_ATCMD_TCP_ENABLE
    - Whether to enable the TCP AT command
    - Y

  * - CONFIG_WM_ATCMD_MQTT_ENABLE
    - Whether to enable the MQTT AT command
    - N

  * - CONFIG_WM_ATCMD_HTTP_ENABLE
    - Whether to enable the HTTP AT command
    - Y

  * - CONFIG_WM_ATCMD_OTA_ENABLE
    - Whether to enable the OTA AT command
    - Y

  * - CONFIG_WM_ATCMD_FS_ENABLE 
    - Whether to enable the file system AT command
    - Y  

  * - CONFIG_WM_ATCMD_PWM_ENABLE
    - Whether to enable the PWM AT command
    - Y

  * - CONFIG_WM_ATCMD_I2C_ENABLE
    - Whether to enable the I2C AT command
    - Y

  * - CONFIG_WM_ATCMD_SPIM_ENABLE
    - Whether to enable the SPI Master AT command
    - Y

API Introduction
^^^^^^^^^^^^^^^^^^

1  Initialize AT Command
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

   /**
   * @brief     init atcmd model
   *
   * @param     void  
   *
   * @return
   *    - WM_ERR_SUCCESS: succeed
   *    - others: failed
   */
   int wm_atcmd_init(void);
   
2 Registration of AT Devices
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

   /**
   * @brief     register atcmd dev ops
   *
   * @param     dev_type  device type  
   *            ops       device ops func
   *
   * @return
   *    - WM_ERR_SUCCESS: succeed
   *    - others: failed
   */
   int wm_atcmd_dev_register(wm_atcmd_dev_type_e dev_type, wm_dev_ops_t ops);

3 AT Outputs Print Information to the Terminal
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

   /**
   * @brief     print result
   *
   * @param     format  format str
   *
   * @return    None
   */
  void wm_atcmd_print_result(char *format,...);

4 AT Enters Transparent Transmission
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::
   
   /**
   * @brief     enter passthrough mode
   *
   * @param     pt_tx_handle  passthrough tx func
   *            priv          user private data
   *            pt_exit_notify passthrough exit notify func
   *            timeout       passthrough timeout
   *
   * @return    None
   */
  void wm_atcmd_enter_passthrough(wm_atcmd_tx_passthrough pt_tx_handle, wm_atcmd_tx_passthrough_exit_notify pt_exit_notify, void *priv, uint32_t timeout);

5 AT Exits Transparent Transmission
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

   /**
   * @brief     exit passthrough mode
   *
   * @param     reason  exit reason
   *
   * @return    None
   */
   void wm_atcmd_exit_passthrough(wm_atcmd_exit_passthrough_reason_e reason);

.. _at-demon:

Demo
^^^^^^^^^^^^^^

::

  #include <stdio.h>
  #include <string.h>

  #include "wm_atcmd.h"
  #include "wm_drv_uart.h"

  void uart1_init(void)
  {
      wm_drv_uart_options_s uart_option;

      memset(&uart_option, 0, sizeof(uart_option));
      uart_option.baudrate = WM_DRV_UART_BAUD_RATE_B115200;
      uart_option.data_bits = WM_DRV_UART_DATA_8BITS;
      uart_option.flow_ctrl = WM_DRV_UART_FLOW_CTRL_NONE;
      uart_option.parity_type = WM_DRV_UART_PARITY_DISABLED;
      uart_option.stop_bits = WM_DRV_UART_ONE_STOP_BITS;
      uart_option.event_callback = wm_at_uart_rx_callback;

      wm_drv_uart_init(ATCMD_UART_INDEX, &uart_option);
  }

  static int wm_cmd_test1(wm_at_type_e type, int argc, char **argv)
  {
      if (AT_TYPE_QUERY == type)
      {
          wm_atcmd_print_result("This is used to query the parameters of user-defined AT commands and return the current value.\r\n");

          return WM_AT_RESULT_OK;
      }
      else if (AT_TYPE_EXEC == type)
      {
          /* do some things... */
          wm_atcmd_print_result("This is used to set the parameter values for user-defined AT commands and run this command.\r\n");

          return WM_AT_RESULT_OK;
      }
      else if (AT_TYPE_TEST == type)
      {
          wm_atcmd_print_result("This is used to set the help information for user-defined AT commands.\r\n");

          return WM_AT_RESULT_OK;
      }
      else
      {
          return WM_AT_RESULT_FAIL;
      }
  }

  /* The third parameter of the AT command registration can be filled in as NULL, indicating the use of help information defined by the user in the command; The third parameter can also be set as a string of help information, indicating the help information set using the parameter. */
  WM_ATCMD_DEFINE("AT+TEST1",  wm_cmd_test1,  NULL) 

  int main(void)
  {
      wm_dev_ops_t ops;
      int err = wm_atcmd_init();

      if (!err)
      {
          wm_log_info("atcmd init finished.");
      }
      else
      {
          wm_log_error("atcmd init failed.");
      }

      uart1_init();
      ops.atcmd_recv = atcmd_recv;
      ops.atcmd_send = atcmd_send;
      wm_atcmd_dev_register(WM_ATCMD_DEV_TYPE_UART, ops);

      return 0;
  }
