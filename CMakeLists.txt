cmake_minimum_required(VERSION 3.0)
project(TheAviator)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}/")
add_subdirectory(source)
