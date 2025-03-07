
.. _Build_system:

构建系统
=====================================

WM IoT SDK 基于组件( `component` )概念设计的构建系统，方便搭建层次结构清晰的项目架构。

用户无需了解太多 CMake 规则知识，只需基于模板修改变量值就可以非常简单地创建自己的项目。

使用时，工程目录和 WM IoT SDK 目录可以分开放置，使用灵活方便。

**目录结构：**

==========  =======================
目录/文件             功能          
==========  =======================
components  组件目录               
docs        文档目录               
examples    工程目录，或者例程目录  
tools       工具目录               
==========  =======================

组件（components）
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

所有库均作为组件( `component` )被放在 `components` 目录或工程目录下，每个组件用一个目录，这个目录就是这个组件的名字。

所有源文件都必须在某个组件内，为了使工程看起来更简洁，不对组件进行嵌套，所有组件都是一个层级。

每个工程可以包含一个或多个组件，但必须包含一个叫 `main` 的组件（即 `examples/project_template/main`  目录），每个组件包含的文件如下：

  - `CMakeLists.txt` ： 必须包含，用于声明组件源文件等，并且调用组件注册函数注册自己，
    详细可以参考 examples/project_template/main 下  `CMakeLists.txt`  的写法
  - `Kconfig` ： 可选，包含了本组件的配置选项， 
    在本组件或者其它依赖了本组件的组件的 CMakeLists.txt 中都可以再加一个 `CONFIG_` 前缀后使用这些配置项，
    比如在  `components/cli` 中， Kconfig 中有 `COMPONENT_CLI_ENABLED`  选项，我们在它的 `CMakeLists.txt` 中就使用了这个变量 `if(CONFIG_COMPONENT_CLI_ENABLED)` 来判断如果用户配置不用这个组件就不注册这个组件

.. hint::
   大多数组件默认都为不启用状态，在工程中如果使用到了某一组件，构建系统会自动设置对应的组件为启用状态。该机制能够有效缩短编译时间，但不能保证满足所有的场景，如果自动启用的组件与手动在 menuconfig 中的配置存在冲突时，可以在编译时使用 `wm.py build --disable-autoconf` 禁用该机制。

工程目录
^^^^^^^^^^^^^^^^


工程目录在 `examples` 目录下，当然，这个目录的名字是可以根据实际需求随意修改的，下面可以包含多个实际的工程目录，需要编译哪个工程时切换到对应的目录就可以编译。如上述，每个工程目录下必须有一个 `main` 组件， 当然也可以放很多自定义的组件。 可以参考 `examples/project_template` 工程目录。

工程目录下文件：

- `CMakeLists.txt`       ： 工程属性文件，必须包含。必须先包含 `include(${SDK_PATH}/tools/cmake/project.cmake)` ，然后用 `project` 函数声明工程名称，比如 `project(project_template)` ，当然还可以编写其它的条件或者变量等，使用 `CMake` 语法， 参考 `examples/project_template/CMakeLists.txt` 的写法 
- `prj.config` ： 工程默认配置文件，可选。执行  `cmake` 构建时会从这里加载默认配置。

.. attention::
    - 可以执行 `wm.py saveconfig` 将当前的工程配置直接保存为配置文件 `prj.config`，如不满足需要还可以继续手动对 `prj.config` 文件进行配置项目增删。
    - 每次修改 `prj.config` 后需要删除 `build` 目录(或者执行 `wm.py distclean`)重新编译一次以使新配置生效，因为当前构建系统会优先使用 `build` 目录下已经存在的配置文件。

快速创建工程
^^^^^^^^^^^^^^^^

通常情况下，只需要拷贝 `examples/project_template` 中的内容来新建一个工程。
然后按照自己的需求，修改 `project_template` 目录的名字，比如改成 `project` 。
当然，也可以复制其它现有的示例工程，比如 `examples/hello_world` 。

在新建工程的 `main` 文件夹中有一个 `CMakeLists.txt` 文件，可以在里面修改工程的源码设置，

其中 `ADD_INCLUDE` 列表变量定义了需要公开的头文件路径，以便其可以被其它组件所引用，设置时可按每行一个路径来书写，如：

::

  list(APPEND ADD_INCLUDE "include"
                          )

