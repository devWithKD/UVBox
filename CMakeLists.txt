# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.13)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

# Include build functions from Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(UVBox C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

aux_source_directory(. DIR_SRCS)

pico_sdk_init()

add_subdirectory(lib)
add_subdirectory(src)
