add_library(core
    Geometry.cpp
    Node.cpp
    Scene.cpp
)

target_include_directories(core PRIVATE
    ${PROJECT_SOURCE_DIR}/external/glm
    ${PROJECT_SOURCE_DIR}/source
)