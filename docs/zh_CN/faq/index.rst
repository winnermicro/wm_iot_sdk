FAQ
===================

本章节对用户使用中常见的问题进行汇总！


资源
-----------------

.. * 您可以通过 GitHub 的 `Issues <https://github.com/winnermicro/wm_iot_sdk/issues>`_  版块提交 bug 或功能请求。在提交新 Issue 之前，请先查看现有的 `Issues <https://github.com/winnermicro/wm_iot_sdk/issues>`_。

* 您可以在 `联盛德问答社区 <http://ask.winnermicro.com/ask/zone/wm_iot_sdk.html>`_ 中提出您的问题，访问社区资源。

* 通过 QQ 群参与互动，群号：570560884、335112873、923841628。

* 通过 Arduino 平台开发应用，请参考 `w80x_arduino <https://github.com/board707/w80x_arduino>`_。

* 通过 MicroPython 平台开发应用，请参考 `micropython-w806 <https://gitee.com/haorongMango/micropython-w806>`_。


编译
-------------------

* 可以不使用你们的 cmd 命令行编译吗？

  完全可以的，cmd 命令行仅是我们提供的一种编译环境示例，用户可以据此自行 diy 自己喜欢的编译环境。
  因为众口难调，没有哪个环境一定是最好的，只有适合自己的环境就是好的编译环境。
  用户可以参考编译环境搭建章节的内容搭建适合自己的环境，但是对于初学者建议仍然下载集成包使用。

* 为什么安装 python 依赖比较慢？

  可以通过替换默认源来提速，如国内用户可以选择清华源
  ::

    python -m pip install --user -r wm_iot_sdk/tools/wm/requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple

  其它源请自行选择使用。

* 代码改动不生效该怎么办？

  最常见的两种问题：切换不同版本的 WM IoT SDK 后、复制示例工程到其它地方后，修改其中的代码始终不生效。

  需要检查当前使用的 WM IoT SDK 版本和工程路径正确，可以通过如下步骤去排查：

  1. 删除工程目录下的 ``build`` 文件夹。

  2. 执行一次编译，观察编译产生的信息。

  3. 检查编译信息中 ``SDK_PATH`` 和 ``PROJECT_PATH`` 的值（分别对应当前所用的 WM IoT SDK 路径和工程路径），确认是否与您期望设置的一样。

  4. 若不一致，请参考编译环境搭建章节重新配置 WM IoT SDK 相关的环境变量。

     若使用 VS Code，则重启 VS Code 后，先在插件设置中重新配置 WM IoT SDK 路径，再到工程下启动编译。