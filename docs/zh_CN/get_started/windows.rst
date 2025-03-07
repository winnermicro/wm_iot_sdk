
.. _windows-system:

Windows 搭建命令行编译环境
===========================

**下载集成包安装编译环境**

在 Windows 系统下，我们提供了一键安装的 `编译环境集成包 <https://doc.winnermicro.net/download/version/index.html#wincc>`_，
安装路径中不能包含空格等特殊字符，安装的默认路径为 ``D:\winnermicro\wmclics``。


.. important::
    - 安装集成编译环境前，需提前下载好 WM IoT SDK。
    - 使用集成编译环境时，每次启动都需要从安装程序提供的快捷方式启动编译环境（桌面快捷方式、开始菜单快捷方式或右键菜单快捷方式），不能直接启动 Windows 系统自带的 CMD 或 PowerShell。
    - 喜欢 DIY 的用户，请参考 Linux 系统手动搭建编译环境。
    - 当使用集成编译环境时遇到 wm.py 不存在问题时，这可能被某些杀毒软件当作威胁删除，建议对本机杀毒软件作相应处理。
    - 当前使用 Ccache 加速编译过程，部分杀毒软件可能对这种机制检测比较费时造成编译卡顿，如果遇到这种情况，建议对本机杀毒软件作相应处理，予以提升编译速度，但也可在 menuconfig 中选择关闭 Ccache 特性。
    - 使用 CMD 编译环境时，可能存在窗口卡住不动的情况，遇到时可在敲击回车键后继续执行，为了避免这种问题，推荐使用 PowerShell 环境。


当使用集成包编译环境想切换到另一份 WM IoT SDK 时，需在生成的快捷方式图标上右键选择属性，然后在起始位置(S):处填入新的 WM IoT SDK 路径即可。


当然，直接在命令行窗口中修改 `WM_IOT_SDK_PATH` 一样可以达到目的，如：

在 CMD 中

.. code-block:: text

  set WM_IOT_SDK_PATH=D:\wm\wm_iot_sdk_new

或在 PowerShell 中

.. code-block:: text

  $env:WM_IOT_SDK_PATH="D:\wm\wm_iot_sdk_new"
