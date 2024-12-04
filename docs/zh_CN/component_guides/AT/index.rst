.. _AT:

.. 待做AT命令
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
AT 命令集
***************

本章将具体介绍如何使用各类 AT 命令。

.. toctree::
   :maxdepth: 1
  
   System_AT
   Wi-Fi_AT
   TCP-IP_AT
   HTTP_AT
   OTA_AT
   Bluetooth_AT
   FS_AT
   PWM_AT
   I2C_AT
   SPI_Master_AT
   MQTT_AT

强烈建议在使用命令之前先阅读以下内容，了解 AT 命令的一些基本信息。 

-  :ref:`AT命令分类 <cmd-1>` 
-  :ref:`参数信息保存在 flash 中的 AT 命令 <cmd-2>` 
-  :ref:`AT 消息 <cmd-3>` 

AT 命令默认使用 UART1，波特率 115200，数据位 8位，停止位 1位，无奇偶校验，无流控。
可以在 menuconfig 菜单中自行选择为其它串口，选择其它串口时建议不要和 LOG 串口混用，避免交互干扰。

.. _cmd-1: 

:ref:`AT命令分类 <AT>`
-------------------------------

通用 AT 命令有四种类型：

.. list-table::
   :header-rows: 1
   :widths: 20 30 55

   * - 类型 
     - 命令格式 
     - 说明
   * - 测试命令
     - AT+<命令名称>=?
     - 查询设置命令的内部参数及其示例命令
   * - 查询命令
     - AT+<命令名称>?
     - 返回当前参数值
   * - 设置命令
     - AT+<命令名称>=<...> 
     - 设置用户自定义的参数值，并运行命令
   * - 执行命令
     - AT+<命令名称>
     - 运行无用户自定义参数的命令

-  不是每条 AT 命令都具备上述四种类型的命令。
-  AT 命令区分大小写，必须为大写字母。
-  命令里输入参数，当前只支持字符串参数和整形数字参数。
-  尖括号 < > 内的参数不可以省略。
-  方括号 [ ] 内的参数可以省略，省略时使用默认值。例如，运行 :ref:`AT+CWJAP <cmd-JAP>` 命令时省略某些参数：

.. code-block:: text

   AT+CWJAP="WinnerMicro","12345678"
   AT+CWJAP="WinnerMicro","12345678","11:22:33:44:55:66"

-  当省略的参数后仍有参数要填写时，必须使用 ``,``，以示分隔，如：

.. code-block:: text

   AT+CWJAP="ssid","password",,1

-  使用双引号表示字符串参数，如：

.. code-block:: text

   AT+CWSAP="WinnerMicro","12345678",1,4

-  特殊字符需作转义处理，如 ``,``、``"``、``\`` 等。

   -  ``\\``：转义反斜杠。
   -  ``\,``：转义逗号，分隔参数的逗号无需转义。
   -  ``\"``：转义双引号，表示字符串参数的双引号无需转义。
   -  ``\<any>``：转义 ``<any>`` 字符，即只使用 ``<any>`` 字符，不使用反斜杠。

-  只有 AT **命令** 中的特殊字符需要转义，其它地方无需转义。例如，AT 命令口打印 ``>`` 等待输入数据时，该数据不需要转义。

.. code-block:: text

   AT+CWJAP="comma\,backslash\\ssid","1234567890"
   AT+MQTTPUB=0,"topic","\"{\"sensor\":012}\"",1,0

-  每条 AT 命令的长度不应超过 256 字节。
-  AT 命令以新行 (CR-LF) 结束，所以串口工具应设置为“新行模式”。
  
.. _cmd-2: 

:ref:`参数信息保存在 flash 中的 AT 命令 <AT>`
------------------------------------------------------------

以下 AT 命令的参数更改将始终保存在 flash 的 NVS 区域中，因此重启后，会直接使用。

-  :ref:`AT+UART_DEF <cmd-UARTD>`: ``AT+UART_DEF=115200,8,1,0,3``
-  :ref:`AT+CWAUTOCONN <cmd-AUTOC>`: ``AT+CWAUTOCONN=1``

.. 其它一些命令的参数更改是否保存到 flash 可以通过 :ref:`AT+SYSSTORE <cmd-SYSSTORE>` 命令来配置，具体请参见命令的详细说明。

