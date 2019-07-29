cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

project(TheAviator)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/dependencies/glm/)
include_directories(${PROJECT_SOURCE_DIR}/dependencies/include/)

if (APPLE)
    set(GLFW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/libglfw3.dylib)
    set(GLEW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/libGLEW.a)
elseif (WIN32)
    set(GLFW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/glfw3.lib)
    set(GLEW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/glew32s.lib)
endif ()

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCE src/*.cc)
file(GLOB_RECURSE HEADER src/*.h)

add_executable(TheAviator ${HEADER} ${SOURCE})
target_compile_definitions(TheAviator PRIVATE DB_DEPTHMAP=0)
target_link_libraries(TheAviator ${OPENGL_LIBRARY} ${GLEW_LIB} ${GLFW_LIB})
