#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

// We have 6 projections in our cube
uniform mat4 lightMatrices[6];

out vec4 fragPos;

void main() {
    // Iterating over 6 faces/textures of the cube
    for(int face = 0; face < 6; face++) {
        gl_Layer = face;

        // Iterating over each triangle we pass
        for(int i = 0; i < 3; i++) {
            fragPos = gl_in[i].gl_Position;
            gl_Position = lightMatrices[face] * fragPos;

            // Stores the vertex at the layer specified
            EmitVertex();
        }

        // End draw of Triangle Strip, move to next one
        EndPrimitive();
    }
}
