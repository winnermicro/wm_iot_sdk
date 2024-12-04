
.. _macos-system:

macOS Command-Line Compilation Environment Setup
==================================================

**Download Tools**

- The recommended version of CMake:3.25.1 (version cannot be lower than 3.20); `Official Download <https://cmake.org/download/>`__; `Backup Download <https://doc.winnermicro.net/download/tools/macos/cmake-3.25.1-macos-universal.dmg>`__.
- The recommended version of Ninja:1.11.1; `Official Download <https://github.com/ninja-build/ninja/releases>`__; `Backup Download <https://doc.winnermicro.net/download/tools/macos/ninja-mac.zip>`__.
- The recommended version of Ccache: 4.7.4 (optional); `Official Download <https://ccache.dev/download.html>`__; `Backup Download <https://doc.winnermicro.net/download/tools/macos/ccache-4.7.4.tar.gz>`__.
- The recommended use of Python: Python3.8.10 (version not less than 3.8); `Official Download <https://www.python.org/downloads/>`__; `Backup Download <https://doc.winnermicro.net/download/tools/macos/python-3.6.8-macosx10.6.pkg>`__.
- Toolchain, macOS requires users to build their own toolchain from  `Toolchain <https://github.com/c-sky/toolchain-build>`_. Refer to  `toolchain-build-ck804 <https://github.com/wdyichen/toolchain-build-ck804/blob/master/toolchain-build-ck804.md>`_.
- DebugServer (optional) : not available.
- Besides downloading from the above addresses, macOS systems can install these tools via third-party package managers, which is not detailed here.
- If these tools already exist on your system and meet the version requirements, you can use them directly without reinstalling.
- Install Python dependency packages

Assuming that the wm_iot_sdk has been downloaded, its name is ``wm_iot_sdk`` 。

::

  python -m pip install --user -r /Users/wm/wm_iot_sdk/tools/wm/requirements.txt

Additionally, the Tkinter module needs to be installed for Python on macOS. You can use ``brew install python-tk`` (or ``brew install python3-tk`` ) to install it.

**Set Environment Variables for Tools**

If tools are installed by third-party package managers like  ``brew`` 、 ``port`` (**recommended**), environment variables might already be set and do not need to be configured again.

- Add CMake to PATH

::

    export PATH=/Users/wm/tools/cmake/bin:$PATH

- Add Ninja to PATH

::

    export PATH=/Users/wm/tools/ninja:$PATH

- Add Ccache to PATH

::

    export PATH=/Users/wm/tools/ccache:$PATH

- Add Python to PATH

::

    macOS provides an installer, just install it.

- Add Toolchain to PATH

::

    export PATH=/Users/wm/tools/csky-elfabiv2-tools/bin:$PATH

- Set WM IoT SDK Environment Variable

::

  export WM_IOT_SDK_PATH=/Users/wm/wm_iot_sdk

- Add wm.py Tool to PATH

::

    export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH

The above operations are **temporary settings**，, only effective for the current session. To make them permanent, save the environment variables to the ``~/.bashrc`` file.


.. important::
    - The scripts in the WM IoT SDK require executable permissions. For the obtained WM IoT SDK compressed package, please unzip it in a macOS system.Do not directly copy already unzipped WM IoT SDK files from another system to the macOS system, as this will result in the loss of permission information and prevent normal usage.

**Switching to the new WM IoT SDK**

When you want to switch to a different version of the WM IoT SDK from the one you configured above, 
you need to run the

::

  export WM_IOT_SDK_PATH=/Users/wm/wm_iot_sdk_new
  export PATH=$WM_IOT_SDK_PATH/tools/wm:$PATH
