if(CONFIG_COMPILER_OPTIMIZE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "Enable ccache will be used for faster recompilation")
    endif()
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif()
endif()

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

if(CONFIG_TOOLCHAIN_PATH OR CONFIG_TOOLCHAIN_PREFIX)
    if(CONFIG_TOOLCHAIN_PATH)
        if(WIN32)
            file(TO_CMAKE_PATH ${CONFIG_TOOLCHAIN_PATH} CONFIG_TOOLCHAIN_PATH)
        endif()
        if(NOT IS_DIRECTORY ${CONFIG_TOOLCHAIN_PATH})
            message(FATAL_ERROR "TOOLCHAIN_PATH set error:${CONFIG_TOOLCHAIN_PATH}")
        endif()
        set(path_split /)
    endif()
    set(TOOLCHAIN_PATH ${CONFIG_TOOLCHAIN_PATH})
    if(USE_CMAKE_BUILD_DEBUG)
        message(STATUS "TOOLCHAIN_PATH: ${CONFIG_TOOLCHAIN_PATH}")
        message(STATUS "TOOLCHAIN_PREFIX: ${CONFIG_TOOLCHAIN_PREFIX}")
    endif()
    set(CMAKE_C_COMPILER   "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}gcc${EXT}")
    set(CMAKE_CXX_COMPILER "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}g++${EXT}")
    set(CMAKE_ASM_COMPILER "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}gcc${EXT}")
    set(CMAKE_LINKER       "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}gcc${EXT}")
    set(CMAKE_AR           "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}ar${EXT}")
    set(CMAKE_RANLIB       "")
    set(CMAKE_OBJCOPY      "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}objcopy${EXT}")
    set(CMAKE_OBJDUMP      "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}objdump${EXT}")
    set(CMAKE_NM           "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}nm${EXT}")
    set(CMAKE_READELF      "${CONFIG_TOOLCHAIN_PATH}${path_split}${CONFIG_TOOLCHAIN_PREFIX}readelf${EXT}")
else()
    set(CMAKE_C_COMPILER   "gcc${EXT}")
    set(CMAKE_CXX_COMPILER "g++${EXT}")
    set(CMAKE_ASM_COMPILER "gcc${EXT}")
    set(CMAKE_LINKER       "ld${EXT}")
    set(CMAKE_AR           "ar${EXT}")
    set(CMAKE_RANLIB       "ranlib${EXT}")
    set(CMAKE_OBJCOPY      "objcopy${EXT}")
    set(CMAKE_OBJDUMP      "objdump${EXT}")
    set(CMAKE_NM           "nm${EXT}")
    set(CMAKE_READELF      "readelf${EXT}")
endif()

# skip compiler check
set(CMAKE_C_COMPILER_WORKS   TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# skip rdynamic check
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

#check gcc version
set(HIGH_VERSION_GCC 0)
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpfullversion -dumpversion OUTPUT_VARIABLE GCC_VERSION)
string(REGEX MATCHALL "[0-9]+" GCC_VERSION_LISTS ${GCC_VERSION})
list(GET GCC_VERSION_LISTS 0 GCC_MAJOR)
if(GCC_MAJOR GREATER_EQUAL 13)
    set(HIGH_VERSION_GCC 1)
endif()