其中 `ADD_PRIVATE_INCLUDE` 列表变量定义了不需要公开的头文件路径，它只会被 `main` 下面的源码所引用，设置时可按每行一个路径来书写，如：

::

  list(APPEND ADD_PRIVATE_INCLUDE "myinc"
                                  )

其中 `ADD_SRCS` 列表变量定义了所需要参与编译的源码，如：

::

  list(APPEND ADD_SRCS "src/main.c"
                       "src/test.c"
                       )

当源码全部需要编译，不用逐一书写时，也可以批量按目录添加，如把 `src` 文件夹里的源码都加入：

::

  append_srcs_dir(ADD_SRCS "src")

当 `src` 文件夹中的源码全部加入后，也可能希望排除某个（或某几个）文件，如排除 `test2.c` 文件，则：

::

  list(REMOVE_ITEM ADD_SRCS "src/test2.c"
                            )

其中 `ADD_DEFINITIONS` 列表变量定义了编译时需要添加的选项，如增加宏定义 `AAABBB`、`AAACCC=1` 和 `BBBBDDDD="abc"` ：

::

  list(APPEND ADD_DEFINITIONS -DAAABBB
                              -DAAACCC=1
                              -DBBBBDDDD="abc"
                              )

其中 `ADD_GDEFINITIONS` 列表变量定义了编译时需要全局添加的选项（每个组件编译时都会被添加），
如增加宏定义 `AAABBB2`、`AAACCC2=1` 和 `BBBBDDDD2="abc"` ：

::

  list(APPEND ADD_GDEFINITIONS -DAAABBB2
                               -DAAACCC2=1
                               -DBBBBDDDD2="abc"
                               )

其中 `ADD_LINK_SEARCH_PATH` 列表变量定义了链接时所需要查找 lib 的路径，如：

::

  list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib"
                                   )

其中 `ADD_STATIC_LIB` 列表变量定义了使用的第三方lib文件（无源码），如使用了 `lib` 文件下的 `libtest.a` ：

::

  list(APPEND ADD_STATIC_LIB "lib/libtest.a"
                             )

其中 `CMAKE_C_LINK_FLAGS` 列表变量定义了链接参数，如使了 math 库时需要添加 `-lm` ：

::

  set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -lm" PARENT_SCOPE)

也可以在添加链接时使用的第三方库文件，如 `test/libtest.a` 和 `libtest2` ：

::

  set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -Wl,--whole-archive test/libtest.a -ltest2 -Wl,--no-whole-archive -Wl,--end-group" PARENT_SCOPE)

在设置上述的所有变量之后，调用 ``register_component`` 函数向编译系统注册组件，之后编译系统会自动导入这些文件和配置，在启动编译后使用。

::

  register_component()

注册的组件默认以其所在的文件夹名为组件名，编译时生成相应的 lib 文件。如组件文件夹为 `mycomp`，则组件名为 `mycomp`，编译后会生成 `libmycomp.a`。
构建系统支持生成指定的 lib 名，这可以避免某些同名造成的编译冲突问题。
如组件文件夹为 `mycomp`，则组件名为 `mycomp`，想在编译后生成 `libmycomp2.a`，则可以这样调用函数：

::

  register_component(mycomp2)


.. tip::
   - 以上除了源码 `ADD_SRCS` 变量必须要被设置，其它项如无都可以不用设置，不用时对应项留空或删掉即可。
   - 添加新组件时，也可参照上面的对 `main` 组件的描述来制作。大多数时候可直接参考 `components` 文件夹中的组件实现或 `examples/project_template` 来制作自己的组件。
   - 如果需要添加多条链接参数时（`CMAKE_C_LINK_FLAGS`），只需保留最后一条中的 `PARENT_SCOPE` 即可，前面的必须删除。
   - 一般而言，推荐在 `CMakeLists.txt` 文件的最末尾调用 `register_component` 函数。

工程代码入口
^^^^^^^^^^^^^^^^^

如上述，每个工程必须包含一个 `main组件` ，而 `main组件` 中则必须包含一个 `main函数` ，当系统启动后会自动执行 `main函数` 。

因此留给用户代码的入口为 `main函数` ，用户可在 `main函数` 中编写自己的代码：

::

  int main(void)
  {
      /* do some things */

      return 0;
  }


