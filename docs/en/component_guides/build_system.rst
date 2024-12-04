
.. _Build_system:

WM-IOS Building System Programming Manual
===========================================

WM-IOS is a building system designed based on the concept of  `component` , which facilitates the construction of project architectures with clear hierarchical structures.

Users do not need to have a lot of knowledge about CMake rules, they can easily create their own projects by modifying variable values based on templates.

When using it, the project directory and the WM IoT SDK directory can be placed separately, providing flexibility and convenience.

**Directory Structure:**

==============  =======================
Directory/File       Function         
==============  =======================
components        Component Directory              
docs            Documentation Directory             
examples          Project or Directory
tools             Tools Directory              
==============  =======================

Components
^^^^^^^^^^^^^^^

All libraries are organized as components in the `component` directory or within the project directory. Each component is placed in its own directory, and the name of this directory represents the component name.

All source files must belong to a component. To keep the project structure clean and straightforward, components are not nested; they all exist at the same level.

Each project can contain one or more components but must contain a component named `main` (i.e., the `examples/project_template/main` directory). Each component includes the following files:

  - `CMakeLists.txt`: Required. It declares the component's source files and registers the component using a component registration function. For detailed guidance, refer to the `CMakeLists.txt` file in `examples/project_template/main` .
  - `Kconfig` : Optional. It includes configuration options for the component.
    These configuration options can be used with a  `CONFIG_` prefix in the `CMakeLists.txt` files of the component itself or of any other components that depend on it. 
    For example, in `components/cli` the Kconfig file may have a `COMPONENT_CLI_ENABLED` option, which is then used in its `CMakeLists.txt`  file with the condition  `if(CONFIG_COMPONENT_CLI_ENABLED)` to determine whether to register the component if the user has configured it to be disabled.

.. hint::
   Most components are disabled by default. If a component is used in a project, the build system automatically sets the corresponding component to enabled. 
   This mechanism helps reduce compilation time but may not cater to all scenarios.
   If there is a conflict between automatically enabled components and manual configurations in `menuconfig`, you can disable this mechanism during compilation by using `wm.py build --disable-autoconf`.
   

Project Directory
^^^^^^^^^^^^^^^^^^^^^^


The project directory is located under the `examples` directory. Of course, the name of this directory can be modified according to actual needs. It can contain multiple actual project directories. When you need to compile a project, simply switch to the corresponding directory to compile. As mentioned earlier, each project directory must have a `main` component, and it can also include many custom components. You can refer to the `examples/project_template` project directory for guidance.

Files in the project directory:

- `CMakeLists.txt`: This is a mandatory project property file. It must include  `include(${SDK_PATH}/tools/cmake/project.cmake)` first, and then use the `project` function to declare the project name, such as `project(project_template)` . You can also write other conditions or variables using `CMake` syntax. Refer to  `examples/project_template/CMakeLists.txt` for an example. 
- `prj.config` : This is an optional default configuration file for the project. When executing `cmake` to build, it will load the default configuration from here.

.. attention::
    - You can execute `wm.py saveconfig` to save the current project configuration directly as the configuration file `prj.config` file. If necessary, you can continue to manually configure add or remove configuration items in the  `prj.config` file.
    - After modifying `prj.config` , you need to delete the `build` directory (or execute `wm.py distclean`)and recompile to apply the new configuration, as the current build system  will prioritize configuration files already present in the ` build` directory. 

Quick Project Creation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Typically,  you can create a new project by copying the contents of `examples/project_template`.
Then, rename the `project_template` directory according to your needs, such as changing it to `project`.
Alternatively, you can copy an existing example project, like`examples/hello_world`.

In the `main` folder of the new project, there is a `CMakeLists.txt` file where you can modify the project's source code settings.

The `ADD_INCLUDE` list variable defines the header file paths that need to be exposed so they can be referenced by other components. When setting this, you can write one path per line, like this:

::

  list(APPEND ADD_INCLUDE "include"
                          )

The `ADD_PRIVATE_INCLUDE` list variable defines the header file paths that do not need to be exposed. They will only be referenced by the source code under `main`. When setting this, you can write one path per line, like this:

::

  list(APPEND ADD_PRIVATE_INCLUDE "myinc"
                                  )

The `ADD_SRCS` list variable defines the source code files that need to be compiled.For example:

::

  list(APPEND ADD_SRCS "src/main.c"
                       "src/test.c"
                       )

When all the source code needs to be compiled and without writing each one separately, you can add them by directory added in batches by directory, such as adding all the source code in the `src` folder:

::

  append_srcs_dir(ADD_SRCS "src")

After adding all source code in the `src` folder,if you want to exclude a specific files, you can do so by:

::

  list(REMOVE_ITEM ADD_SRCS "src/test2.c"
                            )

The `ADD_DEFINITIONS` list variable defines the options that need to be added during compilation, such as adding macro definitions `AAABBB=1` and `BBBBDDDD=2`:

::

  list(APPEND ADD_DEFINITIONS -DAAABBB=1
                              -DBBBBDDDD=2
                              )

The `ADD_LINK_SEARCH_PATH`  list variable defines the paths to search for libraries during linking, such as:

::

  list(APPEND ADD_LINK_SEARCH_PATH "${CONFIG_TOOLCHAIN_PATH}/lib"
                                   )

The `ADD_STATIC_LIB` list variable defines the third-party library files (without source code) that will be used, for example, using `libtest.a` in the `lib`  folder:

::

  list(APPEND ADD_STATIC_LIB "lib/libtest.a"
                             )

The `CMAKE_C_LINK_FLAGS` list variable defines the linking parameters. For example, if you use the math library, you need to add`-lm`:

