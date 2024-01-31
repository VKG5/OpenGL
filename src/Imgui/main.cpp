// STB
#define STB_IMAGE_IMPLEMENTATION

// ImGUI
#include "GUI.h"

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

// Custom Models
#include "Model.h"

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Setting the variables
GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0,
        uniformshadingModel = 0;

// Our main window
Window mainWindow;
GUI mainGUI;

// Creating a vector of the meshes and shaders
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

// Camera
Camera camera;

// Switching between GLFW and ImGUI
bool cursorDisabled = true;

// Textures
Texture brickTexture;
Texture dirtTexture;
Texture whiteTexture;
Texture blackTexture;

// Lights - 1 Directional, Multiple Point
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Materials
Material shinyMat;
Material roughMat;
Material extraRoughMat;
Material extraShinyMat;

// We have 4 shading models that can be iterated through
// 1. Phong Illumination
// 2. Blinn-Phong Illumination
// 3. Gooch Shading
// 4. Photo-realisitc Shading
int shadingModel = 0;

// Models
Model cube;
Model cube1;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Rendering/OpenGL/src/Imgui/Shaders/finalShader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Rendering/OpenGL/src/Imgui/Shaders/finalShader.frag";

// Point Lights
unsigned int pointLightCount = 0;

// Spot Lights
unsigned int spotLightCount = 0;

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

void createLights() {
    // Setting up lights
    mainLight = DirectionalLight( 1.0f, 0.85f, 0.65f,
                                  0.05f, 0.35f,
                                  2.0f, -1.0f, -2.0f );

    // Point Lights
    pointLights[0] = PointLight( 1.0f, 0.0f, 0.0f,
                                 0.35f, 0.25f,
                                 -2.0f, 0.0f, 0.0f,
                                 0.3f, 0.2f, 0.1f );
    // pointLightCount++;

    pointLights[1] = PointLight( 0.0f, 1.0f, 0.0f,
                                 0.35f, 0.25f,
                                 0.0f, 0.0f, 0.0f,
                                 0.3f, 0.1f, 0.1f );
    // pointLightCount++;

    pointLights[2] = PointLight( 0.0f, 0.0f, 1.0f,
                                 0.35f, 0.25f,
                                 2.0f, 0.0f, 1.0f,
                                 0.3f, 0.1f, 0.1f );
    // pointLightCount++;

    // This is our torch
    spotLights[0] = SpotLight(  1.0f, 1.0f, 1.0f,
                                0.25f, 1.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f );
    spotLightCount++;
}

// Global parameter for rotating the objects
float rotationAngle = 0.0f;
float step = 0.005f;

// We are replacing all the texture with realisitc textures to show-case PBR
void renderSceneRealisitc() {
        // Happens in a reverse order
        // Translate
        // Object 1
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        brickTexture.useTexture();
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
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
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[2]->renderMesh();

        // Cubes=======================================================================================================
        // Cube 1
        // Rotation values
        if(rotationAngle < 360.0f)
            rotationAngle += step;

        else
            rotationAngle = 0.0f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        extraRoughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();
}

