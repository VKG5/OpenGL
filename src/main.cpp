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
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Utilities.h"
#include "Material.h"

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Our main window
Window mainWindow;

// Creating a vector of the meshes and shaders
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Camera
Camera camera;

// Textures
Texture brickTexture;
Texture dirtTexture;

// Lights - 1 Directional, Multiple Point
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Materials
Material shinyMat;
Material roughMat;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Yumi/src/Shaders/shader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Yumi/src/Shaders/shader.frag";

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

    unsigned int indicesFloor[] = {
        0, 1, 2,
        1, 2, 3
    };

    GLfloat verticesFloor[] = {
        -10.0f, 0.0f, -10.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,    10.0f, 0.0f,    0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,    0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,     10.0f, 10.0f,   0.0f, -1.0f, 0.0f
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

    // Object 3
    Mesh* obj3 = new Mesh();
    obj3->createMesh(verticesFloor, indicesFloor, 32, 6);

    // Adding to our meshlist
    meshList.push_back(obj3);
}

void createShaders() {
    Shader* shader1 = new Shader();
    shader1->createFromFiles(vertexShader, fragmentShader);

    shaderList.push_back(*shader1);
}

int main() {
    mainWindow = Window(1280, 768);
    mainWindow.initialize();

    createObjects();
    createShaders();

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);

    // Setting up Textures
    brickTexture = Texture("D:/Programs/C++/Yumi/src/Textures/brickHi.png");
    brickTexture.loadTextureA();
    dirtTexture = Texture("D:/Programs/C++/Yumi/src/Textures/mud.png");
    dirtTexture.loadTextureA();

    // Setting up Materials
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(1.0f, 256);
    roughMat = Material(0.25f, 4);

    // Setting up lights
    mainLight = DirectionalLight( 1.0f, 1.0f, 1.0f,
                                  0.2f, 0.75f,
                                  2.0f, -1.0f, -2.0f );
    // Point Lights
    unsigned int pointLightCount = 0;
    pointLights[0] = PointLight( 0.0f, 0.0f, 1.0f,
                                 0.2f, 1.0f,
                                 -2.0f, 0.0f, 0.0f,
                                 0.3f, 0.2f, 0.1f );
    pointLightCount++;

    pointLights[1] = PointLight( 0.0f, 1.0f, 0.0f,
                                 0.3f, 1.0f,
                                 0.0f, 0.0f, 0.0f,
                                 0.3f, 0.1f, 0.1f );
    pointLightCount++;

    // Spot Lights
    unsigned int spotLightCount = 0;
    // This is our torch
    spotLights[0] = SpotLight(  1.0f, 1.0f, 1.0f,
                                0.2f, 0.1f,
                                5.0f, 0.0f, 5.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.7f, 0.3f,
                                12.5f );
    spotLightCount++;

    // Setting the variables
    GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0,
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
        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectionLocation();
        uniformView = shaderList[0].getViewLocation();

        // Specular Light
        uniformEyePosition = shaderList[0].getEyePositionLocation();
        uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
        uniformShininess = shaderList[0].getShininessLocation();

        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.369f;
        // Getting torch control
        spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

        // Lights
        shaderList[0].setDirectionalLight(&mainLight);
        shaderList[0].setPointLight(pointLights, pointLightCount);
        shaderList[0].setSpotLight(spotLights, spotLightCount);

            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
            glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
                                            camera.getCameraPosition().y,
                                            camera.getCameraPosition().z);

            // Happens in a reverse order
            // Translate
            // Object 1
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
            // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Texturing the Mesh
            brickTexture.useTexture();
            shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[0]->renderMesh();

            // Clearing out the properties
            // Object 2
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 3.0f, -2.5f));
            // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Texturing the Mesh
            dirtTexture.useTexture();
            roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[1]->renderMesh();

            // Object 3
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
            // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            // Texturing the Mesh
            brickTexture.useTexture();
            shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
            meshList[2]->renderMesh();

        // Un-Binding the program
        glUseProgram(0);

        // We have 2 scenes, one which we are drawing to and one current
        mainWindow.swapBuffers();
    }

    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
