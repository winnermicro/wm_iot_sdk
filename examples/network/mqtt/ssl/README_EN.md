# MQTT over SSL

## Overview
This example connects to the broker URI by using MQTT SSL and as a demonstration subscribes/unsubscribes and send a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

PEM certificate for this example could be extracted from an openssl `s_client` command connecting to mqtt.eclipseprojects.io.
In case a host operating system has `openssl` and `sed` packages installed, one could execute the following command to download and save the root certificate to a file (Note for Windows users: Both Linux like environment or Windows native packages may be used).
```
echo "" | openssl s_client -showcerts -connect mqtt.eclipseprojects.io:8883 | sed -n "1,/Root/d; /BEGIN/,/END/p" | openssl x509 -outform PEM >mqtt_eclipseprojects_io.pem
```
Please note that this is not a general command for downloading a root certificate for an arbitrary host;
this command works with mqtt.eclipseprojects.io as the site provides root certificate in the chain, 
which then could be extracted with text operation.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Broker URL in menuconfig.
4. A PEM certificate is required. It defaults to mian/src/mqtt_eclipseprojects_io.pem.

## Building and Flashing

Example Location： `examples/network/mqtt/ssl`

For operations such as compiling and burning, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [4.992] got ip: 192.168.3.208
D/wm_nm_core      [4.998] WM_NM_EV post event 4 success
I/mqtt example    [5.002] wifi station start success, network is ready!
D/mqtt            [5.008] uri: ssl://mqtt.eclipseprojects.io:8883
D/mqtt            [5.016] mqtt_config mode: 0
D/mqtt            [5.020] ======================================
D/mqtt            [5.026] Trace Output
D/mqtt            [5.028] se Paho Synchronous MQTT C Client Library
D/mqtt            [5.034] se Paho Synchronous MQTT C Client Library
D/mqtt            [5.040] 17
D/mqtt            [5.044] ======================================
D/mqtt            [5.706] connect out: rc=119
D/mqtt            [5.710] wait connect, timeout: 29340
D/mqtt            [6.020] wait end
E/mqtt            [6.024]  client W800_29315F, packet_type 1, timeout 29340, pack 0x0

> Seeding the random number generator...
  ok
> Loading the CA root certificate ...
  CA root certificate is null.
  ok
> Loading the client cert. and key...
  Client certificate is null.
  Client privateKey is null.
> Setting up the SSL/TLS structure...
  ok
> Performing the SSL/TLS handshake...
  ok
SSLSocket_putdatas: mbedtls_ssl_write rc 27, iov_len 27
D/mqtt            [8.430] wait connect ack, timeout: 26620
D/mqtt            [8.692] wait end
E/mqtt            [8.696]  client W800_29315F, packet_type 2, timeout 26620, pack 0x0
D/mqtt            [8.706] MQTTClient connected!
I/mqtt example    [8.710] mqtt connected
D/mqtt            [8.714] mqtt client publish, topic /topic/qos1
SSLSocket_putdatas: mbedtls_ssl_write rc 23, iov_len 23
D/mqtt            [8.730] mqtt client publish, token: 1
I/mqtt example    [8.734] sent publish successful, msg_id=1
D/mqtt            [8.740] mqtt client subscribe, topic /topic/qos0, qos 0
SSLSocket_putdatas: mbedtls_ssl_write rc 18, iov_len 18
D/mqtt            [8.754] wait sub ack, timeout: 10000
D/mqtt            [8.994] Message with token value 1 delivery confirmed
D/mqtt            [9.252] wait end
E/mqtt            [9.256]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [9.264] sent subscribe successful, msg_id=2
D/mqtt            [9.270] mqtt client subscribe, topic /topic/qos1, qos 1
SSLSocket_putdatas: mbedtls_ssl_write rc 18, iov_len 18
D/mqtt            [9.286] wait sub ack, timeout: 10000
D/mqtt            [9.548] wait end
E/mqtt            [9.550]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [9.558] sent subscribe successful, msg_id=3
D/mqtt            [9.564] mqtt client unsubscribe, topic /topic/qos1
SSLSocket_putdatas: mbedtls_ssl_write rc 17, iov_len 17
D/mqtt            [9.580] wait unsub ack, timeout: 10000
D/mqtt            [9.838] wait end
E/mqtt            [9.842]  client W800_29315F, packet_type 11, timeout 10000, pack 0x0
I/mqtt example    [9.850] sent unsubscribe successful, msg_id=4
I/mqtt example    [9.854] mqtt published, msg_id=1
SSLSocket_putdatas: mbedtls_ssl_write rc 2, iov_len 2
SSLSocket_putdatas: mbedtls_ssl_write rc 2, iov_len 2
SSLSocket_putdatas: mbedtls_ssl_write rc 2, iov_len 2
D/mqtt            [208.546] Message arrived, topic [/topic/qos0], paload [data{a]
I/mqtt example    [208.554] mqtt recved data
```