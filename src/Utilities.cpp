#include "Utilities.h"

// Averaging Normals for Phong Shading
void calcAverageNormals( unsigned int* indices, unsigned int indexCount,
                         GLfloat* vertices, unsigned int vertexCount,
                         unsigned int vLength, unsigned int normalOffset ) {
    // Iterating over the indices
    for(size_t i = 0; i < indexCount; i += 3) {
        unsigned int index1 = indices[i] * vLength;
        unsigned int index2 = indices[i+1] * vLength;
        unsigned int index3 = indices[i+2] * vLength;

        // Creating edges
        glm::vec3 v1(   vertices[index2] - vertices[index1],
                        vertices[index2 + 1] - vertices[index1 + 1],
                        vertices[index2 + 2] - vertices[index1 + 2] );

        glm::vec3 v2(   vertices[index3] - vertices[index1],
                        vertices[index3 + 1] - vertices[index1 + 1],
                        vertices[index3 + 2] - vertices[index1 + 2] );

        glm::vec3 normal = glm::cross(v1, v2);

        normal = glm::normalize(normal);

        // Skipping the values by required places
        index1 += normalOffset; index2 += normalOffset; index3 += normalOffset;

        vertices[index1] += normal.x; vertices[index1+1] += normal.y; vertices[index1+2] += normal.z;
        vertices[index2] += normal.x; vertices[index2+1] += normal.y; vertices[index2+2] += normal.z;
        vertices[index3] += normal.x; vertices[index3+1] += normal.y; vertices[index3+2] += normal.z;
    }

    // Normalizing the normal vector
    for(size_t i=0; i < vertexCount/vLength; i++) {
        unsigned int NOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[NOffset], vertices[NOffset + 1], vertices[NOffset + 2]);
        vec = glm::normalize(vec);
        vertices[NOffset] = vec.x; vertices[NOffset + 1] = vec.y; vertices[NOffset + 2] = vec.z;
    }
}
