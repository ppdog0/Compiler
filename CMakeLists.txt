cmake_minimum_required(VERSION 3.19)

#FILE (GLOB ALL_SOURCES "*.cpp" "*.c" "./AFolder/*.cpp" "./AFolder/*.c" )
#FILE (GLOB ALL_INCLUDES "*.hpp" "*.h" "./AFolder/*.hpp"  "./AFolder/*.h" )

project(compiler)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include ALL_SOURCES)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src ALL_INCLUDES)

add_executable(main ${ALL_SOURCES} ${ALL_INCLUDES})