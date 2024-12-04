
.. _linux-system:

Linux Command-Line Compilation Environment Setup
====================================================

**Download Tools**

- The recommended version of CMake: 3.25.1(version cannot be lower than 3.20), `Official Download <https://cmake.org/download/>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/cmake-3.25.1-linux-x86_64.sh>`__.
- The recommended version of Ninja：1.11.1, `Official Download <https://github.com/ninja-build/ninja/releases>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/ninja-linux.zip>`__.
- The recommended version of Ccache: 4.7.4(optional) , `Official Download <https://ccache.dev/download.html>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/ccache-4.7.4-linux-x86_64.tar.xz>`__.
- The recommended use of Python: Python 3.8.10 (version not less than 3.8), `Official Download <https://www.python.org/downloads/>`__ ; `Backup Download <https://doc.winnermicro.net/download/tools/linux/Python-3.6.8.tar.xz>`__.
- Toolchain：`Official Download <https://occ.t-head.cn/community/my/download?id=3885366095506644992>`__， `Backup Download <https://doc.winnermicro.net/download/tools/linux/csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz>`__.
  
    * The approximate name of the toolchain is csky-elfabiv2-tools-\*.tar，the toolchains with -mingw- in the name are for Windows, while others are for Linux.
    * Under Linux, some versions of toolchain may depend on ncurses. If you see the prompt "error while loading shared libraries: libncurses.so.5: cannot open shared object file: No such file or directory" , install libncurses5 (e.g., sudo apt install libncurses5). Other similar issues may need to be resolved by the user.

- DebugServer (optional): `Official Download <https://www.xrvm.cn/community/download?id=4313368247333359616>`__, `Backup Download <https://doc.winnermicro.net/download/tools/linux/XuanTie-DebugServer-linux.zip>`__.
    * For detailed installation instructions, please refer to the relevant installation section of the `T-Head Debugger Server User Guide <../../../../download/debug_server/T-Head_Debugger_Server_User_Guide_ZH-CN.pdf>`_.

- Besides downloading from the above addresses, Linux systems often allow installation via built-in package managers. Details are not provided here.
- If these tools already exist on your system and meet version requirements, you can use them directly without reinstalling.

On Linux systems, it's **recommended** to use package managers like ``apt`` 、``yum``、``dnf`` to automatically install the related tools. Here are examples of using the apt command on an Ubuntu system, with specific steps outlined below:

- Install CMake

::

  sudo apt-get install cmake

- Instal Ninja

::

  sudo apt-get install ninja-build

- Instal Ccache

::

  sudo apt-get install ccache

- Instal Python3

::

  sudo apt-get install python3

- The toolchain tools still need to be installed manually as mentioned above.

- Install Python dependency packages

Assuming the WM IoT SDK has been downloaded and named ``wm_iot_sdk`` 。

::

  python -m pip install --user -r /home/wm/wm_iot_sdk/tools/wm/requirements.txt

In addition,the Tkinter module needs to be installed for Python on Linux. For Ubuntu systems, you can use ``sudo apt-get install python-tk`` (or ``sudo apt-get install python3-tk`` ) to install it.

**Set Environment Variables for Tools**

If tools are installed by package managers(apt、yum), environment variables might already be set and do not need to be configured again.

- Add CMake to PATH

::

    export PATH=/home/wm/tools/cmake/bin:$PATH

- Add Ninja to PATH

::

    export PATH=/home/wm/tools/ninja:$PATH

- Add Ccache to PATH

::

    export PATH=/home/wm/tools/ccache:$PATH

- Add Python to PATH

::

    //Under the Linux system, only the source code package is provided, so it needs to be compiled first and then added to the environment variables:
	//Below are the general installation steps for source code on Linux. Due to the openness of the Linux system, users need to resolve any issues encountered on their own.

    ./configure --prefix=/home/wm/tools/python

    make

    make install

    export PATH=/home/wm/tools/python:$PATH

- Add Toolchain to PATH

::

    export PATH=/home/wm/tools/csky-elfabiv2-tools/bin:$PATH

- Set WM IoT SDK Environment Variable

::

  export WM_IOT_SDK_PATH=/home/wm/wm_iot_sdk

- Add wm.py Tool to PATH

::

    export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH

The above operations are **temporary settings** that only take effect in the current environment. To make them permanent, save the environment variables to the ``~/.bashrc`` file.

.. important::
    - The scripts in the WM IoT SDK require executable permissions. For the obtained WM IoT SDK compressed package, please unzip it in a Linux system.
      Do not directly copy already unzipped WM IoT SDK files from another system to the Linux system, as this will result in the loss of permission information and prevent normal usage.
    - For users using the Ubuntu system, it is recommended to use Ubuntu 20.04 or later versions.

**Switching to the new WM IoT SDK**

When you want to switch to a different version of the WM IoT SDK from the one you configured above, 
you need to run the

::

  export WM_IOT_SDK_PATH=/home/wm/wm_iot_sdk_new
  export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH
