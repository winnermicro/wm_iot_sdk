Kconfig 配置
****************

简介
============

Kconfig 提供了编译时的项目配置机制，以及多种类型的配置选项（如整数、字符串和布尔值等）。
Kconfig 文件指定了选项之间的依赖关系、默认值和组合方式等。
WM IoT SDK 使用 kconfiglib_ 包（kconfiglib_ 是 Kconfig_ 系统的 Python 扩展）。 

了解所有可用功能，请查看 Kconfig_ 和 `kconfiglib 扩展`_。

配置菜单
============

用户可以在终端中打开项目配置菜单，通过 ``wm.py menuconfig`` 配置工程。

.. figure:: ../../_static/component-guides/kconfig/menuconfig.png
    :align: center
    :alt: menuconfig

在 menuconfig 界面最下方，有提示此界面操作方法，可阅读了解。
最常用的操作为：使用 H、J、K、L 键移动光标(使用键盘方向键也可)，按空格键选中或取消选中，按回车键选中或进入下一级菜单（选项），按 ESC 键退出或返回上级菜单。
当配置修改后，请按 S 键保存配置，否则退出菜单后所有的修改将丢失，按下 S 键后弹出的保存路径无需修改，按回车键确认即可。

配置保存在项目根目录的 ``build\config\wmsdk.config`` 文件中，并在构建目录中生成 ``build\config\wmsdk_config.h`` 文件。
所以，删除 ``build\config`` 文件夹可以让当前工程恢复默认配置。

请注意，menuconfig 界面只支持键盘操作，无法使用鼠标，想使用鼠标的话请输入 ``wm.py guiconfig`` 配置工程。


工程默认配置
================

虽然 Kconfig 在定义每个选项时有默认配置，但是对于每个不同的工程，可能仍然需要一些不同的配置，
所以为了能让每个工程都能具有属于自己的默认配置，WM IoT SDK 可以在工程根目录下放置 ``prj.config`` 文件，
当打开 ``menuconfig`` 界面后，就会加载 ``prj.config`` 文件中的配置，显示出当前工程的配置，在编译时生效。

建议用户使用 ``prj.config`` 文件对工程的用户客制化配置做管理，避免在删除工程中编译生成的 ``build`` 文件夹后，所有在 ``menuconfig`` 的客制化配置也被删除，这会造成因配置项不匹配而在重新编译时产生编译错误，须要用户使用 ``menuconfig`` 对项目进行完整重新配置。

当用户在 ``menuconfig`` 界面进行了修改并保存配置后，依旧会生成 ``build\config\wmsdk.config`` 文件，之后的配置以 ``wmsdk.config`` 为准。
对于一个工程而言，最终实际生效的 Kconfig 配置文件的优先级为 ``wmsdk.config`` > ``prj.config`` > ``Kconfig``。

``prj.config`` 文件的格式同 ``wmsdk.config``，可以在对工程完成配置后，执行命令 ``wm.py saveconfig`` 生成，也可自行编辑生成。


配置选项
==================

按照一般的使用习惯，所有选项名称均为大写字母加下划线。
当 Kconfig 生成 ``wmsdk.config`` 和 ``wmsdk_config.h`` 文件时，生成的选项名称会以 ``CONFIG_`` 为前缀，对于 ``wmsdk_config.h`` 则是 ``CONFIG_`` 为前缀的宏。
例如 Kconfig 文件定义了 ``ENABLE_FOO`` 选项且 ``menuconfig`` 中选择了该选项，则 ``wmsdk.config`` 和 ``wmsdk_config.h`` 文件也将定义 ``CONFIG_ENABLE_FOO``，对于 ``wmsdk_config.h`` 则是 ``#define CONFIG_ENABLE_FOO`` 宏。

用户编程时，在 .c 或 .h 文件中包含 ``wmsdk_config.h`` 即可引用 Kconfig 中生成的宏，例如：

::

  #include "wmsdk_config.h"

请注意，由于所选选项的不同，某些选项可能在 ``menuconfig`` 界面中默认不可见。

.. _Kconfig: https://www.kernel.org/doc/Documentation/kbuild/kconfig-language.txt
.. _kconfiglib: https://github.com/ulfalizer/Kconfiglib
.. _kconfiglib 扩展: https://pypi.org/project/kconfiglib/#kconfig-extensions
