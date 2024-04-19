cmake_minimum_required(VERSION 3.10.2)
project(libuvc VERSION 0.0.7 LANGUAGES C)

if (${CMAKE_BUILD_TYPE} MATCHES "debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "Debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    option(ENABLE_UVC_DEBUGGING "Enable UVC debugging" ON)
else ()
    option(ENABLE_UVC_DEBUGGING "Disable UVC debugging" OFF)
endif ()

set(LIBUVC_HAS_JPEG FALSE)
configure_file(include/libuvc/libuvc_config.h.in
        include/libuvc/libuvc_config.h
        @ONLY)

# include
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../libusb/libusb)

# add library
add_library(${LIB_UVC} STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ctrl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ctrl-gen.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/diag.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/frame.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/init.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/stream.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.c)

# link libraries
if (${CMAKE_BUILD_TYPE} MATCHES "debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "Debug" OR
    ${CMAKE_BUILD_TYPE} MATCHES "DEBUG")
    find_library(log-lib log)
    target_compile_definitions(${LIB_UVC} PRIVATE UVC_DEBUGGING)
    target_link_libraries(${LIB_UVC} PRIVATE ${LIB_USB} ${log-lib})
else ()
    target_link_libraries(${LIB_UVC} PRIVATE ${LIB_USB})
endif ()