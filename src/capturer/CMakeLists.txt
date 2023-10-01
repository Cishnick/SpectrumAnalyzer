cmake_minimum_required(VERSION 3.8)

set(PROJECT capturer)

file(GLOB_RECURSE SOURCES
        "*.h"
        "*.cpp"
        )

add_library(${PROJECT} ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR})
