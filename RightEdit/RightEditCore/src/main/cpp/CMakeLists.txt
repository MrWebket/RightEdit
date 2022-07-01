cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -DNDEBUG -Os")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -DNDEBUG -Os")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

set(LIB_NAME RightEdit)
message("load ${LIB_NAME}")

set(ENABLE_LOAD_FFMPEG_STATIC ON)

#添加配置文件 给变量初始化 该变量可以控制MKConfig.h中的变量
option(CONFIG_IS_ENABLE_LIBYUV "Enable CONFIG_IS_ENABLE_LIBYUV" ON)
option(CONFIG_FFMPEG_LOAD_STATIC "Enable CONFIG_FFMPEG_LOAD_STATIC" ${ENABLE_LOAD_FFMPEG_STATIC})

set(CONFIG_IS_ENABLE_LIBYUV ON)
set(CONFIG_FFMPEG_LOAD_STATIC ${ENABLE_LOAD_FFMPEG_STATIC})

# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/RightEdit/RightConfig.h"
        @ONLY
)

FILE(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/RightEdit/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/RightEdit/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/RightEdit/*.cc")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/RightEdit")

#用来控制IF ELSE语句的书写方式
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

set(LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBRARY_REF ${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI})
set(LIBRARY_OUT ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
message("LIBRARY_REF dir ${LIBRARY_REF}")
message("LIBRARY_OUT dir ${LIBRARY_OUT}")
message("LIBRARY_DIR dir ${LIBRARY_DIR}")

#include_directories("${include_directories}/")

file(GLOB RIGHTEDIT_OTHER_LIB "")

IF(${CONFIG_FFMPEG_LOAD_STATIC} STREQUAL "ON")
    message("load static ffmpeg")
    set(ffmpeg_lib
            ${LIBRARY_REF}/libavformat.a
            ${LIBRARY_REF}/libavcodec.a
            ${LIBRARY_REF}/libavfilter.a
            ${LIBRARY_REF}/libswresample.a
            ${LIBRARY_REF}/libswscale.a
            ${LIBRARY_REF}/libavutil.a
            ${LIBRARY_REF}/libfdk-aac.a
            ${LIBRARY_REF}/libx264.a
            )
    include_directories(${LIBRARY_DIR}/ffmpeg)
    list(APPEND RIGHTEDIT_OTHER_LIB ${ffmpeg_lib})
ELSE()
    #添加ffmpeg库
    MESSAGE("load SHARED ffmpeg")
    add_library(LIBFFMPEG SHARED IMPORTED)
    set_target_properties(LIBFFMPEG
            PROPERTIES IMPORTED_LOCATION
            ${LIBRARY_REF}/libffmpeg.so )
    include_directories(${LIBRARY_DIR}/ffmpeg)
    list(APPEND RIGHTEDIT_OTHER_LIB LIBFFMPEG)
ENDIF()


IF (${CONFIG_IS_ENABLE_LIBYUV} STREQUAL "ON")
    # 添加libyuv库
    MESSAGE("LOAD SHARED LIBYUV")
    add_library(LIBYUV SHARED IMPORTED)
    set_target_properties(LIBYUV
            PROPERTIES IMPORTED_LOCATION
            ${LIBRARY_REF}/libyuv.so )
    include_directories(${LIBRARY_DIR}/libyuv)
    list(APPEND RIGHTEDIT_OTHER_LIB LIBYUV)
ENDIF ()

find_library(
        log-lib
        log
)

add_library(
        ${LIB_NAME}
        SHARED
        ${SOURCES}
)

target_link_libraries(
        ${LIB_NAME}
        ${RIGHTEDIT_OTHER_LIB}

        EGL
        OpenSLES
        GLESv3
        android
        z
        jnigraphics
        ${log-lib}
)

IF(${CONFIG_FFMPEG_LOAD_STATIC} STREQUAL "OFF")
    message("copy libffmpeg to ${LIBRARY_OUT}")
    add_custom_command(TARGET ${LIB_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${LIBRARY_REF}/libffmpeg.so" "${LIBRARY_OUT}/libffmpeg.so"
            COMMENT "Copying libffmpeg to output directory")
ENDIF()