一般而言，只建议用户在 `main函数` 中添加初始化代码（如创建用户任务），具体业务代码在用户任务中去实现。

文件嵌入到代码中
^^^^^^^^^^^^^^^^^^^^^^^

用户无需手动将文件转为 C 语言数组放入代码文件中，使用 ``文件嵌入到代码中`` 功能后，可直接在代码中通过一个变量直接访问文件内容。

在工程文件夹或组件文件夹中，通过修改 `CMakeLists.txt` 文件来设置要嵌入的文件，提供两种不同嵌入方式。

.. _ADD_EMBEDDED_FILES:

**ADD_EMBEDDED_FILES：**
----------------------------------------
 
`ADD_EMBEDDED_FILES` 列表变量定义了需要添加的文件位置和名称，设置时可按每行一个路径来书写，本条列表变量是
将文件直接存储引用，以添加 `main` 组件下的 `src/png` 文件夹下的 `data.png` 为例，如:  

::

  list(APPEND ADD_EMBEDDED_FILES "src/png/data.png"
                                  )

在代码中使用文件需要使用符号名为 ``__binary_`` 加上组件名和 ``ADD_EMBEDDED_FILES`` 所填路径的展开，其中 ``.`` 
和  ``\``  会被  ``_``  代替，以使用上面添加 `main` 组件下的 `src/png` 文件夹下 `data.png` 为例，如：

::

  extern const uint8_t png_demos[]    asm("__binary_main_src_png_data_png");

其中 ``__binary_main_src_png_data_png`` 为符号名，它是上面例子中按照规则生成的符号名并且不可以更改， ``png_demos`` 
是添加的文件的别名，可以按照实际需要来改变名称。

如果需要获取文件的长度以上面添加的文件为例，如：

::

  extern const uint32_t png_demos_len    asm("__binary_main_src_png_data_png_length");

其中 ``__binary_main_src_png_data_png_length`` 是上面例子中的符号名后面加上 ``_length`` 的符号名并且
不可以改变， ``png_demos_len`` 是添加的文件长度的别名，可以按照实际需要改变名称。


**ADD_EMBEDDED_TEXTFILES：**
------------------------------------------

`ADD_EMBEDDED_TEXTFILES` 列表变量定义了需要添加的文本文件位置和名称，设置时可按每行一个路径来书写，这里添加的文件内容将被转成字符串使用（即末尾添加了 ``\0``），以添加使用 `main` 组件下的 `src/txt` 文件
夹下的 `data.txt` 为例，如:  

::

  list(APPEND ADD_EMBEDDED_FILES "src/txt/data.txt"
                                  )

使用文件的方法和  :ref:`ADD_EMBEDDED_FILES <ADD_EMBEDDED_FILES>` 一样。


.. _add_file_to_img:

在固件中添加自定义文件
^^^^^^^^^^^^^^^^^^^^^^^

可在固件中添加自定义文件随编译生成烧录镜像，烧录至指定的 Flash 位置。

在工程文件夹 `main` 文件夹中的 `CMakeLists.txt` 文件 `register_component()` 之前设置要添加自定义文件，然后修改分区表指定烧录位置即可。


**1. 在固件中添加自定义文件：**
------------------------------------------

`ADD_CUSTOM_FILES` 列表变量定义了需要添加到自定义文件到指定分区的信息，在

  下面是添加文件到 custom_pt1 分区和添加文件到 custom_pt2 的设置。

  ::

    list(APPEND ADD_CUSTOM_FILES "custom_pt1"
                     "src/bin/data1_demos.txt 0 1024"
                     "src/bin/data2_demos.bin 1024 -1"
                     )
    list(APPEND ADD_CUSTOM_FILES "custom_pt2"
                    "src/bin/data1_demos.txt 0 -1"
                    )

  - custom_pt1 分区

  添加 data1_demos.txt 文件放在分区偏移为 0 的位置，放该文件前 1024 字节。
  添加 data2_demos.bin 文件到分区偏移 1024 字节的地方，放该文件所有字节。

  - custom_pt2 分区

  添加 data1_demos.txt 文件放在分区偏移为 0 的位置，放该文件所有字节。

**2. 修改分区表：**
------------------------------------------

添加完文件后还需要在分区表中添加 custom_pt1 和 custom_pt2 分区。
以上面添加的自定义文件为例，如: 

