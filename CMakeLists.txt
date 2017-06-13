cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
set(PROJECT_NAME RfCom)
project(${PROJECT_NAME})

set(${CMAKE_PROJECT_NAME}_BOARD micro)
set(${CMAKE_PROJECT_NAME}_PORT /dev/ttyUSB0)

set(${CMAKE_PROJECT_NAME}_SKETCH RfCom.ino)

set(ALL_SRCS configstore.cpp dio.cpp somfy.cpp)

generate_arduino_firmware(${CMAKE_PROJECT_NAME}
        SRCS configstore.cpp dio.cpp somfy.cpp
        HDRS configstore.h dio.h somfy.h
        )
