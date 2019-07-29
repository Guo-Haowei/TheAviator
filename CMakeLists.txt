cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

project(TheAviator)
SET(BASEPATH "${CMAKE_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES("${BASEPATH}")
set(GLFW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/libglfw3.dylib)
set(GLEW_LIB ${PROJECT_SOURCE_DIR}/dependencies/libs/libGLEW.a)
include_directories(${PROJECT_SOURCE_DIR}/dependencies/include)

find_package(OpenGL REQUIRED)

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE src/*.cc)
file(GLOB_RECURSE HEADER src/*.h)

add_executable(TheAviator ${HEADER} ${SOURCE})
target_compile_definitions(TheAviator PRIVATE DB_DEPTHMAP=0)
target_link_libraries(TheAviator ${OPENGL_LIBRARY} ${GLEW_LIB} ${GLFW_LIB})
