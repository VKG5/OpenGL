#version 460 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    float distance_ = length(fragPos.xyz - lightPos);

    // To normalize the distance
    distance_ /= farPlane;

    gl_FragDepth = distance_;
}
