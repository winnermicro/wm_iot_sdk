
POSIX
==============

Introduction
^^^^^^^^^^^^^^

Portable Operating System Interface (POSIX) is a series of standards specified by the IEEE Computer Society to maintain compatibility between operating systems.
The WM IoT SDK  utilizes `FreeRTOS-Plus-POSIX <https://www.freertos.org/zh-cn-cmn-s/FreeRTOS-Plus/FreeRTOS_Plus_POSIX/index.html>`_,which can implement `A subset of POSIX threading API <http://pubs.opengroup.org/onlinepubs/7908799/xsh/threads.html>`_.
With this subset, application developers familiar with POSIX API can develop FreeRTOS applications using POSIX-like threading primitives.
However, FreeRTOS-Plus-POSIX only implements about 20% of the POSIX API.
Therefore, it is not possible to simply port existing POSIX-compliant applications or libraries to run on the FreeRTOS kernel using this wrapper alone.


Current Supported Features
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

FreeRTOS-Plus-POSIX has implemented part of the `IEEE Std 1003.1-2017 version of the Open Group Technical Standards Basic Specification, Issue 7 <http://pubs.opengroup.org/onlinepubs/9699919799/>`_.
FreeRTOS-Plus-POSIX includes the implementation of the following POSIX thread header files:

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


POSIX meunconfig configuration
---------------------------------

The main configurations are as follows:

.. list-table::
  :widths: 45 50 25 
  :header-rows: 0
  :align: center

  * - Configuration Name
    - Config Discriptors
    - Default Values

  * - CONFIG_COMPONENT_POSIX_ENABLED
    - Whether to enable POSIX components
    - N

  * - CONFIG_POSIX_TASK_NAME_DEFAULT 
    - Configure the default name of the thread
    - pthread 

  * - CONFIG_POSIX_TIMER_NAME_DEFAULT 
    - Set the default name of the timer
    - timer

  * - CONFIG_POSIX_MQ_MAX_MESSAGES
    - Configures the default maximum number of messages
    - 10

  * - CONFIG_POSIX_MQ_MAX_SIZE
    - Configures the maximum byte size of the message
    - 128