// STB
#define STB_IMAGE_IMPLEMENTATION

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
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Utilities.h"
#include "Material.h"

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Our main window
Window mainWindow;

// Creating a vector of the meshes and shaders
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

// Camera
Camera camera;

// Textures
Texture brickTexture;
Texture dirtTexture;

// Lights
Light mainLight;

// Materials
Material shinyMat;
Material roughMat;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

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
    //  x      y      z         u     v         Nx    Ny    Nz
        -1.0f, -1.0f, -0.6f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.5f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -0.6f,     1.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.5f, 1.0f,     0.0f, 0.0f, 0.0f
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

    // Object 1
    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 32, 12);

    // Adding to our meshlist
    meshList.push_back(obj1);

    // Object 2
    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, 32, 12);

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

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);

    // Setting up Textures
    brickTexture = Texture("D:/Programs/C++/Computer_Graphics_TCD/src/Textures/brickHi.png");
    brickTexture.loadTexture();
    dirtTexture = Texture("D:/Programs/C++/Computer_Graphics_TCD/src/Textures/mud.png");
    dirtTexture.loadTexture();

    // Setting up Materials
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(1.0f, 32);
    roughMat = Material(0.25f, 4);

    // Setting up lights
    mainLight = Light( 1.0f, 1.0f, 1.0f, 0.2f,
                       2.0f, -1.0f, -2.0f, 0.75f );

    // Setting the variables
    GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0,
            uniformAmbientIntensity = 0, uniformAmbientColour = 0,
            uniformDiffuseIntensity = 0, uniformDirection = 0,
            uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;

    glm::mat4 projection = glm::perspective(45.0f, GLfloat(mainWindow.getBufferWidht())/GLfloat(mainWindow.getBufferHeight()), 0.1f, 100.0f);

    // Main Loop - Running till the window is open
    while(!mainWindow.getShouldClose()) {
        // For Delta Time
        GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
        deltaTime = now - lastTime;  // (now - lastTime)*1000 / SDL_GetPerformanceFrequency(); - ms
        lastTime = now;

        // Get + Handle user input events
        glfwPollEvents();

        // Camera Key Controls
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear window
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding the program
        shaderList[0]->useShader();
        uniformModel = shaderList[0]->getModelLocation();
        uniformProjection = shaderList[0]->getProjectionLocation();
        uniformView = shaderList[0]->getViewLocation();
        uniformAmbientColour = shaderList[0]->getAmbientColourLocation();
        uniformAmbientIntensity = shaderList[0]->getAmbientIntensityLocation();
        uniformDiffuseIntensity = shaderList[0]->getDiffuseIntensityLocation();
        uniformDirection = shaderList[0]->getDiffuseDirectionLocation();
        uniformEyePosition = shaderList[0]->getEyePositionLocation();
        uniformSpecularIntensity = shaderList[0]->getSpecularIntensityLocation();
        uniformShininess = shaderList[0]->getShininessLocation();

        mainLight.useLight( uniformAmbientIntensity, uniformAmbientColour,
                            uniformDiffuseIntensity, uniformDirection );

            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
            glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
                                            camera.getCameraPosition().y,
                                            camera.getCameraPosition().z);

            glm::mat4 model = glm::mat4(1.0f);
            // Happens in a reverse order
            // Translate
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
            // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Texturing the Mesh
            brickTexture.useTexture();
            shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[0]->renderMesh();

            // Clearing out the properties
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 3.0f, -2.5f));
            // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Texturing the Mesh
            dirtTexture.useTexture();
            roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
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
