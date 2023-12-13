#include <iostream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <GLFW/glfw3.h>

#include "Mesh.h"

// Constructor
Mesh::Mesh() {
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
}

void Mesh::createMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices) {
    // Getting the number of indices
    indexCount = numOfIndices;

    // Creating and gettting the vertex ID of a VAO
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

        // Creating the Index Buffer Object
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

            // Size of value we are passing in
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

        // Creating the buffer object inside VAO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // STATIC DRAW - Not chaning the values in the array
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

            // Location = 0
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

        // Un-Binding Buffer Array
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Un-Binding Vertex Array
    glBindVertexArray(0);

    // Un-Binding IBO/EBO after VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::renderMesh() {
    // Fail conditions
    if(!VAO) {
        std::cout<<"VAO NOT DEFINED PROPERLY!";
        return;
    }

    if(!VBO) {
        std::cout<<"VBO NOT DEFINED PROPERLY!";
        return;
    }

    if(!IBO) {
        std::cout<<"IBO NOT DEFINED PROPERLY!";
        return;
    }

    // Binding the Vertex Array for Drawing
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        // Drawing the Elements (Since we are using Indexed arrays)
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::cleanMesh() {
    // Preventing overflow, garbage collection
    if(IBO) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if(VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    // Preventing overflow, garbage collection
    if(VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}

// Failsafe, in case we accidentally delete the mesh
Mesh::~Mesh() {
    cleanMesh();
}
