
.. |Equipment-Name| replace:: w800

应用程序启动流程
=====================================

本文将介绍 |Equipment-Name| 从上电到运行 ``main``
函数中间所经历的步骤（即启动流程）。

在宏观上，该启动流程可以分为以下 3 个步骤：


.. mermaid::
  :align: center

  graph TB
  
  A(一级引导程序)-->B[Bootloader 引导程序]
  B-->C[应用程序（app）]


1. 一级引导程序被固化在内部的 ROM 中，当 CPU 上电后会从地址 ``0x0`` 开始执行 ROM 中的固化程序，并加载 Bootloader 
   引导程序。


2. Bootloader 引导程序是在操作系统运行之前的一小段程序，用于初始化硬件设备，将系统的软硬件环境带到一个合适的状态，
   为最终调用操作系统做好准备，并从 QFlash 中加载分区表，以决定启动的应用程序位置。

3. 应用程序（app）运行阶段，会从文件中获取应用程序运行所需要的相关配置，如内存起始地址，内存布局等。

下面会对上述过程进行更为详细的阐述。

一级引导程序
------------------

上电后，CPU 会启动执行 ROM 中的固化程序，包括 QFLASH 自检，QFLASH 模式切换，IMAGE 校验、向量表重定向和升级程序。

- 详细 ROM 请参考 :ref:`ROM 功能<rom_function>` 。

Bootloader 引导程序
------------------------

它主要完成：QFlash 工作模式切换，应用程序完整性校验，代码解压，代码搬移，跳转到用户的应用程序去运行等。

- 详细 Bootloader 请参考 :ref:`Bootloader 功能<Bootloader>` 。

应用程序（app）运行
-------------------------

在应用程序运行阶段，它会从 `components/wm_soc/w80x/ld` 里的 
``chip_layout.ld.h`` 获取应用程序的入口函数。

从 `components/wm_soc/w80x/src` 里的 ``memory_layout.c`` 获取
应用程序的内存布局。

然后经过一系统初始化后运行  ``main`` 函数，交由用户按实际需要自己编写。


.. _label-components_autoinit:

组件（components）自动初始化
------------------------------------

除了在 ``mian`` 函数之前手动初始化组件，我们还提供组件自动初始化的方法。

每个需要执行初始化的组件，都需要使用 `WM_COMPONEN_INIT_*(component_init_function)` 修饰其组件初始化函数，
系统在启动之后会自动调用组件初始化函数进行组件初始化。

其中 `*` 取值为： `0` 、 `1` 、 `2` 、 `3` 、 `4` ，即：

 - `WM_COMPONEN_INIT_0(component_init_function)`

 - `WM_COMPONEN_INIT_1(component_init_function)`
 - `WM_COMPONEN_INIT_2(component_init_function)`
 - `WM_COMPONEN_INIT_3(component_init_function)`
 - `WM_COMPONEN_INIT_4(component_init_function)`

这4种修饰的组件初始化函数的执行顺序是不同的，其执行顺序为：


.. mermaid::
  :align: center
  
  graph TB
  
  A(WM_COMPONEN_INIT_0)-->B[wm_start]
  B-->C[Driver AUTO INIT]
  C-->D[WM_COMPONEN_INIT_1]
  D-->E[WM_COMPONEN_INIT_2]
  E-->F[WM_COMPONEN_INIT_3]
  F-->G[WM_COMPONEN_INIT_4]
  G-->H[main]

其中 `WM_COMPONEN_INIT_0` 最早被调用，在此阶段时，整个系统只有堆内存被初始化，所以除非特别需要一般不推荐用户使用。

一般而言，
基础系统组件使用 `WM_COMPONEN_INIT_1` ，普通系统组件使用 `WM_COMPONEN_INIT_2`，
应用组件使用 `WM_COMPONEN_INIT_3` ，用户组件使用 `WM_COMPONEN_INIT_4` 。

- 举例：cli应用组件初始化函数为 `wm_component_cli_init` ，则添加
- 修饰 `WM_COMPONEN_INIT_3(wm_component_cli_init)` 。

