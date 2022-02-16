add_library(gfx
    GfxContext.cpp
    PipelineStateObjects.cpp
)

target_include_directories(gfx PRIVATE
    ${PROJECT_SOURCE_DIR}/source
)
