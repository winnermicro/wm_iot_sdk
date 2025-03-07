# iPerf 

## 功能概述
这个示例实现了通用性能测量工具 [iPerf](https://iperf.fr/) 使用的协议。可以在运行此示例的两个 W800 设备之间，或者在单个 W800 设备与运行 iPerf 工具的计算机之间测量性能。

iperf 版本为 3.x， 但本 iperf 示例并不支持标准 iperf 中的所有特性。

## 环境要求

1.  需要有一个能正常连接的AP。
1.  打印` help `以获得命令的概述。
1.  设置 Wi-Fi Station 连接。
1.  运行 iperf3 测试 UDP/TCP RX/TX 的吞吐量。

## 编译和烧录

示例位置：`examples/benchmark/iperf `

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
 __          ____  __   _____   _______      ____   _____    _____ _          _ _ 
 \ \        / /  \/  | |_   _| |__   __|    / __ \ / ____|  / ____| |        | | |
  \ \  /\  / /| \  / |   | |  ___ | |______| |  | | (___   | (___ | |__   ___| | |
   \ \/  \/ / | |\/| |   | | / _ \| |______| |  | |\___ \   \___ \| '_ \ / _ \ | |
    \  /\  /  | |  | |  _| || (_) | |      | |__| |____) |  ____) | | | |  __/ | |
     \/  \/   |_|  |_| |_____\___/|_|       \____/|_____/  |_____/|_| |_|\___|_|_|
                                                                                  

W800: 
 ===================================================
 |       Steps to test WiFi throughput             |
 |                                                 |
 |  1. Print 'help' to gain overview of commands   |
 |  2. Configure device to station or softap       |
 |  3. Setup WiFi connection                       |
 |  4. Run iperf3 to test UDP/TCP RX/TX throughput |
 |                                                 |
 ===================================================

W800: 
W800: iperf3 -h
Usage: iperf3 [-s|-c host] [options]
       iperf3 [-h|--help] [-v|--version]

Server or Client:
  -p, --port      #         server port to listen on/connect to
  -f, --format   [kmgtKMGT] format to report: Kbits, Mbits, Gbits, Tbits
  -i, --interval  #         seconds between periodic throughput reports
  -I, --pidfile file        write PID file
  -F, --file name           xmit/recv the specified file
  -B, --bind      <host>    bind to the interface associated with the address <host>
  -V, --verbose             more detailed output
  -J, --json                output in JSON format
  --logfile f               send output to a log file
  --forceflush              force flushing output at every interval
  --timestamps<=format>     emit a timestamp at the start of each output line
                            (optional "=" and format string as per strftime(3))
  --rcv-timeout #           idle timeout for receiving data (default 120000 ms)
  -d, --debug[=#]           emit debugging output
                            (optional optional "=" and debug level: 1-4. Default is 4 - all messages)
  -v, --version             show version information and quit
  -h, --help                show this message and quit
Server specific:
  -s, --server              run in server mode
  -D, --daemon              run the server as a daemon
  -1, --one-off             handle one client connection then exit
  --server-bitrate-limit #[KMG][/#]   server's total bit rate limit (default 0 = no limit)
                            (optional slash and number of secs interval for averaging
                            total data rate.  Default is 5 seconds)
  --idle-timeout #          restart idle server after # seconds in case it
                            got stuck (default - no timeout)
Client specific:
  -c, --client <host>[%<dev>] run in client mode, connecting to <host>
                              (option <dev> equivalent to `--bind-dev <dev>`)
  -u, --udp                 use UDP rather than TCP
  --connect-timeout #       timeout for control connection setup (ms)
  -b, --bitrate #[KMG][/#]  target bitrate in bits/sec (0 for unlimited)
                            (default 1 Mbit/sec for UDP, unlimited for TCP)
                            (optional slash and packet count for burst mode)
  --pacing-timer #[KMG]     set the timing for pacing, in microseconds (default 1000)
  -t, --time      #         time in seconds to transmit for (default 10 secs)
  -n, --bytes     #[KMG]    number of bytes to transmit (instead of -t)
  -k, --blockcount #[KMG]   number of blocks (packets) to transmit (instead of -t or -n)
  -l, --length    #[KMG]    length of buffer to read or write
                            (default 1 KB for TCP, dynamic or 1460 for UDP)
  --cport         <port>    bind to a specific client port (TCP and UDP, default: ephemeral port)
  -P, --parallel  #         number of parallel client streams to run
  -R, --reverse             run in reverse mode (server sends, client receives)
  --bidir                   run in bidirectional mode.
                            Client and server send and receive data.
  -w, --window    #[KMG]    set send/receive socket buffer sizes
                            (indirectly sets TCP window size)
  -M, --set-mss   #         set TCP/SCTP maximum segment size (MTU - 40 bytes)
  -N, --no-delay            set TCP/SCTP no delay, disabling Nagle's Algorithm
  -4, --version4            only use IPv4
  -6, --version6            only use IPv6
  -S, --tos N               set the IP type of service, 0-255.
                            The usual prefixes for octal and hex can be used,
                            i.e. 52, 064 and 0x34 all specify the same value.
  --dscp N or --dscp val    set the IP dscp value, either 0-63 or symbolic.
                            Numeric values can be specified in decimal,
                            octal and hex (see --tos above).
  -Z, --zerocopy            use a 'zero copy' method of sending data
  -O, --omit N              perform pre-test for N seconds and omit the pre-test statistics
  -T, --title str           prefix every output line with this string
  --extra-data str          data string to include in client and server JSON
  --get-server-output       get results from server
  --udp-counters-64bit      use 64-bit counters in UDP test packets
  --repeating-payload       use repeating pattern in payload, instead of
                            randomized payload (like in iperf2)

[KMG] indicates options that support a K/M/G suffix for kilo-, mega-, or giga-

iperf3 homepage at: https://software.es.net/iperf/
Report bugs to:     https://github.com/esnet/iperf
Usage: iperf3 [-s|-c host] [options]
Try `iperf3 --help' for more information.

W800: 
```