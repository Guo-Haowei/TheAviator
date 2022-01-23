cmake_minimum_required(VERSION 3.1)
project(TheAviator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

# glfw
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# glad
message("Configuring glad...")
add_subdirectory(external/glad)

# glm
message("Configuring glm...")
add_subdirectory(external/glm)

include_directories(external/glad/include)
include_directories(external/glfw/include)
include_directories(external/glm/)
include_directories(${PROJECT_SOURCE_DIR}/source)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(source)
