#version 460 core

layout (location = 0) in vec3 pos;

uniform mat4 model;

// Projection * View
uniform mat4 directionalLightTransform;

void main() {
    gl_Position = directionalLightTransform * model * vec4(pos, 1.0);
}