# TFT_LCD_LVGL_FS

## 功能概述
LVGL 支持将文件(图片,音乐等...)存储在文件系统, 支持内存分配方式的客制化指定，支持软件解码器对部分格式的原始图片进行解码(如:.PNG)。
此应用程序是使用 `WM IoT SDK` 进行 `LVGL` + `pSRAM` + `FATFS` + `PNG Decoder` 的组合能力演示。
同时应用程序也演示了 `客制化分区表`, `自定义分区` 的功能。

此应用程序主要用于组合功能的展示，部分功能会影响画面演示效能。在实际工程中需要做适当取舍。
有关于 `PNG Decoder` + `FATFS` 主要适用于此实例的 LOGO 显示， 对于 LOGO 显示之后的 其他LVGL Demo原生场景并未启用这两个功能做展示。

## 环境要求

1. TFT LCD 屏：

   - 生产厂家： New Vision Microelectronics Inc。
   - 型号：NV3041A
   
2. TFT LCD 屏引脚连接：

   硬件配置存储在设备表中，包含 SDIO 设备的引脚定义 和 SDIO(SPI Mode) 时钟设定 以及 TFT LCD 屏设备的其他引脚定义。
   SDIO 设备 及 TFT LCD 屏设备 可在设备表中 通过设备名 进行定义查询。
   设备表的定义文件是 `wm_dt_hw.c`，但具体可能会因为此工程所引用的设备表文件不同而由差异。

   下图为 W801S 的 GPIO 序号  与 NV3041A PIN 的对应关系 示例， 实际应该参考电路图及对应的设备表 配置。

   |    GPIO序号    | 引脚编号 | 连接描述  | NV3041A PIN |
   | -------------- |-------- | -------- | ----------- |
   | WM_GPIO_NUM_22 |   PB6   |   CLK    | 11          |
   | WM_GPIO_NUM_23 |   PB7   |   MOSI   | 13          |
   | WM_GPIO_NUM_27 |   PB11  |   CS     | 9           |
   | WM_GPIO_NUM_26 |   PB10  |   RESET  | 15          |
   | WM_GPIO_NUM_32 |   PB16  |   LED    | 36          |
   | WM_GPIO_NUM_25 |   PB9   |   DCX    | 10          |
   | WM_GPIO_NUM_24 |   PB8   |   TE     | 8(optional) |

3. pSRAM 要求

   最大支持 8 MB 外挂 pSRAM。
   下图为 W801S 的 GPIO 序号  与 pSRAM PIN 的对应关系 示例， 实际应该参考电路图及对应的设备表 配置。

   |    GPIO序号    | 引脚编号 | 连接描述  | pSRAM PIN |
   | -------------- |-------- | -------- | --------- |
   | WM_GPIO_NUM_15 |   PA15  |   SCLK   | 11        |
   | WM_GPIO_NUM_43 |   PB27  |   CS     | 7         |
   | WM_GPIO_NUM_18 |   PB2   |   SIO0   | 9         |
   | WM_GPIO_NUM_19 |   PB3   |   SIO1   | 3         |
   | WM_GPIO_NUM_20 |   PB4   |   SIO2   | 5         |
   | WM_GPIO_NUM_21 |   PB5   |   SIO3   | 1         |

4. Flash 要求

   默认使用了内置 Flash, 并采用独立的 Flash 分区表文件 "partition_table_custom.csv"。
   在此分区表中包含了 `348KB` 的 `FATFS` 分区，用于存放包含原始图片的 FATFS 镜像。


## 编译和烧录

示例位置：`examples\tft_lcd\tft_lcd_lvgl_fs`

编译选项可以在 `menuconfig` 或 工程实例目录的 `prj.config` 中进行配置。

**1： SOC 和 Clock 的配置：**
```
CONFIG_BUILD_TYPE_XXX=y
```
说明： 
XXX 表示 如W800, W802, W801S, W803, W805, W806 等 WM_IOT_SDK 所支持的芯片型号。
所选择的 `CONFIG_BUILD_TYPE_XXX` 以及 所引用的 设备表 需要与程序所运行的开发板一致。
效能上，默认 RCC CPU Clock为 80MHz， 为了更高的画面显示效率，用户可在设备表 `wm_dt_hw.c` 中将 RCC CPU Clock 设定成较高的值，比如：240MHz。

**2： TFT-LCD 屏的配置：**
```
CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y
```
说明：
应用程序 使用的 TFT LCD 屏可以被替换,但需要满足几个条件：
- 所选择 TFT LCD 屏的设备信息 应该在设备表中存在。
- `wm_drv_tft_lcd_cfg.h` 中的宏定义 `WM_LVGL_LCD_MODULE_NAME`， 其值应与设备表中所选择 TFT LCD 屏的设备名一致。
- `menuconfig -> PERIPHERALS -> TFT LCD -> LCD Device` 中须要勾选上 期望选中的 LCD 设备, 或者在 `prj.config` 中直接定义所需 LCD 设备的编译开关，如 `CONFIG_COMPONENT_DRIVER_LCD_NV3041A_SPI=y` 。
- 被选中的 TFT LCD 屏的设备驱动有被完整实现，主要是其驱动的 `ops` 函数列表实现，并将对应的 `ops` 注册到设备表的 `dt_hw_table_entry[]` 。

**3： Demo 场景：**
```
CONFIG_LV_USE_DEMO_MUSIC=y
```
说明：
默认打开的是 `CONFIG_LV_USE_DEMO_MUSIC=y`, 可以在 `menuconfig -> LVGL -> Demos` 中修改，或者在 `prj.config` 中直接定义所需选中 Demo 场景的编译项开关，比如：打开 `CONFIG_LV_USE_DEMO_STRESS=y`。
需要注意的是，为了避免运行冲突, 不能同时打开两个及以上的 LVGL原生Demo 场景的宏定义开关。

**4： LVGL Memory客制化配置：**
```
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"
```
说明：
配置开启后，LVGL 的 lv_mem_alloc， lv_mem_realloc,  lv_mem_free API 会由 wm_lv_port_mem.h 中定义的 pSRAM 的内存管理接口来实现。
硬件上，也须确保 pSRAM 元件有存在于程序所运行的开发板。
效能上，pSRAM 访问效率弱于 sRAM，打开此功能后，LVGL 的画面显示速度会降低。
务必确保此定义打开后，需要开发板上有pSRAM 元件，否则运行时会发生 wm heap 相关的 `Exception`。

**5： FATFS + PNG Decoder + 客制化分区表 的配置：**
```
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"

CONFIG_LV_USE_PNG=y
CONFIG_LV_USE_FS_FATFS=y
CONFIG_LV_FS_FATFS_LETTER=68
CONFIG_LV_FS_FATFS_CACHE_SIZE=4096

CONFIG_COMPONENT_FATFS_ENABLED=y
CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE=y
CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE=y
CONFIG_FATFS_LFN_HEAP=y
CONFIG_FATFS_MAX_LFN=255

CONFIG_WM_PARTITION_TABLE_CUSTOMIZATION=y
```

说明：
此示例包含一个 fat16 镜像文件， 镜像文件中有一张 Winner Mico LOGO 的 PNG 图片。
LVGL 组件通过 lv_img_set_src(img, src) 来加载图片，需要注意的是： 
- 若 src 为有效的文件路径，LVGL 组件会通过 f_open, f_read 等接口，获取文件系统中的文件。
  - 若文件为PNG格式的图片，LVGL 组件会找对应的图片解码器解码后显示到屏幕。
  - 若文件为 图片 RGB565 格式的二进制数组，LVGL 组件会 跳过 文件系统接口 和 PNG Decoder 操作，直接做后续处理并显示到屏幕。
  - 有关 src 的有效文件路径，特别要注意的是盘符的设定，即 src 所指向路径 "D:assert/pic.png"， 其中 "D" 必须要与如下配置一致
    - `CONFIG_LV_FS_FATFS_LETTER 68` 这是 LVGL 组件的内部配置，68 为字符 D 的 ASCILL 码。
    - `CONFIG_FATFS_INTERNAL_FLASH_DISK_FF_VOLUME_STRS “D”` 这是 FATFS 组件 的配置，在打开  `CONFIG_FATFS_FF_STR_VOLUME_ID_ENABLE`，`CONFIG_FATFS_INTERNAL_FLASH_DISK_ENABLE` 的选项时，即文件系统所用的盘是选择为内置Flash，则默认会用此盘符 "D"。
- 若 src 为 图片的 RGB565 格式的二进制数组，LVGL 组件会 跳过 文件系统接口 和 PNG Decoder 操作，直接做后续处理并显示到屏幕。
效能上，PNG Decoder 由 CPU 运算，画图效率低，内存开销大。实际工程中，建议文件系统中存储的不是 PNG格式原图，而是图片所转换成RGB565格式的bin文件，如果这样，如下配置就无须配置。
```
CONFIG_LV_USE_PNG=y
CONFIG_COMPONENT_DRIVER_PSRAM_ENABLED=y
CONFIG_LV_MEM_CUSTOM=y
CONFIG_LV_MEM_CUSTOM_INCLUDE="wm_lv_port_mem.h"
CONFIG_LV_USE_PNG=y
```

对于包含图片的 FAT 格式的镜像文件，可以通过一些工具产生，比如 winhex 等。
需要注意的是，产生的镜像文件大小 <= `fatfs` 分区大小(定义在Flash 分区表文件）。

**6： CLI 的配置：**
```
CONFIG_COMPONENT_CLI_ENABLED=y
CONFIG_CLI_COMMANDS_SYSTEM=y
```

说明：
CLI 为 debug 用途，通过 CLI 查询工程运行时状态，在用户工程中的量产版本 可以考虑关闭。


编译、烧录等操作请参考：[快速入门](https://doc.winnermicro.net/w800/zh_CN/2.2-beta.2/get_started/index.html)


## 运行结果

成功运行将输出如下日志
```
W801S: [I] (675) lvgl_example: LCD x_resolution = 480
[I] (675) lvgl_example: LCD y_resolution = 272
[I] (675) lvgl_example: LCD rotation = 0

当文件系统中有找到对应的LOGO文件时，会直接从Filesystem中取原始图片，并用PNG Decoder实时解码后显示
[I] (702) app_fs: found file in the file system, size: 33074 bytes
[I] (703) lvgl_example: show logo from filesystem
```

```
W801S: [I] (675) lvgl_example: LCD x_resolution = 480
[I] (675) lvgl_example: LCD y_resolution = 272
[I] (675) lvgl_example: LCD rotation = 0

当文件系统中没有找到对应的LOGO文件时，比如这里测试将盘符改成了 "E"。 
此时会直接用预先存储到 Text区段的 RGB格式的数组中取转换成RGB格式的图片的数组，不会经过PNG Decoder ，而直接显示到屏幕。
[E] (701) app_fs: failed(3) to mount fs: E:assets/winner_micro_logo_120x120.png
[I] (701) lvgl_example: show logo from array
```
运行实际效果 会显示在 TFT-LCD 屏上。



如下是上电后，通过 Uart0（默认 CLI 端口，可通过 menuconfig 调整), 输入 help 命令 呈现出的 CLI 命令。
```
W801S: help
COMMAND LIST:
cls                   -- clear command line
help                  -- show help info
log                   -- log level cmd
heap                  -- show heap tracing
free                  -- show free heap size
task                  -- show task list
reboot                -- reboot system
version               -- show sdk version info
partition             -- partition table cmd
```