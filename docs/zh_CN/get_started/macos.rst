
.. _macos-system:

macOS 搭建命令行编译环境
=========================

**下载工具**

- CMake 推荐版本：3.25.1（版本不能低于3.20）：`官方下载 <https://cmake.org/download/>`__，`备用下载 <https://doc.winnermicro.net/download/tools/macos/cmake-3.25.1-macos-universal.dmg>`__。
- Ninja 推荐版本：1.11.1：`官方下载 <https://github.com/ninja-build/ninja/releases>`__，`备用下载 <https://doc.winnermicro.net/download/tools/macos/ninja-mac.zip>`__。
- Ccache 推荐版本：4.7.4（可选）：`官方下载 <https://ccache.dev/download.html>`__，`备用下载 <https://doc.winnermicro.net/download/tools/macos/ccache-4.7.4.tar.gz>`__。
- Python 推荐使用：Python3.8.10（版本不能低于3.8）：`官方下载 <https://www.python.org/downloads/>`__，`备用下载 <https://doc.winnermicro.net/download/tools/macos/python-3.8.10-macos.zip>`__。
- Toolchain，macOS 系统需要自行构建 `工具链 <https://github.com/c-sky/toolchain-build>`_，可参阅 `toolchain-build-ck804 <https://github.com/wdyichen/toolchain-build-ck804/blob/master/toolchain-build-ck804.md>`_。
- DebugServer（可选）：暂无。
- 除了在上述地址下载之外，macOS 系统可以通过第三方包管理工具进行安装，此处不再细分阐述。
- 如果系统中已经存在这些工具，版本满足要求的话可以直接使用，无需再次下载安装。
- 安装 Python 依赖包

当前假设已经下载了 wm_iot_sdk，其名称为 ``wm_iot_sdk`` 。

::

  python -m pip install --user -r /Users/wm/wm_iot_sdk/tools/wm/requirements.txt

在 macOS 系统下还需要为 Python 安装 Tkinter 模块的支持。如可以使用 ``brew install python-tk`` （或 ``brew install python3-tk`` ）来安装。

**设置工具的环境变量**

如果用户使用类似 ``brew`` 、 ``port`` 等之类第三方包管理工具来安装（**推荐**），则环境变量可能已被加入，无需再次设置。

- 将 CMake 添加至环境变量

::

    export PATH=/Users/wm/tools/cmake/bin:$PATH

- 将 Ninja 添加至环境变量

::

    export PATH=/Users/wm/tools/ninja:$PATH

- 将 Ccache 添加至环境变量

::

    export PATH=/Users/wm/tools/ccache:$PATH

- 将 Python 添加至环境变量

::

    macOS 系统下提供了安装程序，直接安装即可。

- 将 Toolchain 添加至环境变量

::

    export PATH=/Users/wm/tools/csky-elfabiv2-tools/bin:$PATH

- 设置 WM IoT SDK 环境变量

::

  export WM_IOT_SDK_PATH=/Users/wm/wm_iot_sdk

- 将 wm.py 工具添加至环境变量

::

    export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH

以上操作为 **临时设置**，仅对当前环境有效。如果想永久生效：请将环境变量保存至 ``~/.bash_profile`` 文件。


.. important::

    WM IoT SDK 中的脚本需要可执行权限，对于获得的 WM IoT SDK 压缩包，请在 macOS 系统中解压。
    不要直接拷贝在其它系统中已经解压的 WM IoT SDK 文件至 macOS 系统中，这会丢失权限信息，导致不能正常使用。

**切换新的 WM IoT SDK**

当上述配好的环境，想切换至另一份不同版本的 WM IoT SDK 使用时，需执行

::

  export WM_IOT_SDK_PATH=/Users/wm/wm_iot_sdk_new
  export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH
