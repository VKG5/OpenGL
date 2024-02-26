#version 460 core

in vec4 col;
in vec2 texCoord;
flat in vec3 Normal;

out vec4 colour;

// Object Color
uniform vec4 objectColor;
uniform vec4 wireframeColor;
uniform bool materialPreview;

// Wireframe or Shaded
uniform bool isWireframe;
uniform bool isShaded;

void main() {
    // If we are in wireframe mode
    if(isWireframe) {
        colour = wireframeColor;
    }

    else {
        colour = objectColor;
    }
}
