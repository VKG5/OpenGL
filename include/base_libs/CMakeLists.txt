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
