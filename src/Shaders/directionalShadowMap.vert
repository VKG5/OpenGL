#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 model;

// Projection * View
uniform mat4 directionalLightSpaceTransform;

void main() {
    gl_Position = directionalLightSpaceTransform * model * vec4(pos, 1.0f);
}
