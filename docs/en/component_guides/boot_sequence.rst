
.. |Equipment-Name| replace:: W800

Application Startup Process
=====================================

This document will introduce the steps (i.e., the startup process) that the |Equipment-Name| undergoes from power-on to running the ``main`` function.

Macroscopically, the startup process can be divided into the following three steps:


.. mermaid::
  :align: center

  graph TB
  
  A(Primary Bootloader)-->B[Bootloader program]
  B-->C[Application]


1. The primary bootloader is fixed in the internal ROM. Once the CPU is powered on, it begins executing the program stored in ROM starting from address ``0x0`` and loads the Bootloader.

2. The Bootloader is a small program that runs before the operating system, responsible for initializing hardware devices, bringing the system's software and hardware environment to a suitable state,preparing for the final call to the operating system, and loading the partition table from QFlash to determine the location of the application to be started.


3. During the application(app) runtime phase, it retrieves the necessary configurations for the application's operation from the file, such as the starting memory address and memory layout.

Below is a more detailed explanation of the aforementioned processes.



Primary Bootloader Program
---------------------------

Upon powering on, the CPU initiates execution of the program stored in ROM, including QFLASH self-check, QFLASH mode switching, IMAGE verification, vector table redirection, and upgrade procedures.

- For detailed ROM functions, please refer to :ref:`ROM Functionality<rom_function>` 。

Bootloader Program
------------------------

Its main tasks include: switching the QFlash working mode, verifying the integrity of the application, decompressing the code, moving the code, and jumping to the user's application to run.

- For detailed information on the Bootloader, please refer to :ref:`Bootloader Functionality<Bootloader>` 。

Application Running
-------------------------

During the application runtime phase, it retrieves the application's entry function from the ``chip_layout.ld.h`` in `components/wm_soc/w80x/ld` .

It obtains the application's memory layout from ``memory_layout.c`` located in `components/wm_soc/w80x/src`.

After a series of initializations, it runs the ``main`` function, which the user can write as needed.


.. _label-components_autoinit:

Automatic Initialization of Components
----------------------------------------

In addition to manually initializing components before the ``mian`` function, we also provide a method for automatic component initialization.

Each component that needs to execute initialization must use `WM_COMPONEN_INIT_*(component_init_function)` to annotate its component initialization function. The system will automatically call the component initialization function after startup to initialize the component.

The value of `*`  is: `0`, `1`, `2` , `3`, `4`, that is:

 - `WM_COMPONEN_INIT_0(component_init_function)`

 - `WM_COMPONEN_INIT_1(component_init_function)`
 - `WM_COMPONEN_INIT_2(component_init_function)`
 - `WM_COMPONEN_INIT_3(component_init_function)`
 - `WM_COMPONEN_INIT_4(component_init_function)`

The execution order of these four annotated component initialization functions is different, with the execution order being:


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


Among them, `WM_COMPONEN_INIT_0` is called at the earliest stage, at this stage, the whole system's only heap memory is initialized, so it is not recommended to use it unless specially needed.

Generally,
Basic system components use `WM_COMPONEN_INIT_1` , regular system components use  `WM_COMPONEN_INIT_2` , application components use `WM_COMPONEN_INIT_3` , and user components use `WM_COMPONEN_INIT_4`.

- For example, if the initialization function of the CLI application component is `wm_component_cli_init` , then add
- Decorate `WM_COMPONEN_INIT_3(wm_component_cli_init)`.