// We are replacing all the texture with a plain white material to show-case toon shading better
void renderSceneNPR() {
        // Happens in a reverse order
        // Translate
        // Triangle 1
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->renderMesh();

        // Clearing out the properties
        // Triangle 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, -2.5f));
        // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[1]->renderMesh();

        // Ground
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[2]->renderMesh();

        // Cubes=======================================================================================================
        // Cube 1
        // Rotation values
        if(rotationAngle < 360.0f)
            rotationAngle += step;

        else
            rotationAngle = 0.0f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 3
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();

        // Cube 4
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-6.0f, 1.0f, 0.0f));

        // Rotating the model
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        // Texturing the Mesh
        whiteTexture.useTexture();
        extraRoughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
        cube.renderModel();
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // Setting initial GLFW Window
    glViewport(0, 0, 1366, 768);

    // Clear window
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // UI - Render Frame
    mainGUI.newFrame();

    // Toggle cursor mode on pressing the 'C' key
    if (mainWindow.getKeys()[GLFW_KEY_C]) {
        // Altering the state
        cursorDisabled = !cursorDisabled;

        // Debugging
        // printf("Pressed C!\n%i", cursorDisabled);

        mainWindow.changeState(cursorDisabled);

        // Preventing multiple inputs
        mainWindow.getKeys()[GLFW_KEY_C] = false;
    }

    // Handling input events exclusive to the GLFW Window
    if(!mainGUI.getIO().WantCaptureMouse) {
        // If the cursor is disabled, don't update the camera
        if(cursorDisabled) {
            // Camera Key Controls
            camera.keyControl(mainWindow.getKeys(), deltaTime);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        }

        // Toggling Spot Light on pressing L
        if(mainWindow.getKeys()[GLFW_KEY_L]) {
            // Debugging
            // printf("Pressed L!");

            spotLights[0].toggle();
            mainWindow.getKeys()[GLFW_KEY_L] = false;
        }

        if(mainWindow.getKeys()[GLFW_KEY_B]) {
            // Debugging
            // printf("Pressed B!\n");

            shadingModel = (shadingModel + 1) % MAX_SHADING_MODELS;

            // Debugging
            // std::cout<<shadingModel<<"\n";

            // To prevent multiple clicks in this loop
            mainWindow.getKeys()[GLFW_KEY_B] = false;
        }
    }

    // Binding the program
    shaderList[0].useShader();
    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();

    // Specular Light
    uniformEyePosition = shaderList[0].getEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();

    // Getting UseBlinn
    uniformshadingModel = shaderList[0].getUniformshadingModel();

    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.369f;
    // Getting torch control
    spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

    // Lights
    shaderList[0].setDirectionalLight(&mainLight);
    shaderList[0].setPointLight(pointLights, pointLightCount);
    shaderList[0].setSpotLight(spotLights, spotLightCount);

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
                                    camera.getCameraPosition().y,
                                    camera.getCameraPosition().z);

    glUniform1i(uniformshadingModel, shadingModel);

    if(shadingModel < 2 || shadingModel == 3) {
        renderSceneRealisitc();
    }

    else if(shadingModel == 2) {
        renderSceneNPR();
    }

    else {
        printf("Invalid Shading Model!");
    }

    // Drawing the UI
    mainGUI.render();
}

int main() {
    mainWindow = Window(1280, 768);
    mainWindow.initialize();

    createObjects();
    createShaders();

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);

    // Setting up Textures=============================================================================================
    brickTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Textures/brickHi.png");
    brickTexture.loadTextureA();
    dirtTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Textures/mud.png");
    dirtTexture.loadTextureA();
    whiteTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Textures/white.jpg");
    whiteTexture.loadTexture();
    blackTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Textures/black.jpg");
    blackTexture.loadTexture();

    // Setting up Materials============================================================================================
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(1.0f, 256);
    roughMat = Material(0.5f, 32);
    extraRoughMat = Material(0.125f, 2);
    extraShinyMat = Material(1.0f, 1024);

    // Loading Models==================================================================================================
    cube = Model();
    cube.loadModel("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Models/monkey.obj");
    cube1 = Model();
    cube1.loadModel("D:/Programs/C++/Rendering/OpenGL/src/Imgui/Models/monkey1.obj");

    // Creating the lights=============================================================================================
    createLights();

    glm::mat4 projection = glm::perspective(45.0f, GLfloat(mainWindow.getBufferWidht())/GLfloat(mainWindow.getBufferHeight()), 0.1f, 100.0f);

    // ImGUI===========================================================================================================
    mainGUI.initialize(mainWindow.getWindow());

    // Main Loop - Running till the window is open=====================================================================
    while(!mainWindow.getShouldClose()) {
        // For Delta Time
        GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
        deltaTime = now - lastTime;  // (now - lastTime)*1000 / SDL_GetPerformanceFrequency(); - ms
        lastTime = now;

        // Get + Handle user input events
        glfwPollEvents();

        // Handles the rendering of each elements - UI, GLFW, Objects, etc.
        renderPass(projection, camera.calculateViewMatrix());

        // Un-Binding the program
        glUseProgram(0);

        // We have 2 scenes, one which we are drawing to and one current
        mainWindow.swapBuffers();
    }

    // Shutting down ImGUI
    mainGUI.shutdown();

    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
