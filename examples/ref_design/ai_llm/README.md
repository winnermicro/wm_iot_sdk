# HTTP 客户端

## 功能概述
本例演示了如何 通过 AT Command 利用 http client 组件 访问云端大语言模型(比如豆包，Deepseek)。

## 环境要求

1. 需要有一台 AP 路由器 或手机热点, 能够连接到互联网。
2. 需要使用者具有 自己对应的大语言模型 API KEY，并在代码中 llm_cli_cmd.c 进行相关宏定义内容的替换。如：WM_LLM_DS_KEY，WM_LLM_DS_MODEL。
   有关如何申请对应的大预言模型 API KEY 可去其官网上找寻
3. 准备一台电脑，有安装串口调试工具，默认 Uart Baudrate 115200, 用于向 W80X 输入CLI命令 及显示响应内容。

## 编译和烧录

示例位置：`examples/ref_design/ai_llm`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 注意

1. 此工程默认的 API KEY 可能会失效而无法连接 大语言模型服务器。 请使用者 务必自行申请有效的 API KEY，并替换代码中相应的宏定义内容。
2. 此工程采用 CLI 组件进行用户信息输入，其长度在 `CONFIG_CLI_LINE_SIZE` 宏定义进行设定，目前该工程已在 `prj.config` 中将其调整成默认值 256 Byte。
用户 若有更长长度的信息输入需求 可以在 `prj.config` ，或者 `menuconfig` 界面做相应调整。  否则遇到超出长度的信息，系统会提示 `Warning: Command is too long`
3. 用户需要在 menuconfig 中配置自己所布置的路由器的 WiFi SSID 和 WiFi Password， 这样 W80X 系统上电后 会尝试自动连接此路由器。
4. 目前系统集成的是 deepseek-chat 大语言模型， 和豆包的 doubao-lite-128k-240828 大语言模型， 它们的数据通信 都采用 HTTP + CJSON 的传输方式和内容格式。举例豆包的 curl 格式如下， 此工程也主要是构造其 HTTP Client Header 以及 cJSON 格式数据内容，并通过 HTTP Client API 对 URL指定的服务器进行消息发送。
```
curl https://ark.cn-beijing.volces.com/api/v3/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer 8eccdf3b-3012-4418-9e54-93af922decad" \
  -d '{
    "model": "doubao-lite-128k-240828",
    "messages": [
      {"role": "user","content": "常见的十字花科植物有哪些？"}
    ]
  }'
```

## 运行结果

1. W80X 在系统上电后，通过串口输入 llm 字符串命令，可以返回命令说明。
```
llm

Usage
  llm [options]

Options:
  ds <contents>   send contents to deepseek llm, max len < CONFIG_CLI_LINE_SIZE Bytes
  db <contents>   send contents to doubao llm, max len < CONFIG_CLI_LINE_SIZE Bytes
  help            print help and exit

W800: 
```

2. 用户可以继续参考命令说明输入与大语言模型交互的信息， 比如输入 `llm db 写一首夏天的诗`

成功运行将输出如下日志

```
W800: 
W800: llm db 写一首夏天的诗
[I] (783055) llm: WM_HTTP_CLIENT_EVENT_CONNECTED
[I] (783055) llm: WM_HTTP_CLIENT_EVENT_HEADER_SENTED
[I] (785098) llm: WM_HTTP_CLIENT_EVENT_RECV_HEADER
[I] (785098) llm: Start to receive data from remote server...
[I] (785099) llm: WM_HTTP_CLIENT_EVENT_RECV_DATA
[I] (785099) llm: 

【LLM Answer】:
assistant:  《夏韵》

夏日骄阳似火烧，
荷塘翠叶展娇娆。
蝉鸣高树声声闹，
蛙鼓池塘阵阵敲。
蒲扇轻摇消酷暑，
冰糕入口沁心潮。
时光沉醉清欢里，
诗意 summer 韵味饶。 
[I] (785100) llm: 

【LLM Answer End】:
[I] (785101) llm: WM_HTTP_CLIENT_EVENT_DISCONNECTED
```
