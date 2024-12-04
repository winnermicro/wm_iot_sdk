# WinnerMicro Documentation 

* [English](README_EN.md)

这里是 WM IoT SDK 的[文档中心](https://doc.winnermicro.net) 。

## 安装依赖

- 安装 doxygen，根据所使用系统自行安装
- 安装 python，版本不得低于 3.8
-  安装 docs 目录下的 python 库依赖：`python -m pip install --user -r requirements.txt`

## 编译文档

在 `wm_iot_sdk/docs` 目录中，执行命令 `python build.py` 编译文档。

构建结果在 `wm_iot_sdk/docs/build` 目录，按芯片类型、语言类型、版本类型分类放置于各自的 `html` 文件夹中。

> 说明：清除编译结果需要手动删除 `build` 文件夹。
>

## 查看文档

将 `build` 文件夹中的 html 文件放至于 web 服务器，然后使用浏览器访问该 web 服务器即可。

> 说明：可以直接本地双击网页打开阅读使用，默认起始页面为 index.html。
>

## 注意事项

本文档使用 Sphinx 技术构建，使用 Windows 命令行编译环境需更新到 0.0.13 及以上版本才能支持当前文档编译，否则需要自行安装 Sphinx 相关工具。
