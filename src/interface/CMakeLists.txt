set(TARGET interface)

file(GLOB_RECURSE SOURCES
        "*.h"
//        "*.cpp"
        )

add_library(${TARGET} INTERFACE ${SOURCES})

include_directories(${CMAKE_SOURCE_DIR})
