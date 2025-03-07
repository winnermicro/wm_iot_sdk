
.. _ide:

VS Code
==================
**The following IDEs are currently available for users:**

:ref:`VS Code<ide>`

**Environment Requirements**

- VS Code requires version V1.70.0 or above, otherwise the plugin cannot be installed successfully.

.. _vscode_windows:

**Usage in Windows Environment**

1. Install VS Code

Visit the `official website <https://code.visualstudio.com/>`__ of Visual Studio Code to download and install it correctly.

2. After opening VS Code, click on the extensions on the leftmost side, then search for ``WM-IDE`` to install it.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_plugin.svg
   :align: center

3. For better development with VS Code, it is recommended to install the following extensions as well:

- Microsoft C/C++ extensions

- Microsoft Serial Monitor Extension

4. Open the ``WM-IDE`` plugin

Click on the WinnerMicro icon in the left functional panel to enter the plugin's main interface, where you can perform operations such as environment configuration and project creation.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_ui.svg
   :align: center

5. Configure the ``WM-IDE`` plugin

Before using WM-IDE, you must first perform the configuration, and before that, you need to obtain the WM IoT SDK software development framework source code from the `download page <https://doc.winnermicro.net/download/version>`__ .

Complete the configurations shown in the diagram, including ``WM IoT SDK Path`` and ``WM Dev Suite Path``.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config1.svg
   :align: center

For ``WM IoT SDK Path``:

- If you have already downloaded the wm_iot_sdk source code, you can directly select ``Browse`` to configure

::

  WM IoT SDK source code download storage path\wm_iot_sdk

- If you haven't downloaded the wm_iot_sdk source code, you can click ``Download`` and select the download path for automatic download and installation

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_iot_sdk_download.svg
   :align: center


For ``WM Dev Suite Path``:

- If you have already installed the Integrated Development Environment(IDE) by referring to :ref:`Set up command line compilation environment in Windows<windows-system>`, you can directly configure it.

::

  Installation path of the Integrated Development Environment(IDE)\winnermicro\wmclics

- If you have not installed the Integrated Development Environment(IDE) by referring to :ref:`Setting up command line compilation environment in Windows<windows-system>`, you can click ``Install`` and select the installation path for automatic installation configuration.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config_install.svg
   :align: center

- If you do not wish to use the Integrated Development Environment(IDE), you can also click ``Advance`` for advanced configuration and configure each item separately.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_win_config2.svg
   :align: center

Click ``Save`` after completing the configuration.

6. Create a project

There are three ways to create a project:

- Create Empty Project: Create an empty project, and you can add corresponding functions to the project after creation.
- Create Hello World Project: Create the simplest `Hello World` project for a quick experience.
- Import Project: Import various example projects from the WM IoT SDK examples directory to quickly master the use of related functions.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_creat_project.svg
   :align: center

After entering the project save location and project name in the project creation interface, the project will be generated.

7. Configure, compile, burn, and run on the created project

Select any project file, such as main.c, click main.c and then right-click:

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_build.svg
   :align: center

Follow the steps in the above picture:

  1. Select `menuconfig` to open the settings menu for project configuration.
  2. Select `build` to compile the project.
  3. Select `flash` to burn the firmware.

You can also select the corresponding command from the WM IoT SDK menu in the status bar at the bottom.

.. hint::

   `devconfig` command is used to configure the device table, please refer to :ref:`Device Table<lable_device_table>`
   `clean` command is used to clear all compilation generated files in the compilation directory while keeping configuration files
   `debug` command is used for debugging, please refer to :ref:`Online Debugging with Debugger<debug>`

After successfully burning the firmware, you can click `Serial Monitor` -> `Change Port Number` (consistent with the chip port number) -> `Change Baud Rate` -> `Run` to view the running results.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_log.svg
   :align: center


.. hint::

   * You can also skip step six to create a project and directly open the project under examples in wm_iot_sdk, then configure, compile, burn, and run it.

**Usage in Linux Environment**

The usage method and steps are completely the same as :ref:`Usage in Windows Environment<vscode_windows>`, the following describes the steps with differences:

1. Install VS Code

On Linux system, it is more **recommended** to use package management tools such as ``apt``, ``yum``, ``dnf``, etc., to automatically install related tools. The following is an example of using the apt command on the Ubuntu system:

::

   sudo apt update
   sudo apt install code

5. Configure the ``WM-IDE`` plugin

Before configuration, you need to obtain the WM IoT SDK software development framework source code from the `download page <https://doc.winnermicro.net/download/version>`__ and refer to :ref:`Set up command line compilation environment in Linux<linux-system>` to install the development compilation environment.

Complete the configurations shown in the diagram below, and the configuration paths need to be set according to the actual installation paths.

.. figure:: ../../_static/get_started/ide/wm_vscode_ide_linux_config.svg
   :align: center


.. hint::

   * To connect the development board, you need to install the corresponding USB to serial port (e.g., CH34X) driver.
   * If the serial port cannot be opened, it may be a serial port permission issue. Try adding the user to the dialout group with sudo gpasswd --add username dialout to gain serial port access permissions, or use sudo chmod 777 /dev/ttyUSB0 (depending on the actual serial port used) to grant read and write permissions to all users.