.. note::
  AT 命令里的参数保存，是通过 :ref:`NVS <nvs>` 库实现的。因此，如果命令配置相同的参数值，则不会写入 flash；如果命令配置不同的参数值，flash 也不会被频繁擦除。

.. _at-messages:

.. _cmd-3: 

:ref:`AT消息 <AT>`
--------------------------------------------------

从 WinnerMicro-AT 命令端口返回的 WinnerMicro-AT 消息有两种类型：WinnerMicro-AT 响应（被动）和 WinnerMicro-AT 消息报告（主动）。

.. _at-messages-response:

- WinnerMicro-AT 响应（被动）
  
  每个输入的 WinnerMicro-AT 命令都会返回响应，告诉发送者 WinnerMicro-AT 命令的执行结果。响应的最后一条消息必然是 ``OK`` 或者 ``ERROR``。

  .. centered:: WinnerMicro-AT 响应

  .. list-table::
      :header-rows: 1
      :widths: 40 60

      * - AT 响应
        - 说明
      * - OK
        - AT 命令处理完毕，返回 OK
      * - ERROR
        - AT 命令错误或执行过程中发生错误
      * - SEND OK
        - 数据已发送到协议栈（针对于 :ref:`AT+CIPSEND <cmd-SEND>` 和 :ref:`AT+CIPSENDEX <cmd-SENDEX>` 命令），但不代表数据已经发到对端
      * - SEND FAIL
        - 向协议栈发送数据时发生错误（针对于 :ref:`AT+CIPSEND <cmd-SEND>` 和 :ref:`AT+CIPSENDEX <cmd-SENDEX>` 命令）
      * - +<Command Name>:``...``
        - 详细描述 AT 命令处理结果
      * - CMD FORMAT ERR
        - 没有 该 AT 命令
      * - CMD PARSE ERR
        - 命令解析错误，一般为该 AT 的参数没有按照要求的格式输入

.. _at-messages-report:

- WinnerMicro-AT 消息报告（主动）

  WinnerMicro-AT 会报告系统中重要的状态变化或消息。

  .. centered:: WinnerMicro-AT 消息报告

  .. list-table:: 
     :header-rows: 1
     :widths: 60 60
  
     * - WinnerMicro-AT 消息报告
       - 说明
     * - busy ...
       - 系统繁忙，正在处理上一条命令，无法处理新的命令
     * - WIFI CONNECTED
       - Wi-Fi station 接口已连接到 AP
     * - WIFI GOT IP
       - Wi-Fi station 接口已获取 IPv4 地址
     * - WIFI DISCONNECT
       - Wi-Fi station 接口已与 AP 断开连接
     * - [<conn_id>,]CONNECT
       - ID 为 ``<conn_id>`` 的网络连接已建立（默认情况下，ID 为 0）
     * - [<conn_id>,]CLOSED
       - ID 为 ``<conn_id>`` 的网络连接已断开（默认情况下，ID 为 0）
     * - +STA_CONNECTED: <sta_mac>
       - station 已连接到 WinnerMicro-AT 的 Wi-Fi softAP 接口
     * - +DIST_STA_IP: <sta_mac>,<sta_ip>
       - WinnerMicro-AT 的 Wi-Fi softAP 接口给 station 分配 IP 地址
     * - +STA_DISCONNECTED: <sta_mac> 
       - station 与 WinnerMicro-AT 的 Wi-Fi softAP 接口的连接断开
     * - +IPD
       - WinnerMicro-AT 在非透传模式下，已收到来自网络的数据。有以下的消息格式：

         - 如果 AT+CIPMUX=0，AT+CIPRECVMODE=1，打印：``+IPD,<length>``
         - 如果 AT+CIPMUX=1，AT+CIPRECVMODE=1，打印：``+IPD,<link_id>,<length>``
         - 如果 AT+CIPMUX=0，AT+CIPRECVMODE=0，AT+CIPDINFO=0，打印：``+IPD,<length>:<data>``
         - 如果 AT+CIPMUX=1，AT+CIPRECVMODE=0，AT+CIPDINFO=0，打印：``+IPD,<link_id>,<length>:<data>``
         - 如果 AT+CIPMUX=0，AT+CIPRECVMODE=0，AT+CIPDINFO=1，打印：``+IPD,<length>,<"remote_ip">,<remote_port>:<data>``
         - 如果 AT+CIPMUX=1，AT+CIPRECVMODE=0，AT+CIPDINFO=1，打印：``+IPD,<link_id>,<length>,<"remote_ip">,<remote_port>:<data>``

         其中的 ``link_id`` 为连接 ID，``length`` 为数据长度，``remote_ip`` 为远端 IP 地址，``remote_port`` 为远端端口号，``data`` 为数据。

         注意：当这是个 SSL 连接时，在被动接收模式下（AT+CIPRECVMODE=1），AT 命令口回复的 ``length`` 可能和实际可读的 SSL 数据长度不一致。因为 AT 会优先返回 SSL 层可读的数据长度，如果 SSL 层可读的数据长度为 0，AT 会返回套接字层可读的数据长度。

