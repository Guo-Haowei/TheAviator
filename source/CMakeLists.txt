file(GLOB_RECURSE SOURCE ./*.cc)
file(GLOB_RECURSE HEADER ./*.h)

add_executable(TheAviator ${HEADER} ${SOURCE})

target_link_libraries(TheAviator glad glfw)
