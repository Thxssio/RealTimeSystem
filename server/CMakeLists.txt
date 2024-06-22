cmake_minimum_required(VERSION 3.10)
project(GPSReader)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(src)
include_directories(src/gps)
include_directories(src/utils)

add_executable(GPSReader
    src/main.c
    src/gps/gps.c
    src/utils/utils.c
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
