
性能优化
==============

更好的性能
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: w800

    * 提高 cpu 主频，最高可达 240 MHz


* 修改默认编译优化级别为 -O2 或 -O3，需注意，提高编译器优化级别可能增大固件体积和暴露某些之前未发现的错误。
* 将频繁执行的代码移至 SRAM 中，减少 XIP 执行加载等待时间。

    - 单个函数可使用 ``ATTRIBUTE_IRAM`` 修饰，如将函数 `my_func` 放在 IRAM 中

    ::

        ATTRIBUTE_IRAM void my_func(void)
        {
            /* do somethings... */
        }

    - 整个文件可在 ``components\wm_system\ld\code_in_ram.ld.h`` 中指定，如将组件 `main` 中的文件 `my_code.c` 放在 IRAM 中

    ::

         *libmain.a:my_code.c.obj(.text .text.*)

* 减少日志开销，编译时可将日志默认等级静态调至 ERROR 及以下。
* 关闭堆内存和 FreeRTOS 中为调试附加的统计功能。
* 合理调整 FreeRTOS 各任务的优先级。
* 提高中断性能，尽量减少开关中断作为临界区保护。
* 提高网络速度，可参考 iperf 示例，尽量提高 WiFi 驱动和 TCP/IP 中的可用缓存和配置。
* 重新启用跳转表优化，这将提高大型 switch cases 代码中的热路径性能，编译时可添加编译参数 ``-fjump-tables -ftree-switch-conversion``。
* 程序中避免使用浮点运算（float 和 double），浮点运算总是慢于整数运算。

此外，提高应用程序性能的方法根据使用场景可能有更多的方式，需用户自行灵活选择。

更好的体积
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* 关闭各组件中不使用的功能，可在 menuconfig 中选择关闭。
* 编译优化级别选择 -Os，这也是默认选择的级别。
* 编译后查看 ``map`` 文件分析大小占用情况，map 文件在工程 ``build`` 文件夹中。
