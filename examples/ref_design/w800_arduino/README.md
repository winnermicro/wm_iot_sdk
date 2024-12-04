# W800 Arduino Board Reference Project

## 功能概述

此工程演示如何基于 W800_ARDUINO_Vx.x 硬件开发板，进行 Device Table 的配置, 并集成 CLI 模组，和集成基础的 WiFi, System， PWM 等 cli command, 
让用户能快速掌握 WM IOT SDK v2.x 的基础的联网和硬件操作功能。

需要注意的是：
WM IOT SDK v2.x 目前并不支持 Arduino IDE。
WM IOT SDK v2.x 所支持的开发环境请参考：[搭建编译环境](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html#compiling-environment)


## 环境要求

联盛德的官方 W800_ARDUINO_Vx.x 开发板，电脑通过 USB 线接到开发板的 UART0 口。
将 W800_ARDUINO_Vx.x 开发板 K1 拨码开关全部拨到 ON。
电脑端需要 安装 串口工具，比如：SecureCRT ，TeraTeam ，串口调试助手 等。
串口配置： Baudrate=115200， DataBits=8,   Stop bits=1,  Parity=None,   无 RTS/CTS

此工程 UART0 默认情况下包含的功能有：
1. 固件下载(从电脑下载到 W800 的内置 Flash 中)
2. LOG 打印
3. CLI 操作

## 操作步骤说明

程序运行后，直接会再 串口工具上 打印出相关信息，按照提示进行 CLI 命令操作即可。
比如：直接输入 help 可显示出 所有支持的 Cli 命令列表

## 编译和烧录

示例位置：`examples/ref_design/w800_arduino`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

上电后成功运行将输出如下日志

```
W800: heap free size=258416(bytes)
partition table info:
  name             offset     size       flag
  ft               0x00000000 0x00002000 0x00
  bootloader       0x00002000 0x0000c000 0x00
  partition_table  0x0000e000 0x00001000 0x00
  app              0x0000f000 0x001e1000 0x00
  nvs              0x001f0000 0x00008000 0x00

 ================================================================================
 |  W800 Demo Project on W800_ARDUINO_Vx.x Board                                |
 |                                                                              |
 |  1. Print 'help' to get overview of commands                                 |
 |  2. Configure device to station                                              |
 |                                                                              |
 |  3. Setup WiFi connection                                                    |
 |     Suggest preparing an AP router or setting up a hotspot on a phone with   |
 |     accessible internet. Once the W800 connects to the AP or hotspot,        |
 |     it will be able to access (ping or use HTTP) internet resources.         |
 |                                                                              |
 |  4. Run ifconfig to check ip info                                            |
 |  5. Run ping to check internet/ethernet link                                 |
 |  6. Run http to download specific webpage                                    |
 |     Put following cmd to know more for this activity from webpage            |
 |     CMD: http client get https://doc.winnermicro.net/download/html/w800.txt  |
 |  7. Run pwm to control RGB led on Arduino Board                              |
 |                                                                              |
 ================================================================================

 =========================================================
 | Tech Support                                          |
 |                                                       |
 | SDK Introduction Onsite Page:                         |
 |     https://doc.winnermicro.net                       |
 |                                                       |
 | SDK Packag Download Page:                             |
 |     https://doc.winnermicro.net/download/version      |
 =========================================================
```

## CLI 示例说明
### WiFi 示例

- WiFi 初始化：`wifi init`
- WiFi 联网： `wifi connect ssid password`

进行 WiFi 相关的所有操作之前，必须要先执行一次 WiFi 初始化（不必重复执行）。
当 WiFi 联网成功之后，会提示 `sta connected`。

ssid 即为网络名称（如路由器显示的无线名称、手机热点显示的名称），password 即为无线网络的密码。
如路由器的名称为 my_wifi，密码为 12345678，则联网时输入 `wifi connect my_wifi 12345678`。

当 WiFi 联网成功之后，可以使用命令 `ifconfig` 查询当前网络状态和获得的 IP 地址。

### Ping 示例

当 WiFi 联网成功之后，可以使用 Ping 命令对 IP 地址或域名进行测试：

```
Usage
  ping [options] <destination>

Options:
  <destination>      dns name or ip address
  -c <count>         stop after <count> replies
  -i <interval>      milliseconds between sending each packet
  -s <size>          use <size> as number of data bytes to be sent
  -t <ttl>           define time to live
  -q <tos>           define type of service
  -w <timeout>       milliseconds to wait for response
  -h                 print help and exit
```

如对百度进行 Ping 测试：`ping www.baidu.com`。

对互联网进行 Ping 访问时，请让 W800 开发板 通过 WiFi 连接上 具有 Internet 访问能力的路由器 或 手机热点。

### HTTP Client 示例

当 WiFi 联网成功之后，可以使用 HTTP Client 命令进行 HTTP 下载并显示：

```
http usage:
  Retrieve HTTP resources: http client get [url]
```

如对下载 "w800.txt" 文件并显示：

`http client get https://doc.winnermicro.net/download/html/w800.txt`。

对互联网进行 Ping 访问时，请让 W800 开发板 通过 WiFi 连接上 具有 Internet 访问能力的路由器 或 手机热点。


### PWM CLI 示例

上电后可以使用 CLI 命令 `pwm -h` 查看帮助手册
```
W800: pwm -h

Usage
  pwm [options]

Options:
  -r <color value>         set red color value [0, 255]
  -g <color value>         set green color value [0, 255]
  -b <color value>         set blue color value [0, 255]
  -h                       print help and exit
```

在 W800_ARDUINO_Vx.x 开发板的硬件参考设计中 RGB LED 灯通过 3 根 PWM 信号进行控制，LED灯与 PWM channel 之间的映射关系如下
下面是您提供的信息，整理成Markdown表格的格式：

| PWM Channel | LED Color |
|-------------|-----------|
| PWM Channel 0 | LED Green |
| PWM Channel 1 | LED Red   |
| PWM Channel 2 | LED Blue |

注意 W800_ARDUINO_Vx.x 开发板的硬件参考设计中，占空比越高 LED 灯亮度越暗，
为方便用户直观感受， 在软件 CLI 示例中有对应转换，以常用的 RGB 三色数值 做为参数表示。
例如： `pwm -r 255 -g 255 -b 255` 设定为高亮全白

| 颜色   |    对应RGB占空比值     | 命令                                   |
| ------ | ---------------------- | -------------------------------------- |
| 青色   | (0, 255, 255)          | pwm -r 0 -g 255 -b 255                 |
| 紫色   | (255, 0, 255)          | pwm -r 255 -g 0 -b 255                 |
| 黄色   | (255, 255, 0)          | pwm -r 255 -g 255 -b 0                 |
| 蓝色   | (0, 0, 255)            | pwm -r 0 -g 0 -b 255                   |
| 红色   | (255, 0, 0)            | pwm -r 255 -g 0 -b 0                   |
| 绿色   | (0, 255, 0)            | pwm -r 0 -g 255 -b 0                   |
| 关灯   | (0, 0, 0)              | pwm -r 0 -g 0 -b 0                     |
| 白色   | (255, 255, 255)        | pwm -r 255 -g 255 -b 255               |
| 灰色   | (10, 10, 10)           | pwm -r 10 -g 10 -b 10                  |

