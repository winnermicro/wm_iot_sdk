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

* 可以不使用你们的 cmd 命令行编译吗?

  完全可以的，cmd 命令行仅是我们提供的一种编译环境示例，用户可以据此自行 diy 自己喜欢的编译环境。
  因为众口难调，没有哪个环境一定是最好的，只有适合自己的环境就是好的编译环境。
  用户可以参考编译环境搭建章节的内容搭建适合自己的环境，但是对于初学者建议仍然下载集成包使用。

* 为什么安装 python 依赖比较慢

  可以通过替换默认源来提速，如国内用户可以选择清华源

::

  pip install package -i https://pypi.tuna.tsinghua.edu.cn/simple

其它源请自行选择使用。
