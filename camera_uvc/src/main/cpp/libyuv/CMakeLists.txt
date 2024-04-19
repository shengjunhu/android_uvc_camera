cmake_minimum_required(VERSION 3.10.2)
project(libyuv VERSION 1763)

# set source
set(DIR_SRC source/compare.cc
            source/compare_common.cc
            source/convert.cc
            source/convert_argb.cc
            source/convert_from.cc
            source/convert_from_argb.cc
            source/convert_to_argb.cc
            source/convert_to_i420.cc
            source/cpu_id.cc
            source/planar_functions.cc
            source/rotate.cc
            source/rotate_any.cc
            source/rotate_argb.cc
            source/rotate_common.cc
            source/row_any.cc
            source/row_common.cc
            source/scale.cc
            source/scale_any.cc
            source/scale_argb.cc
            source/scale_common.cc
            source/video_common.cc)

option(LIBYUV_ENABLE_JPEG "option for libjpeg" OFF)
if(LIBYUV_ENABLE_JPEG)
  add_definitions(-DHAVE_JPEG)
  add_library(jpeg SHARED IMPORTED)
  set(DIR_JPEG
          source/convert_jpeg.cc
          source/mjpeg_decoder.cc
          source/mjpeg_validate.cc)
  list(APPEND DIR_SRC ${DIR_JPEG})
endif()

if(${ANDROID_ABI} MATCHES "armeabi-v7a")
  add_definitions(-DLIBYUV_NEON)
  set(DIR_ABI source/compare_neon.cc
              source/rotate_neon.cc
              source/scale_neon.cc
              source/row_neon.cc)
elseif(${ANDROID_ABI} MATCHES "arm64-v8a")
  add_definitions(-DLIBYUV_NEON)
  set(DIR_ABI source/compare_neon64.cc
              source/rotate_neon64.cc
              source/scale_neon64.cc
              source/row_neon64.cc)
else()
  set(DIR_ABI source/compare_gcc.cc
              source/rotate_gcc.cc
              source/scale_gcc.cc
              source/row_gcc.cc)
endif()

list(APPEND DIR_SRC ${DIR_ABI})
include_directories(include)
add_library(${MODULE_YUV} STATIC ${DIR_SRC})