#wmsdk options
option(USE_CMAKE_BUILD_DEBUG "show wmsdk cmake build verbose log" OFF)
option(USE_CMAKE_BUILD_PROMPT "show wmsdk cmake build prompt info" ON)
option(USE_CMAKE_PRE_PROCESS "preprocess c/c++ code without compiling" OFF)

# global variables
set(CMAKE_ECLIPSE_VERSION 3.6 CACHE INTERNAL "cmake eclipse version")
set(g_link_search_path "" CACHE INTERNAL "g_link_search_path")

# convert to cmake path(for Windows)
file(TO_CMAKE_PATH "${SDK_PATH}" SDK_PATH)

get_filename_component(parent_dir ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(current_dir ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
get_filename_component(parent_dir_name ${parent_dir} NAME)

# set project dir, so just projec can include this cmake file!!!
set(PROJECT_SOURCE_DIR ${parent_dir})
set(PROJECT_PATH       ${PROJECT_SOURCE_DIR})
set(PROJECT_BINARY_DIR "${parent_dir}/build")
set(wmsdk_config_dir "${PROJECT_BINARY_DIR}/config")
set(wmsdk_dt_dir "${PROJECT_BINARY_DIR}/device_table")

message(STATUS "SDK_PATH: ${SDK_PATH}")
message(STATUS "PROJECT_PATH: ${PROJECT_SOURCE_DIR}")

include("${SDK_PATH}/tools/cmake/function.cmake")

macro(project name)

    set(EXT "")

    include("${SDK_PATH}/tools/cmake/check.cmake")

    # include confiurations
    include("${SDK_PATH}/tools/cmake/menuconfig.cmake")
    include("${wmsdk_config_dir}/wmsdk_config.cmake")
    include("${SDK_PATH}/tools/cmake/kconfig.cmake")

    # config toolchain
    include("${SDK_PATH}/tools/cmake/toolchain.cmake")

    # declare project 
    # this function will clear flags!
    _project(${name} ASM C CXX)

    include("${SDK_PATH}/tools/cmake/compile.cmake")

    include("${SDK_PATH}/tools/cmake/component.cmake")

    # partition table confiurations
    include("${SDK_PATH}/tools/cmake/partition.table.cmake")
    include("${wmsdk_config_dir}/wmsdk_config.cmake")

    include("${SDK_PATH}/tools/cmake/device.table.cmake")

    # add lib search path to link flags
    foreach(abs_dir ${g_link_search_path})
        set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -L${abs_dir} -Wl,-rpath,${abs_dir}")
        set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -L${abs_dir} -Wl,-rpath,${abs_dir}")
    endforeach()

    # set build target
    set(project_elf ${name}.elf)
    set(project_bin ${name}.bin)
    set(project_img ${CMAKE_BINARY_DIR}/${name})
    set(project_mapfile "${name}.map")

    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/project)

    # create dummy source file project_elf_src.c to satisfy cmake's `add_executable` interface!
    set(project_elf_src ${CMAKE_BINARY_DIR}/project/project_elf_src.c)
    add_executable(${project_elf} "${project_elf_src}")
    add_custom_command(OUTPUT ${project_elf_src}
#                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/project
                       COMMAND ${CMAKE_COMMAND} -E touch ${project_elf_src}
                       VERBATIM)
    add_custom_target(gen_project_elf_src DEPENDS "${project_elf_src}")
    add_dependencies(${project_elf} gen_project_elf_src)

    include("${SDK_PATH}/tools/cmake/target.cmake")

    # add map file
    if(HIGH_VERSION_GCC)
        target_link_libraries(${project_elf} "-Wl,-Map,\"${project_mapfile}\"")
    else()
        target_link_libraries(${project_elf} "-Wl,-ckmap=\"${project_mapfile}\"")
    endif()

    # add ld file
    #execute_process(COMMAND ${CMAKE_C_COMPILER} -E -P -I ${CMAKE_BINARY_DIR}/config -I ${SDK_PATH}/components/wm_system/ld ${SDK_PATH}/components/wm_soc/${chip_family_type}/ld/chip_layout.ld.h -o ${CMAKE_BINARY_DIR}/project/chip_layout.ld)
    target_link_libraries(${project_elf} "-Wl,-T \"project/chip_layout.ld\"")


    # add link lib
    target_link_libraries(${project_elf} "-Wl,--start-group")
    target_link_libraries(${project_elf} "-Wl,--whole-archive")
    foreach(component_lib ${all_component_libs})
        if(IS_ABSOLUTE ${component_lib})
            file(RELATIVE_PATH rel_path ${CMAKE_BINARY_DIR} ${component_lib})
            target_link_libraries(${project_elf} ${PROJECT_BINARY_DIR}/${rel_path})
        else()
            target_link_libraries(${project_elf} ${component_lib})
        endif()
    endforeach()
    target_link_libraries(${project_elf} "-Wl,--no-whole-archive")
    target_link_libraries(${project_elf} "-Wl,--end-group")

endmacro()
