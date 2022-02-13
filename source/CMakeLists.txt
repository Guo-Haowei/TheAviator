add_executable(TheAviator
    d3dApp.cpp
    d3dUtil.cpp
    MathHelper.cpp
    ExampleMain.cpp
)

target_link_libraries(TheAviator PRIVATE
    dxgi.lib
    d3d12.lib
    d3dcompiler.lib
)

target_compile_definitions(TheAviator PRIVATE
    -DUNICODE
)
