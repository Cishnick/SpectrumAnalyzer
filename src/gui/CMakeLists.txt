set(PROJECT gui)

file(GLOB_RECURSE SOURCES
        "*.h"
        "*.cpp"
        )

add_library(${PROJECT} ${SOURCES} "../../resources/Kursach.rc")

include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT} capturer)