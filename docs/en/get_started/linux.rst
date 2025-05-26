
.. _linux-system:

Linux Command-Line Compilation Environment Setup
====================================================

Tools and Versions
~~~~~~~~~~~~~~~~~~

- The recommended version of CMake: 3.25.1(version cannot be lower than 3.20)
- The recommended version of Ninja: 1.11.1
- The recommended version of Ccache: 4.7.4(optional)
- The recommended use of Python: Python 3.8.10 (version not less than 3.8)
- Toolchain
- DebugServer (optional)

Install Tools
~~~~~~~~~~~~~~

There are two ways to install tools:  
  - 1. Use package management tools such as apt, yum, dnf, etc. to automatically install related tools.
  - 2. Users can download from the official website or designated website of each tool and manually configure environment variables.

**Installation method 1**

This is also our **recommended** approach, as if users use commands (apt, yum) to install related tools, the environment variables may have 
already been added and there is no need for users to manually set them again.

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

- **Toolchain tool still needs to be installed manually using the following method.**.


- Install Python dependency packages

Assuming the WM IoT SDK has been downloaded and named ``wm_iot_sdk`` 。

::

  python -m pip install --user -r /home/wm/wm_iot_sdk/tools/wm/requirements.txt

Due to the protection mechanism enabled in Python 3.11 and above (PEP 668), pip is prevented from directly installing Python packages. Execute the above command
Possible errors may occur: externally-managed-environment. Need to add the ``-- break system packages`` parameter to bypass the protection mechanism.

In addition,the Tkinter module needs to be installed for Python on Linux. For Ubuntu systems, you can use ``sudo apt-get install python-tk`` (or ``sudo apt-get install python3-tk`` ) to install it.

**Installation method 2**

- CMake: `Official Download <https://cmake.org/download/>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/cmake-3.25.1-linux-x86_64.sh>`__.
- Ninja: `Official Download <https://github.com/ninja-build/ninja/releases>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/ninja-linux.zip>`__.
- Ccache(optional): `Official Download <https://ccache.dev/download.html>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/ccache-4.7.4-linux-x86_64.tar.xz>`__.
- Python: `Official Download <https://www.python.org/downloads/>`__ ; `Backup Download <https://doc.winnermicro.net/download/tools/linux/Python-3.6.8.tar.xz>`__.
- Toolchain: `Official Download <https://occ.t-head.cn/community/my/download?id=3885366095506644992>`__; `Backup Download <https://doc.winnermicro.net/download/tools/linux/csky-elfabiv2-tools-x86_64-minilibc-20210423.tar.gz>`__.

    - The approximate name of the toolchain is csky-elfabiv2-tools-\*.tar, the toolchains with -mingw- in the name are for Windows, while others are for Linux.
    - Under Linux, some versions of toolchain may depend on ncurses. If you see the prompt "error while loading shared libraries: libncurses.so.5: cannot open shared object file: No such file or directory" , install libncurses5 (e.g., sudo apt install libncurses5). Other similar issues may need to be resolved by the user.

- DebugServer (optional): `Official Download <https://www.xrvm.cn/community/download?id=4313368247333359616>`__, `Backup Download <https://doc.winnermicro.net/download/tools/linux/XuanTie-DebugServer-linux.zip>`__.

    - For detailed installation instructions, please refer to the relevant installation section of the `T-Head Debugger Server User Guide <../../../../download/debug_server/T-Head_Debugger_Server_User_Guide_ZH-CN.pdf>`_.

**Set Environment Variables for Tools**

Assuming that all tools are stored in the/home/wm/tools directory

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
