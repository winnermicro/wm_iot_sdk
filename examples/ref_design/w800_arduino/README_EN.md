# W800 Arduino Board Reference Project

## Functional Overview

This project demonstrates how to configure the Device Table based on the W800_ARDUINO_Vx.x hardware development board, integrate the CLI module, and integrate basic CLI commands such as those for WiFi, System, and PWM, allowing users to quickly grasp the basic networking and hardware operation functions of WM IOT SDK v2.x.

It should be noted that:：
WM IOT SDK v2.x currently does not support Arduino IDE.
For the development environment supported by WM IOT SDK v2.x, please refer to: [Setup compiling environment](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html#compiling-environment)


## Environmental Requirements

The official W800_ARDUINO_Vx.x development board from Winner Micro. Connect the computer to the UART0 port of the development board via a USB cable.
Set all the dip switches (K1) on the W800_ARDUINO_Vx.x development board to ON.
The computer side needs to install serial port tools, such as SecureCRT, TeraTeam, Serial Port Debugging Assistant, etc.
Serial port configuration: Baudrate = 115200, DataBits = 8, Stop bits = 1, Parity = None, without RTS/CTS.

The functions included in UART0 by default in this project are:
1. Firmware download (from the computer to the built-in Flash of W800)
2. LOG printing
3. CLI operations

## Instructions for Operating Steps

After the program runs, relevant information will be directly printed on the serial port tool. Just perform CLI command operations according to the prompts.
For example, directly entering "help" will display a list of all supported Cli commands.

## Compilation and Burning

Example Location: `examples/ref_design/w800_arduino`

For compilation, burning, and more, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/2.2-beta.2/get_started/index.html)


## Running Results

After successful power-on and running, the following logs will be output:

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

## CLI Example Illustrations
### WiFi Example

- WiFi Initialization: `wifi init`
- WiFi Networking： `wifi connect ssid password`

Before performing all operations related to WiFi, it is necessary to execute WiFi initialization once (no need to repeat). After successful WiFi networking, it will prompt `sta connected`。

The "ssid" is the network name (such as the wireless name displayed by the router or the name displayed by the phone hotspot), and the "password" is the password of the wireless network. For example, if the router's name is "my_wifi" and the password is "12345678", then when networking, enter `wifi connect my_wifi 12345678`.

After successful WiFi networking, you can use the command `ifconfig` to query the current network status and obtain the IP address.

### Ping Example

After successful WiFi networking, you can use the Ping command to test IP addresses or domain names:

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

For example, to perform a Ping test on Baidu: `ping www.baidu.com`。

When performing Ping access to the Internet, please ensure that the W800 development board is connected to a router or phone hotspot with Internet access capabilities via WiFi.

### HTTP Client Example

After successful WiFi networking, you can use the HTTP Client command to perform HTTP downloads and display:

```
http usage:
  Retrieve HTTP resources: http client get [url]
```

For example, to download and display the "w800.txt" file:

`http client get https://doc.winnermicro.net/download/html/w800.txt`。

When performing Ping access to the Internet, please ensure that the W800 development board is connected to a router or phone hotspot with Internet access capabilities via WiFi.


### PWM CLI Example

After power-on, you can use the CLI command `pwm -h` to view the help manual:

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

In the hardware reference design of the W800_ARDUINO_Vx.x development board, the RGB LED is controlled by 3 PWM signals. The mapping relationship between the LED lights and the PWM channels is as follows:

| PWM Channel | LED Color |
|-------------|-----------|
| PWM Channel 0 | LED Green |
| PWM Channel 1 | LED Red   |
| PWM Channel 2 | LED Blue |

It should be noted that in the hardware reference design of the W800_ARDUINO_Vx.x development board, the higher the duty cycle, the darker the LED light. For the convenience of users to have an intuitive feeling, there is a corresponding conversion in the software CLI examples, using the commonly used RGB three-color values as parameters for representation.

For example: `pwm -r 255 -g 255 -b 255` is set to high-brightness full white.

| Color  | RGB Duty Cycle Values  | Command                                |
| ------ | ---------------------- | -------------------------------------- |
| Cyan   | (0, 255, 255)          | pwm -r 0 -g 255 -b 255                 |
| Purple | (255, 0, 255)          | pwm -r 255 -g 0 -b 255                 |
| Yellow | (255, 255, 0)          | pwm -r 255 -g 255 -b 0                 |
| Blue   | (0, 0, 255)            | pwm -r 0 -g 0 -b 255                   |
| Red    | (255, 0, 0)            | pwm -r 255 -g 0 -b 0                   |
| Green  | (0, 255, 0)            | pwm -r 0 -g 255 -b 0                   |
| Off    | (0, 0, 0)              | pwm -r 0 -g 0 -b 0                     |
| White  | (255, 255, 255)        | pwm -r 255 -g 255 -b 255               |
| Gray   | (10, 10, 10)           | pwm -r 10 -g 10 -b 10                  |

