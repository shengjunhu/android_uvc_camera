cmake_minimum_required(VERSION 3.10.2)
project(camera_uvc LANGUAGES CXX C)

# set config
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused -frtti -fexceptions")

# add debug log definitions
add_definitions(-DLOG_E)
if (${CMAKE_BUILD_TYPE} MATCHES "debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "Debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    add_definitions(-DLOG_W)
    add_definitions(-DLOG_I)
    add_definitions(-DLOG_D)
endif ()

# libjpeg-turbo, v3.0.2, https://github.com/libjpeg-turbo/libjpeg-turbo
include(ExternalProject)
ExternalProject_Add(libjpeg-turbo SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo)
set(LIB_JPEG turbojpeg-static)

# libyuv, v1763, https://chromium.googlesource.com/external/libyuv
set(LIB_YUV yuv)
add_subdirectory(libyuv)

# libusb, v1.0.27, https://github.com/libusb/libusb
set(LIB_USB usb)
add_subdirectory(libusb)

# libuvc, v0.0.7, https://github.com/libuvc/libuvc
set(LIB_UVC uvc)
add_subdirectory(libuvc)

# main module
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo
        ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb
        ${CMAKE_CURRENT_SOURCE_DIR}/libuvc/include)

# add library
add_library(${PROJECT_NAME} SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/source/camera_device.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/camera_device_jni.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_callback.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_converter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_decoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_encoder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_queue.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/source/frame_viewer.cpp)

# find log library
find_library(log-lib log)

# target link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${LIB_JPEG} ${LIB_YUV} ${LIB_UVC}
        ${log-lib} OpenMAXAL mediandk)