::

  set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -lm" PARENT_SCOPE)

You can also add third-party library files for linking here, such as `test/libtest.a` and `libtest2`:

::

  set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -Wl,--whole-archive test/libtest.a -ltest2 -Wl,--no-whole-archive -Wl,--end-group" PARENT_SCOPE)

After setting all the above variables, call the  ``register_component`` function to register the component with the build system. The build system will automatically import these files and configurations for use during compilation.

::

  register_component()

The registered component defaults to using the folder name as the component name, and a corresponding lib file is generated after compilation. For example, if the component folder is named `mycomp`,the component name is `mycomp`,and after compilation, `libmycomp.a`is generated. 
The build system supports generating specified lib names to avoid compilation conflicts caused by identical names. 
For example, if you want to generate `libmycomp2.a` after compiling the `mycomp` component folder, you can call the function like this:

::

  register_component(mycomp2)


.. tip::
   - Besides the mandatory `ADD_SRCS` variable , other items can be left empty or deleted if not needed.
   - When adding new components,  follow the description of the `main` component above. Most of the time, you can directly refer to the components implementations in the  `components` folder or `examples/project_template` to create your own components.
   - When adding multiple linking parameters(`CMAKE_C_LINK_FLAGS`), only keep `PARENT_SCOPE` in the last one, and delete the previous ones.
   - Generally, it is recommended to call the `register_component` function at the end of the `CMakeLists.txt` file. 

Project Code Entry
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned above, each project must include a `main component` , and the `main component` must contain a  `main function`. When the system starts, it will automatically execute the  `main function` .

Therefore, the entry point for the user's code is the `main function` , where users can insert their own code:

::

  int main(void)
  {
      /* do some things */

      return 0;
  }


Generally, it is recommended that users only add initialization code (such as creating user tasks) in the `main function` and implement specific business logic in user tasks.

Embedding Files into Code
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Users do not need to manually convert files to C language arrays and insert them into code files. By using the ``embedding files into code`` feature, files can be directly accessed through a variable in the code.

In the project folder or component folder, modify the `CMakeLists.txt` file to set the files to be embedded. Two different embedding methods are provided.


.. _ADD_EMBEDDED_FILES:

**ADD_EMBEDDED_FILES：**
----------------------------------------
 
The `ADD_EMBEDDED_FILES` list variable defines the paths and names of the files to be added. Each path should be written on a new line. This variable directly stores file references. For example, to add `data.png` in the `src/png` folder under the `main` component, you can write:

::

  list(APPEND ADD_EMBEDDED_FILES "src/png/data.png"
                                  )

To use the file in the code, you need to use a symbol name ``__binary_``, followed by the component name and the expanded path filled in ``ADD_EMBEDDED_FILES``, where ``.`` and ``\`` are replaced with ``_``. For example, using the above-added `data.png` in the `src/png` folder under the `main` component, you can write:

::

  extern const uint8_t png_demos[]    asm("__binary_main_src_png_data_png");

``__binary_main_src_png_data_png`` is the symbol name generated according to the rules above and cannot be changed. ``png_demos`` is the alias for the added file and can be changed as needs.

To get the length of the file using the above-added file as an example:

::

  extern const uint32_t png_demos_len    asm("__binary_main_src_png_data_png_length");

``__binary_main_src_png_data_png_length`` is the symbol name with  ``_length`` appended and cannot be changed. ``png_demos_len`` is an alias for the file length and can be changed as needed. 


**ADD_EMBEDDED_TEXTFILES：**
------------------------------------------
  
The `ADD_EMBEDDED_TEXTFILES` list variable defines the paths and names of the text files to be added. Each path should be written on a new line. The content of the files added here will be converted to strings (i.e., a ``\0`` will be appended at the end). For example, to add `data.txt` from the `src/txt` folder under the `main` component, you can write:

::

  list(APPEND ADD_EMBEDDED_FILES "src/txt/data.txt"
                                  )

The method of using the files is the same as for :ref:`ADD_EMBEDDED_FILES <ADD_EMBEDDED_FILES>`.


Adding Custom Files to Firmware
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can add custom files to the firmware to be included in the compiled and burned image, which will then be burned to a specified Flash location.

To add custom files to your firmware, configure the settings in the `CMakeLists.txt` file located in the main folder of your project before the `register_component()` function. Then, modify the partition table to specify the burning location.

**1. Adding Custom Files to Firmware:**
------------------------------------------

The `ADD_CUSTOM_FILES` list variable defines information about custom files to be added to specified partitions.

Below is an example of adding files to custom_pt1 and custom_pt2 partitions:

::

  list(APPEND ADD_CUSTOM_FILES "custom_pt1"
                   "src/bin/data1_demos.txt 0 1024"
                   "src/bin/data2_demos.bin 1024 -1"
                   )
  list(APPEND ADD_CUSTOM_FILES "custom_pt2"
                  "src/bin/data1_demos.txt 0 -1"
                  )

- custom_pt1 partition

Add data1_demos.txt file at partition offset 0, storing the first 1024 bytes.
Add data2_demos.bin file at partition offset 1024 bytes, storing all bytes.

- custom_pt2 partition

Add data1_demos.txt file at partition offset 0, storing all bytes.

**2. Modifying the Partition Table:**
------------------------------------------

After adding the files, you need to add the custom_pt1 and custom_pt2 partitions to the partition table
Using the custom files added above as an example:

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


For details on custom partition tables, please refer to :ref:`Partition Table Mechanism <partition_table>`

.. attention:: The Offset and size in the partition table need to be 4K-aligned. Ensure that when configuring the partition table, the Offset and size of each partition are multiples of 4K.