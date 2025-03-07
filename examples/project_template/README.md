# 工程模板

## 功能概述

这是一个完整的构建示例项目，用户可以根据需要复制和修改。

### 示例文件夹内容

项目 **project_template**  包含一个 C 语言的源文件 main.c 。该文件位于文件夹 main 中。

WMSDK是使用CMake构建的。项目构建配置包含在`CMakeLists.txt`文件中。 

下面是对项目文件夹中剩余文件的简要说明。

```
├── main
│   ├── include
│   │   └── test.h
│   ├── src
│   │   ├── src
│   │   ├── src
│   │   └── test.h
│   ├── CMakeLists.txt
│   └── Kconfig
│   ├── CMakeLists.txt
│   └── Kconfig
│   ├── CMakeLists.txt
│   └── Kconfig
├── CMakeLists.txt
├── prj.config
└── README.md                  这就是您当前正在读取的文件
```
此外，该项目还包含 kconfig 文件，用于 menuconfig 配置系统。

## 环境要求

无。

## 编译和烧录

示例位置：`examples/project_template`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)
