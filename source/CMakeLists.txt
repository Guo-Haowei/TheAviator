add_subdirectory(base)
add_subdirectory(core)

add_executable(TheAviator
    d3dApp.cpp
    d3dUtil.cpp
    ExampleMain.cpp
    FrameResource.cpp
)

target_link_libraries(TheAviator PRIVATE
    base
    core
    dxgi.lib
    d3d12.lib
    d3dcompiler.lib
)

target_compile_definitions(TheAviator PRIVATE
    -DUNICODE
)

target_include_directories(TheAviator PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glm
)
