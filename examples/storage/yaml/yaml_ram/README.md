# libyaml

## 功能概述

本示例演示了如何使用 libyaml 解析 yaml 文件。

## 环境要求

无。

## 编译和烧录

示例位置：`examples/storage/yaml/yaml_ram`

编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/latest/get_started/index.html)

## 运行结果

成功运行将输出如下日志

```
yaml_ram_example: checking utf-8 sequences...
yaml_ram_example: 	a simple test:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	an empty line:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-0 is a control character:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #0 at 0)
yaml_ram_example: 
yaml_ram_example: 	u-80 is a control character:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	u-800 is valid:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-10000 is valid:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 5 bytes sequences are not allowed:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 
yaml_ram_example: 	6 bytes sequences are not allowed:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 
yaml_ram_example: 	u-7f is a control character:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #7F at 0)
yaml_ram_example: 
yaml_ram_example: 	u-7FF is valid:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-FFFF is a control character:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #FFFF at 0)
yaml_ram_example: 
yaml_ram_example: 	u-1FFFFF is too large:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #1FFFFF at 0)
yaml_ram_example: 
yaml_ram_example: 	u-3FFFFFF is 5 bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FB at 0)
yaml_ram_example: 
yaml_ram_example: 	u-7FFFFFFF is 6 bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FD at 0)
yaml_ram_example: 
yaml_ram_example: 	u-D7FF:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-E000:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-FFFD:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-10FFFF:
yaml_ram_example: 		+ +++
yaml_ram_example: (no error)
yaml_ram_example: 
yaml_ram_example: 	u-110000:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #110000 at 0)
yaml_ram_example: 
yaml_ram_example: 	first continuation byte:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	last continuation byte:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BF at 0)
yaml_ram_example: 
yaml_ram_example: 	2 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	3 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	4 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	5 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	6 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	7 continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 
yaml_ram_example: 	sequence of all 64 possible continuation bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #80 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #81 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #82 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #83 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #84 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #85 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #86 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #87 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #88 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #89 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8A at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8B at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8C at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8D at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8E at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #8F at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #90 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #91 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #92 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #93 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #94 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #95 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #96 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #97 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #98 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #99 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9A at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9B at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9C at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9D at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9E at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #9F at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A0 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A1 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A2 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A3 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A4 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A5 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A6 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A7 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #A9 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AA at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AB at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AC at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AD at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AE at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #AF at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B0 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B1 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B2 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B3 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B4 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B5 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B6 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B7 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #B9 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BA at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BB at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BC at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BD at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BE at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #BF at 0)
yaml_ram_example: 
yaml_ram_example: 	32 first bytes of 2-byte sequences {0xc0-0xdf}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid trailing UTF-8 octet: #20 at 1)
yaml_ram_example: 
yaml_ram_example: 	16 first bytes of 3-byte sequences {0xe0-0xef}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 
yaml_ram_example: 	8 first bytes of 4-byte sequences {0xf0-0xf7}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 
yaml_ram_example: 	4 first bytes of 5-byte sequences {0xf8-0xfb}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F9 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FA at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FB at 0)
yaml_ram_example: 
yaml_ram_example: 	2 first bytes of 6-byte sequences {0xfc-0xfd}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FD at 0)
yaml_ram_example: 
yaml_ram_example: 	sequences with last byte missing {u-0}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 
yaml_ram_example: 	sequences with last byte missing {u-...FF}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: incomplete UTF-8 octet sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FB at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FD at 0)
yaml_ram_example: 
yaml_ram_example: 	impossible bytes:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FE at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FF at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FE at 0)
yaml_ram_example: 
yaml_ram_example: 	overlong sequences {u-2f}:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 
yaml_ram_example: 	maximum overlong sequences:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 
yaml_ram_example: 	overlong representation of the NUL character:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid length of a UTF-8 sequence at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #F8 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid leading UTF-8 octet: #FC at 0)
yaml_ram_example: 
yaml_ram_example: 	single UTF-16 surrogates:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #D800 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB7F at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB80 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DBFF at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DC00 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DF80 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DFFF at 0)
yaml_ram_example: 
yaml_ram_example: 	paired UTF-16 surrogates:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #D800 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #D800 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB7F at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB7F at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB80 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DB80 at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DBFF at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: invalid Unicode character: #DBFF at 0)
yaml_ram_example: 
yaml_ram_example: 	other illegal code positions:
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #FFFE at 0)
yaml_ram_example: 		+ +++
yaml_ram_example: (reader error: control characters are not allowed: #FFFF at 0)
yaml_ram_example: 
yaml_ram_example: checking utf-8 sequences: 0 fail(s)
yaml_ram_example: checking boms...
yaml_ram_example: 	no bom (utf-8): 
yaml_ram_example: +
yaml_ram_example: 	bom (utf-8): 
yaml_ram_example: +
yaml_ram_example: 	bom (utf-16-le): 
yaml_ram_example: +
yaml_ram_example: 	bom (utf-16-be): 
yaml_ram_example: +
yaml_ram_example: checking boms: 0 fail(s)
yaml_ram_example: checking a long utf8 sequence...
yaml_ram_example: checking a long utf8 sequence: 0 fail(s)
yaml_ram_example: checking a long utf16 sequence...
yaml_ram_example: checking a long utf16 sequence: 0 fail(s)
yaml_ram_example: localhost, abc
```