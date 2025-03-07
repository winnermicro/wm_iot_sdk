if(${CONFIG_COMPILER_OPTIMIZE_LEVEL_O0})
    set(compiler_optimization "-O0")
elseif(${CONFIG_COMPILER_OPTIMIZE_LEVEL_OS})
    set(compiler_optimization "-Os")
elseif(${CONFIG_COMPILER_OPTIMIZE_LEVEL_O2})
    set(compiler_optimization "-O2")
elseif(${CONFIG_COMPILER_OPTIMIZE_LEVEL_O3})
    set(compiler_optimization "-O3")
endif()

if(${CONFIG_COMPILER_DEBUG_INFORMATION})
    set(compiler_optimization "${compiler_optimization} -g3")
endif()

if(USE_CMAKE_PRE_PROCESS)
    set(compiler_optimization "${compiler_optimization} -E")
    if(USE_CMAKE_BUILD_PROMPT)
        set(USE_CMAKE_BUILD_PROMPT OFF)
    endif()
endif()

set(DEFINE_MACRO -DCONFIG_PROJECT_NAME="\\"${PROJECT_NAME}\\""
                 -DCONFIG_COMPILER_OPTS="\\"${compiler_optimization}\\""
                 )

if(${CONFIG_CHIP_W80X})
    include(${SDK_PATH}/tools/cmake/compile.w80x.cmake)
elseif(${CONFIG_CHIP_W9XX})
    include(${SDK_PATH}/tools/cmake/compile.w9xx.cmake)
endif()

#remove <FLAGS>
if(NOT USE_CMAKE_PRE_PROCESS)
    set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <CMAKE_C_LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <CMAKE_CXX_LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
else()
    set(CMAKE_C_LINK_EXECUTABLE "")
    set(CMAKE_CXX_LINK_EXECUTABLE "")
endif()

#overwrite ar
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> ${CMAKE_AR_FLAGS} <TARGET> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> ${CMAKE_AR_FLAGS} <TARGET> <OBJECTS>")

#don't need ranlib
set(CMAKE_C_COMPILER_RANLIB "")
set(CMAKE_CXX_COMPILER_RANLIB "")

set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_MODULE_LINKER_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "")
set(CMAKE_STATIC_LINKER_FLAGS "")

# Convert list to string
string(REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE ";" " " CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}")
string(REPLACE ";" " " LINK_FLAGS "${LINK_FLAGS}")
string(REPLACE ";" " " CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}")
string(REPLACE ";" " " CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}")
string(REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
string(REPLACE ";" " " CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
string(REPLACE ";" " " CMAKE_SHARED_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
string(REPLACE ";" " " CMAKE_STATIC_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
