cmake_minimum_required(VERSION 3.15)

project(platinum)

include_directories(
        ThirdParty/Neptune/Source/Core
        ThirdParty/Neptune/Source/System/StdC
        ThirdParty/Neptune/Source/System/Bsd
        ThirdParty/Neptune/Source/System/Posix
        ThirdParty/Neptune/Source/System/Null

        Source/Core
        Source/Platinum
        Source/Extras
        Source/Devices/MediaConnect
        Source/Devices/MediaServer
        Source/Devices/MediaRenderer
)

file(GLOB SRCS_SYSTEM
        ThirdParty/Neptune/Source/Core/*.cpp
        ThirdParty/Neptune/Source/System/StdC/*.cpp
        ThirdParty/Neptune/Source/System/Bsd/*.cpp
        ThirdParty/Neptune/Source/System/Posix/*.cpp
        ThirdParty/Neptune/Source/System/Null/NptNullAutoreleasePool.cpp
        ThirdParty/Neptune/Source/System/Null/NptNullSerialPort.cpp

        Source/Core/*.cpp
        Source/Extras/*.cpp
        Source/Devices/MediaConnect/*.cpp
        Source/Devices/MediaServer/*.cpp
        Source/Devices/MediaRenderer/*.cpp
        )

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    link_libraries(pthread dl)
endif ()

add_library(${PROJECT_NAME} ${SRCS_SYSTEM})
link_libraries(${PROJECT_NAME})

file(GLOB SRCS Source/Apps/MicroMediaController/*.cpp)
add_executable(demo_MicroMediaController ${SRCS})
