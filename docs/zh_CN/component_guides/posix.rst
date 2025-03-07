
POSIX
==============

简介
^^^^^^^^^^^^^^

可移植操作系统接口 (POSIX) 是 IEEE 计算机学会为维护操作系统之间的兼容性而指定的一系列标准。
WM IoT SDK 使用 `FreeRTOS-Plus-POSIX <https://www.freertos.org/zh-cn-cmn-s/FreeRTOS-Plus/FreeRTOS_Plus_POSIX/index.html>`_，可实现 `POSIX 线程 API 的小子集 <http://pubs.opengroup.org/onlinepubs/7908799/xsh/threads.html>`_。
借助此子集，熟悉 POSIX API 的应用程序开发者可使用类似线程原语的 POSIX 开发 FreeRTOS 应用程序。
FreeRTOS-Plus-POSIX 仅实现了约 20% 的 POSIX API。
因此，无法仅使用此包装器将现有的 POSIX 兼容应用程序或 POSIX 兼容库移植到 FreeRTOS 内核上运行。


当前支持的功能
^^^^^^^^^^^^^^^^^^^^^^^^^^

FreeRTOS-Plus-POSIX 实现了部分 `IEEE Std 1003.1-2017 版《开放组技术标准基础规范》 第 7 期 <http://pubs.opengroup.org/onlinepubs/9699919799/>`_。
FreeRTOS-Plus-POSIX 包括以下 POSIX 线程标头文件的实现：

* errno.h
* fcntl.h
* mqueue.h
* pthread.h
* sched.h
* semaphore.h
* signal.h
* sys/types.h
* time.h
* unistd.h
* utils.h

POSIX 的 meunconfig 配置
---------------------------

主要配置如下：

.. list-table::
   :widths: 45 50 25 
   :header-rows: 0
   :align: center

   * - 配置名称
     - 配置描述
     - 默认值

   * - CONFIG_COMPONENT_POSIX_ENABLED
     - 是否启用 POSIX 组件
     - N

   * - CONFIG_POSIX_TASK_NAME_DEFAULT 
     - 配置线程的默认名称
     - pthread 

   * - CONFIG_POSIX_TIMER_NAME_DEFAULT 
     - 配置定时器的默认名称
     - timer

   * - CONFIG_POSIX_MQ_MAX_MESSAGES
     - 配置消息的默认最大数量
     - 10

   * - CONFIG_POSIX_MQ_MAX_SIZE
     - 配置消息的最大字节大小
     - 128
