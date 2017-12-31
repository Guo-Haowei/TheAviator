cmake_minimum_required(VERSION 3.1)
project(TheAviator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

# glfw
message("Configuring glfw...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include_directories(external/glfw/include)
add_subdirectory(external/glfw)

# glad
message("Configuring glad...")
include_directories(external/glad/include)
add_subdirectory(external/glad)

# glm
message("Configuring glm...")
include_directories(external/glm/)
add_subdirectory(external/glm)

include_directories(${PROJECT_SOURCE_DIR}/src)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCE src/*.cc)
file(GLOB_RECURSE HEADER src/*.h)

add_executable(TheAviator ${HEADER} ${SOURCE})
target_link_libraries(TheAviator glad glfw)
