cmake_minimum_required(VERSION 3.10.2)
project(libusb VERSION 1.0.27 LANGUAGES C)

# config flag
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -pthread")

# config log
if (${CMAKE_BUILD_TYPE} MATCHES "debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "Debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    message(STATUS "USB DEBUGGING Enable!")
    add_definitions(-DENABLE_DEBUG_LOGGING)
endif()

# add include
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/android
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os)

# add library
add_library(${LIB_USB} STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/core.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/descriptor.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/hotplug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/io.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/sync.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/strerror.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/linux_usbfs.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/events_posix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/threads_posix.c
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/os/linux_netlink.c)

# link libraries
if (${CMAKE_BUILD_TYPE} MATCHES "debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "Debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    find_library(log-lib log)
    target_link_libraries(${LIB_USB} PRIVATE ${log-lib})
endif ()