自定义 AT 命令
------------------------------------------------

本章将具体介绍如何进行 AT 二次开发。

命令注册
^^^^^^^^^^^^

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

-  cmd_name是一个字符指针，可以用来存储命令的名称。
-  cmd_handle是一个类型为wm_atcmd_act的成员，是一个函数指针，可以用来处理特定的命令。
-  cmd_usg是一个字符指针，用来存储AT命令的help信息。

设备注册
^^^^^^^^^^^^

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

- 为了向 AT 注册不同的设备，上述结构体包括了设备类型、设备发送和接收的能力。
- wm_atcmd_dev_type：设备的类型，用于区分不同类型的设备。
- wm_dev_ops_t：操作接口，用于定义设备的各种操作或方法。

指令类型
^^^^^^^^^^^^

::

   AT_TYPE_QUERY = 0, //?    是询问或查询类型的AT指令 。
   AT_TYPE_TEST  = 1, //=?   是测试或验证类型的AT指令。
   AT_TYPE_EXEC  = 2,        是执行或操作类型的AT指令。
   AT_TYPE_MAX               枚举类型的最大值

AT返回值
^^^^^^^^^^^^^

::
         
   WM_AT_RESULT_OK = 0,              /* AT result Ok */  表示AT指令执行成功，值为0 
   WM_AT_RESUT_FAIL,                /* AT result have a generic error */  表示AT指令执行失败，这是一个通用错误
   WM_AT_RESULT_NULL,              /* AT result no string return */   表示AT指令没有返回任何字符串结果
   WM_AT_RESULT_CMD_ERR,          /* AT command can not find */     表示AT指令无法找到或不存在。
   WM_AT_RESULT_PARSE_FAIL,      /* AT command parse  error */     表示AT指令解析错误。
   AT_RESULT_MAX     枚举最大值

API介绍
^^^^^^^^^^^^^^^^^^

1 初始化AT命令
~~~~~~~~~~~~~~~~~~

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
   
2 AT设备的注册
~~~~~~~~~~~~~~~~~~~~

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

3 AT向终端输出打印信息
~~~~~~~~~~~~~~~~~~~~~~~~~

::

   /**
   * @brief     print result
   *
   * @param     format  format str
   *
   * @return    None
   */
  void wm_atcmd_print_result(char *format,...);

4 AT进入透传
~~~~~~~~~~~~~~~~

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

5 AT退出透传
~~~~~~~~~~~~~~~~~~

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

DEMON 演示
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
          wm_atcmd_print_result("这个用来查询用户自定义AT命令的参数，并返回当前值。\r\n");

          return WM_AT_RESULT_OK;
      }
      else if (AT_TYPE_EXEC == type)
      {
          /* do some things... */
          wm_atcmd_print_result("这个用来设置用户自定义AT命令的参数值，并运行此命令。\r\n");

          return WM_AT_RESULT_OK;
      }
      else if (AT_TYPE_TEST == type)
      {
          wm_atcmd_print_result("这个用来设置用户自定义AT命令的help信息。\r\n");

          return WM_AT_RESULT_OK;
      }
      else
      {
          return WM_AT_RESULT_FAIL;
      }
  }

  /* 注册AT命令的第三个参数可以填写成NULL，表示使用用户在命令中定义的help信息；也可以将第三个参数设置成help信息的字符串，表示使用参数设置的help信息。 */
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
