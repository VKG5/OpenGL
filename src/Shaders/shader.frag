#version 460 core

in vec4 col;
in vec2 texCoord;
in vec3 Normal;

out vec4 color;

struct DirectionalLight {
    vec3 colour;
    float ambientIntensity;
    vec3 direction;
    float diffuseIntensity;
};

uniform sampler2D theTexture;

// Making an object for the struct
uniform DirectionalLight directionalLight;

void main() {
    vec4 ambientColour = vec4(directionalLight.colour, 1.0f) * directionalLight.ambientIntensity;

    // Getting the cosine of angle between two vectors
    float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0f);
    vec4 diffuseColour = vec4(directionalLight.colour, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    color = texture(theTexture, texCoord) * (ambientColour + diffuseColour);
}
