# cJSON 示例

## 功能概述

此应用程序启动后执行下面如下操作：

1. 创建 cJSON 根节点
2. 向根节点添加字符串、数字等内容
3. 创建叶子节点，向叶子节点添加字符串、数字等内容，完成后将叶子节点添加到根节点
4. 创建叶子节点，向叶子节点添加数组，完成后将叶子节点添加到根节点
5. 添加 True, NULL 等对象到根节点
6. 将根节点转换成字符串并打印
7. 释放根节点

## 编译和烧录

示例位置：`examples/storage/cjson`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/cjson_example   [0.004] start cJSON demo...
{
        "Name": "Jack",
        "Age":  28,
        "Weight":       68,
        "Information":  {
                "Company":      "WinnerMicro",
                "Department":   "SoftwareDepartment",
                "Number":       8
        },
        "Score":        [18, 20, 23, 25],
        "IsMan":        true,
        "Other":        null
}
```
