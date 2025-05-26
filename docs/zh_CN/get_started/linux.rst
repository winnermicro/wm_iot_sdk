
.. _linux-system:

Linux 搭建命令行编译环境
==========================

所需工具及版本
~~~~~~~~~~~~~~

- CMake：推荐版本 3.25.1（版本不能低于3.20）
- Ninja：推荐版本 1.11.1
- Ccache（可选）：推荐版本 4.7.4
- Python：推荐使用 Python3.8.10（版本不能低于3.8）
- Toolchain
- DebugServer（可选）

安装工具
~~~~~~~~~~~

安装工具有两种方式：  
  - 1、使用类似 ``apt`` 、``yum``、``dnf`` 等之类包管理工具自动安装相关工具。
  - 2、用户在各工具的官方网站或指定网站下载，然后手动配置环境变量。

**安装方式1：**

这也是 **推荐** 的方式，因为用户使用命令（apt、yum）来安装相关工具，则环境变量可能已被加入，无需用户再次手动设置。

- 安装 CMake

::

  sudo apt-get install cmake

- 安装 Ninja

::

  sudo apt-get install ninja-build

- 安装 Ccache

::

  sudo apt-get install ccache

- 安装 Python3

::

  sudo apt-get install python3

- **Toolchain 工具仍需采用下面的手动方式安装**。


- 安装 Python 依赖包

当前假设已经下载了 wm_iot_sdk，其名称为 ``wm_iot_sdk``，存放在 /home/wm 目录下。

::

  python -m pip install --user -r /home/wm/wm_iot_sdk/tools/wm/requirements.txt

由于 Python 3.11 以上启用的保护机制（PEP 668），阻止了 pip 直接安装 python 包。执行上面命令
可能会出现 error: externally-managed-environment 。需要添加 ``--break-system-packages`` 参数绕过保护机制。

在 Linux 系统下还需要为 Python 安装 Tkinter 模块的支持。如 Ubuntu 系统可使用 ``sudo apt-get install python-tk`` （或 ``sudo apt-get install python3-tk`` ）来安装。

**安装方式2：**

用户在各工具的官方网站或指定网站下载，然后手动配置环境变量。

- CMake：`官方下载 <https://cmake.org/download/>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/cmake-3.25.1-linux-x86_64.sh>`__。
- Ninja：`官方下载 <https://github.com/ninja-build/ninja/releases>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/ninja-linux.zip>`__。
- Ccache（可选）：`官方下载 <https://ccache.dev/download.html>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/ccache-4.7.4-linux-x86_64.tar.xz>`__。
- Python：`官方下载 <https://www.python.org/downloads/>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/Python-3.8.10.tar.xz>`__。
- Toolchain：`官方下载 <https://occ.t-head.cn/community/my/download?id=3885366095506644992>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz>`__。

  - 工具链名字大致为 csky-elfabiv2-tools-\*.tar，名字中带 -mingw- 的为 Windows 系统下的工具链，其余为 Linux 系统下的。

  - Linux 系统下有些版本的工具链有可能依赖 ncurses，如提示 "error while loading shared libraries: libncurses.so.5: cannot open shared object file: No such file or directory" 时，可安装libncurses5（如"sudo apt install libncurses5"），其它类似问题需自行研究解决。

- DebugServer（可选）：`官方下载 <https://www.xrvm.cn/community/download?id=4313368247333359616>`__，`备用下载 <https://doc.winnermicro.net/download/tools/linux/XuanTie-DebugServer-linux.zip>`__。

  - 详细安装方法请查看 `T-Head Debugger Server User Guide <../../../../download/debug_server/T-Head_Debugger_Server_User_Guide_ZH-CN.pdf>`_ 相关安装章节。

**设置工具的环境变量**

当前假设各个工具都存放到 /home/wm/tools 目录下

- 将 CMake 添加至环境变量

::

    export PATH=/home/wm/tools/cmake/bin:$PATH

- 将 Ninja 添加至环境变量

::

    export PATH=/home/wm/tools/ninja:$PATH

- 将 Ccache 添加至环境变量

::

    export PATH=/home/wm/tools/ccache:$PATH

- 将 Python 添加至环境变量

::

    //Linux 系统下只提供了源码包，所以需要先编译，再添加至环境变量：
    //如下为 Linux 下的源码常规安装步骤，因 Linux 系统的开放性，遇到问题需用户自行解决。

    ./configure --prefix=/home/wm/tools/python

    make

    make install

    export PATH=/home/wm/tools/python:$PATH

- 将 Toolchain 添加至环境变量

::

    export PATH=/home/wm/tools/csky-elfabiv2-tools/bin:$PATH

- 设置 WM IoT SDK 环境变量

::

  export WM_IOT_SDK_PATH=/home/wm/wm_iot_sdk

- 将 wm.py 工具添加至环境变量

::

    export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH

以上操作为 **临时设置**，仅对当前环境有效。如果想永久生效：请将环境变量保存至 ``~/.bashrc`` 文件。

.. important::

    - WM IoT SDK 中的脚本需要可执行权限，对于获得的 WM IoT SDK 压缩包，请在 Linux 系统中解压。
      不要直接拷贝在其它系统中已经解压的 WM IoT SDK 文件至 Linux 系统中，这会丢失权限信息，导致不能正常使用。
    - 对于使用 Ubuntu 系统的用户，推荐使用 Ubuntu 20.04 及以上版本的系统。

**切换新的 WM IoT SDK**

当上述配好的环境，想切换至另一份不同版本的 WM IoT SDK 使用时，需执行

::

  export WM_IOT_SDK_PATH=/home/wm/wm_iot_sdk_new
  export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH
