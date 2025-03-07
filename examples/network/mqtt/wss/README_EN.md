# MQTT over WSS

## Overview
This example connects to the broker URI over using MQTT WSS and as a demonstration subscribes/unsubscribes and sending a message on certain topic.
(Please note that the public broker is maintained by the community so may not be always available, for details please see this [disclaimer](https://iot.eclipse.org/getting-started/#sandboxes))

PEM certificate for this example could be extracted from an openssl `s_client` command connecting to mqtt.eclipseprojects.io.
In case a host operating system has `openssl` and `sed` packages installed, one could execute the following command to download and save the root certificate to a file (Note for Windows users: Both Linux like environment or Windows native packages may be used).
```
echo "" | openssl s_client -showcerts -connect mqtt.eclipseprojects.io:443 | sed -n "1,/Root/d; /BEGIN/,/END/p" | openssl x509 -outform PEM >mqtt_eclipseprojects_io.pem
```
Please note that this is not a general command for downloading a root certificate for an arbitrary host;
this command works with mqtt.eclipseprojects.io as the site provides root certificate in the chain, 
which then could be extracted with text operation.

## Requirements
1. A working AP is required.
2. Configure WiFi SSID and WiFi Password in menuconfig.
3. Configure the Broker URL in menuconfig.
4. The PEM certificate is required. It defaults to mian/src/mqtt_eclipseprojects_io.pem, mian/src/mqtt_eclipseprojects_io.crt, mian/src/mqtt_eclipseprojects_io.key.

## Building and Flashing

Example Location： `examples/network/mqtt/wss`

compile, burn, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)


## Running result

Upon successful execution, it will output the following logs

```
I/wm_nm_core      [5.488] got ip: 192.168.3.208
D/wm_nm_core      [5.492] WM_NM_EV post event 4 success
I/mqtt example    [5.996] wifi station start success, network is ready!
D/mqtt            [6.004] uri: wss://mqtt.eclipseprojects.io:443
D/mqtt            [6.012] mqtt_config mode: 3
D/mqtt            [6.016] mode: MQTT_AUTH_MODE_MUTUAL_AUTH
D/mqtt            [6.862] ======================================
D/mqtt            [6.868] Trace Output
D/mqtt            [6.872] se Paho Synchronous MQTT C Client Library
D/mqtt            [6.878] se Paho Synchronous MQTT C Client Library
D/mqtt            [6.884] 55
D/mqtt            [6.886] ======================================
D/mqtt            [6.906] connect out: rc=119
D/mqtt            [6.910] wait connect, timeout: 29982
D/mqtt            [7.224] wait end
E/mqtt            [7.228]  client W800_29315F, packet_type 1, timeout 29982, pack 0x0

> Seeding the random number generator...
  ok
> Loading the CA root certificate ...
  ok
> Loading the client cert. and key...
  ok (key type: RSA)
> Setting up the SSL/TLS structure...
  ok
> Performing the SSL/TLS handshake...
  ok
SSLSocket_putdatas: mbedtls_ssl_write rc 245, iov_len 245
D/mqtt            [9.730] wait connect, timeout: 27164
D/mqtt            [10.024] wait end
E/mqtt            [10.028]  client W800_29315F, packet_type 1, timeout 27164, pack 0x0
SSLSocket_putdatas: mbedtls_ssl_write rc 33, iov_len 33
D/mqtt            [15.034] wait connect ack, timeout: 21860
D/mqtt            [15.312] wait end
E/mqtt            [15.316]  client W800_29315F, packet_type 2, timeout 21860, pack 0x0
D/mqtt            [20.312] MQTTClient connected!
I/mqtt example    [20.316] mqtt connected
D/mqtt            [20.320] mqtt client publish, topic /topic/qos1
SSLSocket_putdatas: mbedtls_ssl_write rc 29, iov_len 29
D/mqtt            [20.336] mqtt client publish, token: 1
I/mqtt example    [20.342] sent publish successful, msg_id=1
D/mqtt            [20.348] mqtt client subscribe, topic /topic/qos0, qos 0
SSLSocket_putdatas: mbedtls_ssl_write rc 24, iov_len 24
D/mqtt            [20.362] wait sub ack, timeout: 10000
D/mqtt            [20.610] Message with token value 1 delivery confirmed
D/mqtt            [20.886] wait end
E/mqtt            [20.890]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [20.898] sent subscribe successful, msg_id=2
D/mqtt            [20.906] mqtt client subscribe, topic /topic/qos1, qos 1
SSLSocket_putdatas: mbedtls_ssl_write rc 24, iov_len 24
D/mqtt            [20.922] wait sub ack, timeout: 10000
D/mqtt            [21.194] wait end
E/mqtt            [21.198]  client W800_29315F, packet_type 9, timeout 10000, pack 0x0
I/mqtt example    [21.206] sent subscribe successful, msg_id=3
D/mqtt            [21.212] mqtt client unsubscribe, topic /topic/qos1
SSLSocket_putdatas: mbedtls_ssl_write rc 23, iov_len 23
D/mqtt            [21.228] wait unsub ack, timeout: 10000
D/mqtt            [21.506] wait end
E/mqtt            [21.510]  client W800_29315F, packet_type 11, timeout 10000, pack 0x0
I/mqtt example    [21.518] sent unsubscribe successful, msg_id=4
I/mqtt example    [21.524] mqtt published, msg_id=1
```