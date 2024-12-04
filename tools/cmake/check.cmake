#uname -s: Linux, Windows, Darwin
set(host_os_type ${CMAKE_HOST_SYSTEM_NAME})

# check python env
get_python(python python_version python_path)
if(NOT python)
    message(FATAL_ERROR "python not found, please install python firstly(python3 recommend)!")
endif()
if(USE_CMAKE_BUILD_DEBUG)
    message(STATUS "Found ${python_version}, path: ${python_path}")
endif()
