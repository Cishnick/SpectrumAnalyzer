cmake_minimum_required(VERSION 3.8)

project(SpectrumAnalyzer)

add_compile_options(
        #        /MP     # multi-processor compilation
        #        /W4		# warnings level 4
        #        /WX     # warning is error
        #        /we5038 # reorder
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory("src/capturer")
add_subdirectory("src/app")
add_subdirectory("src/interface")
