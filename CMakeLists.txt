cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)

if(UNIX AND NOT APPLE)
set(LINUX TRUE)
endif()

project(TheAviator)
SET(BASEPATH "${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${BASEPATH}")

if(LINUX)
link_directories("${CMAKE_SOURCE_DIR}/libs")

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(OpenAL REQUIRED)

file(GLOB_RECURSE SOURCE src/*.cc src/*.cpp)
file(GLOB_RECURSE HEADER src/*.h src/*.hpp)

add_executable(TheAviator ${HEADER} ${SOURCE})
target_compile_definitions(TheAviator PRIVATE DB_DEPTHMAP=0)
target_link_libraries(TheAviator ${OPENAL_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} glfw3 pthread X11 Xi Xrandr Xcursor Xxf86vm Xinerama alut)

else()

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

find_package(OpenAL REQUIRED)

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE src/*.cc)
file(GLOB_RECURSE HEADER src/*.h)

add_executable(TheAviator ${HEADER} ${SOURCE})
target_compile_definitions(TheAviator PRIVATE DB_DEPTHMAP=0)
target_link_libraries(TheAviator ${OPENAL_LIBRARY} ${OPENGL_LIBRARY} ${GLEW_LIBRARY} ${GLFW_STATIC_LIBRARIES})

endif()
