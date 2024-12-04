# 基于 TCP 的 MQTT

## 功能概述
这个示例使用 MQTT TCP 连接到指定的 URI，并演示如何订阅/取消订阅主题和发送消息。
(请注意，所使用的公共 URI 可能不是永远可用的，详情请参阅此[免责声明](https://iot.eclipse.org/getting-started/#sandboxes))

## 环境要求

1. 需要有一个能正常连接的 AP。
2. 在 menuconfig 中配置 WiFi SSID 和 WiFi Password。
3. 在 menuconfig 中配置 Broker URL。

## 编译和烧录

示例位置：`examples/network/mqtt/tcp`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
I/wm_nm_core      [4.264] got ip: 192.168.3.208
D/wm_nm_core      [4.268] WM_NM_EV post event 4 success
I/mqtt example    [4.778] wifi station start success, network is ready!
D/mqtt            [4.786] uri: mqtt.eclipseprojects.io
D/mqtt            [4.792] ======================================
D/mqtt            [4.796] Trace Output
D/mqtt            [4.800] se Paho Synchronous MQTT C Client Library
D/mqtt            [4.806] se Paho Synchronous MQTT C Client Library
D/mqtt            [4.812] 13
D/mqtt            [4.816] ======================================
D/mqtt            [5.676] connect out: rc=119
D/mqtt            [5.680] wait connect, timeout: 29142
D/mqtt            [5.990] wait end
E/mqtt            [5.994]  client W800_29315F, packet_type 1, timeout 29142, pack 0x0
D/mqtt            [6.006] wait connect ack, timeout: 28816
D/mqtt            [6.270] wait end
E/mqtt            [6.274]  client W800_29315F, packet_type 2, timeout 28816, pack 0x0
D/mqtt            [6.282] MQTTClient connected!
I/mqtt example    [6.288] mqtt connected
D/mqtt            [6.290] mqtt client publish, topic /topic/qos1
D/mqtt            [6.300] mqtt client publish, token: 1
I/mqtt example    [6.306] sent publish successful, msg_id=1
D/mqtt            [6.310] mqtt client subscribe, topic /topic/qos0, qos 0
D/mqtt            [6.320] wait sub ack, timeout: 10000
D/mqtt            [6.590] Message with token value 1 delivery confirmed
D/mqtt            [6.932] wait end
E/mqtt            [6.934]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [6.942] sent subscribe successful, msg_id=2
D/mqtt            [6.948] mqtt client subscribe, topic /topic/qos1, qos 1
D/mqtt            [6.958] wait sub ack, timeout: 10000
D/mqtt            [6.966] Message arrived, topic [/topic/qos0], paload [Hello World]
D/mqtt            [7.224] wait end
E/mqtt            [7.228]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [7.236] sent subscribe successful, msg_id=3
D/mqtt            [7.242] mqtt client unsubscribe, topic /topic/qos1
D/mqtt            [7.252] wait unsub ack, timeout: 10000
D/mqtt            [7.520] wait end
E/mqtt            [7.524]  client W800_29315F, packet_type 11, timeout 10000, pack 0x0
I/mqtt example    [7.532] sent unsubscribe successful, msg_id=4
I/mqtt example    [7.538] mqtt published, msg_id=1
I/mqtt example    [7.542] mqtt recved data
```