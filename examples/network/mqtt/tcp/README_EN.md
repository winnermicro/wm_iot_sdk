# MQTT over TCP

## Overview
This example connects to the broker URI by using MQTT TCP and as a demonstration subscribes/unsubscribes and send a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Broker URL in menuconfig.

## Building and Flashing

Example Location： `examples/network/mqtt/tcp`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

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