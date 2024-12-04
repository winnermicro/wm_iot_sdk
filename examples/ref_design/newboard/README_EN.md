# New Board Reference Project

## Functional Overview

This project demonstrates how to configure the Device Table and Partition Table based on a new hardware development board, assisting developers in quickly sorting out the development steps and completing the creation of a basic project in a brand-new project.

## Environmental Requirements

Either the official development board from Winner Micro can be used, or the development board built by the developer himself/herself based on the WinnerMicro SoC module can be used.

## Instructions for Development Steps

### 1: Understand the Requirements of Hardware Design

   Obtain the schematic diagram of the development board or the hardware requirement specification from the hardware engineer or the development board supplier. Based on this, the software engineer can clearly know the following in this project:

   1.1. What is the model of the SoC used? How much hardware resources such as Memory, built-in/external Flash, Interrupt, Timer, and IO does the SoC contain?
   The corresponding chip manual materials can be referred to at [SoC Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/soc_guides/index.html)

   1.2. What peripherals are paired with it, such as UART, SPI-Master, I2S, etc.? How many pins of the SOC are needed? What are the performance requirements and corresponding parameter configuration requirements for each peripheral in this project?
        
   The documentation can be referred to at [SoC-PINMUX](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/pinmux.html)

### 2:Sort out the Overall Software Function Requirements of the Project

   Based on this, the software engineer can clearly know the following in this project:

   2.1. How many functions are to be developed, and approximately how much hardware storage resources such as Memory and Flash are needed for each function.

   2.2. The overall framework of the project software, how many tasks need to be created, the priority of each task, and the resource requirements for stack and heap, the selection of the communication mechanism and the collaborative relationship between each task, etc.

### 3:After Confirming the Software and Hardware Requirement Items, Create a New SDK Project

   There are two ways to do this:

   3.1. Directly in the SDK, select a base project, such as the 'hello_world' or the 'newboard' project here, copy it to another directory of examples and rename it to your own project name, such as 'project_a'.
   It should be noted that the project name should be in English letters, numbers can be added, but it should not contain spaces or characters of other languages.

   The documentation can be referred to at [Build System-Quick Project Creation](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/build_system.html)

   3.2. Create a project based on the vscode IDE environment.

   The documentation can be referred to at [IDE-Creat Project](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/ide.html)

### 4:Create the Device Table Corresponding to the New Project

   Although the default wm_dt_hw.c and wm_dt_hw.h files are included in the components/wm_dt directory of the SDK, the contents of these files include the device table definitions of all the controller devices and external devices natively supported by the SDK.
   In the developer's own project, not all the devices defined therein may be used, and the used devices may not necessarily retain the same configuration parameters (for example, the pin selection in different projects may be different).
   Through the investigation in 1.2, the developer should basically be able to determine whether a unique Device Table also needs to be created for the new project.
   If so, it can be created with reference to this example.
   
   The documentation can be referred to at [Device Table](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/device_table.html)
   
### 5：Create the Partition Table Corresponding to the New Project

   Although the default partition_table_large.csv, partition_table_normal.csv, and partition_table_with_ota.csv partition table files are included in the components/partition_table/config directory of the SDK, these files can be selected by the user in the menuconfig interface, but these partition table files only contain basic partitions.
   In the developer's own project, there may be a need for project-specific partitions to facilitate the implementation of business operations, such as storing configuration files, audio files, keys, factory parameters, etc.
   Through the investigation in 2.1, the developer should basically be able to determine whether a unique partition also needs to be created for the new project and manage its partition table file separately.
   If so, it can be created with reference to this example.
   
   The documentation can be referred to at [Partition Table](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/partition_table.html)

### 6:Configure the Function Switches Based on Business Requirements

   The SDK adopts a component architecture, and each component is managed by one or more Kconfig function switches, and many function switches are closed by default.
   It is not recommended to directly open the function switches needed by the project in the Kconfig files under each component directory. Instead, it is recommended to add the definitions of the required function switches to the prj.config file of the new project.
   This can avoid affecting other projects in this SDK.
   After modification, you can use the menuconfig interface to check whether all the function switches required by this project are correctly set.
   
   The documentation can be referred to at [Kconfig](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/kconfig.html)

   It should be noted that the function switches related to devices will be related to the configuration in the device table. When opening the relevant device functions, the contents of the original device table also need to be copied to the device table of the new project, otherwise compilation errors will occur.
   

### 7:Create Your Own Tasks and Components for Business Development

   When creating your own application tasks, you need to consider the impact on other tasks. Therefore, you need to query the overall priority allocation situation of other tasks in the system to determine the priority of the application task.

   The documentation can be referred to at [Development Assistant](https://doc.winnermicro.net/w800/en/2.2-beta.2/component_guides/dev_asst.html)


## Compilation and Burnol

Example Location: `examples/ref_design/newboard`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)

## Running Results

Successful running will output the following logs, among which the partition table info will print out the contents of the partition_table_custom.csv stored in this project directory.

```
partition table info:
  name             offset     size       flag
  ft               0x00000000 0x00002000 0x00
  bootloader       0x00002000 0x0000c000 0x00
  partition_table  0x0000e000 0x00001000 0x00
  app              0x0000f000 0x001d1000 0x00
  user             0x001e0000 0x00002000 0x00
  nvs              0x001f0000 0x00008000 0x00
[I] (4) main: Ver: 0.8.1 build at Sep  3 2024 21:16:57
[I] (4) ref new board: heap free size=271296(bytes)
[I] (1004) ref new board: heap free size=275552(bytes)
[I] (2004) ref new board: heap free size=275552(bytes)
[I] (3004) ref new board: heap free size=275552(bytes)
[I] (4004) ref new board: heap free size=275552(bytes)
```