.. list-table:: 
   :align: center

   * - # name 
     - offset 
     - size 
     - flag 
 
   * - custom_pt1 
     - 0xA0000 
     - 0x10000 
     - 0x0 
 
   * - custom_pt2  
     - 0xB0000 
     - 0x80000 
     - 0x0 


注意自定义分区表请参考 :ref:`分区表机制 <partition_table>`

.. attention:: 分区表的 Offset 和 size 需要 4K 对齐。请确保在配置分区表时，每个分区的 Offset 和 size 都是 4K 的整数倍。


.. _ADD_FATFS_FILES_TO_IMG:

在固件中添加 FATFS 文件系统镜像
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

可在固件中添加 FATFS 文件系统镜像随编译生成烧录镜像，烧录至指定的 Flash 分区（CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME，默认为名为 fatfs 的分区）。该功能允许在编译时将文件和文件夹打包成 FATFS 文件系统镜像，并烧录到 Flash 中。

在工程文件夹 `main` 文件夹中的 `CMakeLists.txt` 文件 `register_component()` 之前设置要添加到 FATFS 文件系统镜像的文件或文件夹，然后修改分区表指定烧录位置即可。


**1. 在固件中添加到 FATFS 文件系统镜像的文件或文件夹：**
----------------------------------------------------------------------------------

使用 `ADD_FATFS_FILES` 列表变量来指定需要添加到 FATFS 文件系统镜像的文件或文件夹。可以按每行一个路径来添加多个项目，示例如下：

  ::

    list(APPEND ADD_FATFS_FILES "../fatfs_folder/"
                                "../fatfs_file.txt"
                                    )

说明：

- `"../fatfs_folder/"` - 将指定文件夹下的所有内容添加到文件系统镜像中，但不包含 `fatfs_folder` 文件夹本身
- `"../fatfs_file.txt"` - 将单个文件添加到文件系统镜像的根目录

**2. 配置分区表：**
------------------------------------------

需要在分区表中添加用于存储 FATFS 文件系统的分区。默认分区名为 fatfs，也可通过 CONFIG_FATFS_INTERNAL_FLASH_PARTITION_NAME 配置项修改。

分区表配置示例：

.. list-table::
   :align: center

   * - # name
     - offset
     - size
     - flag

   * - fatfs
     - 0x131000
     - 0xAF000
     - 0x0

更多信息：

- 内部 Flash 中使用 FATFS 的完整示例，请参考 :ref:`examples/storage/fatfs/internal_flash_disk<storage_example>`
- 关于分区表的详细说明，请参考 :ref:`分区表机制 <partition_table>`

.. attention::

  - 分区表的 Offset 和 size 需要 4K 对齐。请确保在配置分区表时，每个分区的 Offset 和 size 都是 4K 的整数倍；
  - fatfs 分区大小务必 ≥ 96KB；
  - 生成的 FATFS 文件系统镜像大小等于 fatfs 分区大小，如果由于文件太大超过 fatfs 分区大小将会编译报错，导致构建失败；
  - 默认使用 8.3 格式文件名（8 个字符文件名 + 3 个字符扩展名），如需支持长文件名，可在 menuconfig 中配置 `CONFIG_FATFS_LFN_HEAP=y` 并通过 `CONFIG_FATFS_MAX_LFN` 设置最大长度（默认 128 字符）；
  - 生成的 fatfs 镜像会保存在 `build/fatfs_bin` 中，执行 `wm.py flash` 时会自动烧写该镜像，同时也会随 `app` 打包到工程的 `xxx.fls` 文件中，也可以通过 `wm.py flash -i` 命令行或 `Upgrade Tools 工具 <http://isme.fun/?log=blog&id=34>`_ 单独烧写 fatfs 镜像；
  - 对于外部 flash，也可以使用 `tools/wm/mkfs2img.py` 打包成 fatfs 二进制镜像文件，使用方法可通过 `python tools/wm/mkfs2img.py --help` 命令进行查阅；生成的镜像文件可由开发者自己开发的应用程序烧写到外部 flash 中，比如：应用自行创建一个 xmodem 通过 UART 从 PC 端获取镜像文件并烧写到外部 flash 的指定位置，或创建一个 http client 从网络下载并烧写到外部 flash 的指定位置。
