# 基于 Websocket 的 MQTT

## 功能概述
这个示例使用 MQTT Websocket 连接到指定的 URI，并演示如何订阅/取消订阅主题和发送消息。
(请注意，所使用的公共 URI 可能不是永远可用的，详情请参阅此[免责声明](https://iot.eclipse.org/getting-started/#sandboxes))

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Broker URL。

## 编译和烧录

示例位置：`examples/network/mqtt/ws`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [5.666] got ip: 192.168.3.208
D/wm_nm_core      [5.672] WM_NM_EV post event 4 success
I/mqtt example    [6.176] wifi station start success, network is ready!
D/mqtt            [6.184] uri: ws://mqtt.eclipseprojects.io:80
D/mqtt            [6.190] ======================================
D/mqtt            [6.196] Trace Output
D/mqtt            [6.200] se Paho Synchronous MQTT C Client Library
D/mqtt            [6.206] se Paho Synchronous MQTT C Client Library
D/mqtt            [6.212] 16
D/mqtt            [6.216] ======================================
D/mqtt            [6.236] connect out: rc=119
D/mqtt            [6.240] wait connect, timeout: 29980
D/mqtt            [6.554] wait end
E/mqtt            [6.558]  client W800_29315F, packet_type 1, timeout 29980, pack 0x0
D/mqtt            [6.570] wait connect, timeout: 29652
D/mqtt            [6.848] wait end
E/mqtt            [6.850]  client W800_29315F, packet_type 1, timeout 29652, pack 0x0
D/mqtt            [6.862] wait connect ack, timeout: 29360
D/mqtt            [7.152] wait end
E/mqtt            [7.158]  client W800_29315F, packet_type 2, timeout 29360, pack 0x0
D/mqtt            [7.166] MQTTClient connected!
I/mqtt example    [7.170] mqtt connected
D/mqtt            [7.174] mqtt client publish, topic /topic/qos1
D/mqtt            [7.182] mqtt client publish, token: 1
I/mqtt example    [7.190] sent publish successful, msg_id=1
D/mqtt            [7.196] mqtt client subscribe, topic /topic/qos0, qos 0
D/mqtt            [7.206] wait sub ack, timeout: 10000
D/mqtt            [7.472] Message with token value 1 delivery confirmed
D/mqtt            [7.744] wait end
E/mqtt            [7.746]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [7.758] sent subscribe successful, msg_id=2
D/mqtt            [7.762] mqtt client subscribe, topic /topic/qos1, qos 1
D/mqtt            [7.772] wait sub ack, timeout: 10000
D/mqtt            [8.044] wait end
E/mqtt            [8.048]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [8.058] sent subscribe successful, msg_id=3
D/mqtt            [8.064] mqtt client unsubscribe, topic /topic/qos1
D/mqtt            [8.074] wait unsub ack, timeout: 10000
D/mqtt            [8.344] wait end
E/mqtt            [8.348]  client W800_29315F, packet_type 11, timeout 10000, pack 0x0
I/mqtt example    [8.356] sent unsubscribe successful, msg_id=4
```