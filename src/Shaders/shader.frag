#version 460 core

in vec4 col;
in vec2 texCoord;

out vec4 color;

struct DirectionalLight {
    vec3 colour;
    float ambientIntensity;
};

uniform sampler2D theTexture;

// Making an object for the struct
uniform DirectionalLight directionalLight;

void main() {
    vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

    color = texture(theTexture, texCoord) * ambientColour;
}
