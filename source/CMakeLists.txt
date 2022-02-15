add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(gfx)

add_executable(TheAviator
    d3dUtil.cpp
    FrameResource.cpp
    main.cpp
)

target_link_libraries(TheAviator PRIVATE
    base
    core
    gfx
    dxgi.lib
    d3d12.lib
    d3dcompiler.lib
)

target_include_directories(TheAviator PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glm
)
