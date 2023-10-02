set(PROJECT ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES
        "*.h"
        "*.cpp"
        )

add_executable(${PROJECT} WIN32 ${SOURCES} "Kursach.rc")

include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT} capturer)