cmake_minimum_required(VERSION 3.20)

# Get SDK path
if(NOT SDK_PATH)
    get_filename_component(SDK_PATH ../../ ABSOLUTE)
    if(EXISTS $ENV{WM_IOT_SDK_PATH})
        set(SDK_PATH $ENV{WM_IOT_SDK_PATH})
    endif()
endif()

# Check SDK Path
if(NOT EXISTS ${SDK_PATH})
    message(FATAL_ERROR "SDK path Error, Please set WM_IOT_SDK_PATH variable")
endif()

# Call compile rules
include(${SDK_PATH}/tools/cmake/project.cmake)

# Project Name, default the same as project directory name
get_filename_component(parent_dir ${CMAKE_PARENT_LIST_FILE} DIRECTORY)
get_filename_component(project_dir_name ${parent_dir} NAME)

set(PROJECT_NAME ${project_dir_name}) # change this var if don't want the same as directory's

message(STATUS "PROJECT_NAME: ${PROJECT_NAME}")
project(${PROJECT_NAME})
