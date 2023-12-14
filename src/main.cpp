// Basic C++ Libraries for various operations
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <GLFW/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Libraries
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Our main window
Window mainWindow;

// Creating a vector of the meshes and shaders
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Computer_Graphics_TCD/src/Shaders/shader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Computer_Graphics_TCD/src/Shaders/shader.frag";

void createObjects() {
    // Index Array
    // Index of the vertices that are being drawn
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    // A VAO can hold multiple VBOs and other types of buffers
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Object 1
    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 12, 12);

    // Adding to our meshlist
    meshList.push_back(obj1);

    // Object 2
    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, 12, 12);

    // Adding to our meshlist
    meshList.push_back(obj2);
}

void createShaders() {
    Shader* shader1 = new Shader();
    shader1->createFromFiles(vertexShader, fragmentShader);

    shaderList.push_back(shader1);
}

int main() {
    mainWindow = Window(1280, 768);
    mainWindow.initialize();

    createObjects();
    createShaders();

    // Setting the variables
    GLuint uniformProjection = 0, uniformModel = 0;

    glm::mat4 projection = glm::perspective(45.0f, GLfloat(mainWindow.getBufferWidht())/GLfloat(mainWindow.getBufferHeight()), 0.1f, 100.0f);

    // Main Loop - Running till the window is open
    while(!mainWindow.getShouldClose()) {
        // Get + Handle user input events
        glfwPollEvents();

        // Clear window
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding the program
        shaderList[0]->useShader();
        uniformModel = shaderList[0]->getModelLocation();
        uniformProjection = shaderList[0]->getProjectionLocation();

            glm::mat4 model = glm::mat4(1.0f);
            // Happens in a reverse order
            // Translate
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            meshList[0]->renderMesh();

            // Clearing out the properties
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            meshList[1]->renderMesh();

        // Un-Binding the program
        glUseProgram(0);

        // We have 2 scenes, one which we are drawing to and one current
        mainWindow.swapBuffers();
    }

    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
