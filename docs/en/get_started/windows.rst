
.. _windows-system:

Windows Command-Line Compilation Environment Setup
======================================================

**Download the Integrated Package to Install the Compilation Environment**

Under the Windows system, we provide a one-click `installation package for the integrated compilation environment <https://doc.winnermicro.net/download/winnermicro_cli_compile_suite_setup.exe>`_。 
The installation path must not contain special characters such as spaces, and the default installation path is ``D:\winnermicro\wmclics``.


.. important::
    - Before installing the integrated compilation environment, you need to download the WM IoT SDK in advance.
    - When using the integrated compilation environment, you must always launch it through the shortcuts provided by the installer (desktop shortcut, Start menu shortcut, or context menu shortcut). Do not directly launch CMD or PowerShell that come with the Windows system.
    - For users who enjoy DIY, please refer to the manual setup for the compilation environment on Linux or macOS systems.
    - If you encounter an issue where wm.py is missing when using the integrated compilation environment, it may have been deleted by certain antivirus software as a threat. It is recommended to adjust your antivirus software settings accordingly.
    - Currently, Ccache is used to accelerate the compilation process. Some antivirus software may take a long time to detect this mechanism, causing compilation to lag. If this happens, it is recommended to adjust your antivirus software settings to improve compilation speed. Alternatively, you can choose to disable the Ccache feature in menuconfig.
    - When using the CMD compilation environment, there may be situations where the window becomes stuck. If this occurs, you can try pressing the Enter key to continue execution. To avoid this issue, it is recommended to use the PowerShell environment.


If you want to switch to a different WM IoT SDK when using the integrated package compilation environment, right-click the generated shortcut icon and select Properties. Then, in the Start in (S): field, enter the new path to the WM IoT SDK.


Of course, you can also achieve the same purpose by directly modifying the WM_IOT_SDK_PATH in the command line window, for example:

In CMD

.. code-block:: text

  set WM_IOT_SDK_PATH=D:\wm\wm_iot_sdk_new
  
Or in PowerShell

.. code-block:: text

  $env:WM_IOT_SDK_PATH="D:\wm\wm_iot_sdk_new"
