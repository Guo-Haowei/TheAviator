add_subdirectory(base)

add_executable(TheAviator
    d3dApp.cpp
    d3dUtil.cpp
    ExampleMain.cpp
)

target_link_libraries(TheAviator PRIVATE
    base
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
