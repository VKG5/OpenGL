set(
    LIBRARY_SOURCES
)

set(
    LIBRARY_HEADERS
    "Mesh.h"
    "Shader.h"
    "Window.h"
    "Camera.h"
    "deps/imgui_impl_glfw.h"
    "GUI.h"
    "Texture.h"
    "Light.h"
    "Utilities.h"
    "Material.h"
    "DirectionalLight.h"
    "PointLight.h"
    "SpotLight.h"
    "Model.h"
    "ShadowMap.h"
    "OmniShadowMap.h"
    "Skybox.h"

    # "animation/filesystem.h"
    # "animation/shader_m.h"
    # "animation/camera.h"
    # "animation/animator.h"
    # "animation/model_animation.h"
    # "animation/animation.h"
    # "animation/bone.h"
    # "animation/assimp_glm_helpers.h"
    # "animation/mesh.h"
    # "animation/shader.h"
    # "animation/animdata.h"
)

set(
    LIBRARY_INCLUDES
    "./"
)

# STATIC/DYNAMIC LINKING OF LIBRARIES
add_library(
    ${LIBRARY_NAME} STATIC
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS}
)

# Including the current directory for libraries
target_include_directories(
    ${LIBRARY_NAME} PUBLIC
    ${LIBRARY_INCLUDES}
)
