// STB
#define STB_IMAGE_IMPLEMENTATION

// Including all necessary headers
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

// Skybox
#include "Skybox.h"

// Procedural Content Generation
#include "randomDistribute.h"

// Defining all necessary variables
// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Seed for Procedural Content
GLuint seed = 165316;

// Random buildings variables
int gridSize = 20, pointSize = 2, numPoints = 20;
std::vector<std::pair<int, int>> randomPoints;
std::vector<glm::vec3> randomScales;
std::vector<int> randomHeights;

// Setting a random function to use
std::mt19937 gen(seed);

// Setting the variables
GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0, uniformRoughness = 0,
        uniformshadingModel = 0,
        uniformIsShaded = 0, uniformIsWireframe = 0, uniformObjectColor = 0, uniformWireframeColor = 0,
        uniformMaterialPreview = 0,
        uniformMainTexture = 0, uniformNoiseTexture = 0,
        uniformEnvMapping = 0, uniformSkybox = 0, uniformBackgroundColor = 0;

// Our main window
Window mainWindow;
GUI mainGUI;
std::string shadingMode = "Phong Illumination";

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

// Noise Texture for random maps
// Texture noiseTexture;

// Lights - 1 Directional, Multiple Point, Multiple Spot
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Point Lights
unsigned int pointLightCount = 0;

// Spot Lights
unsigned int spotLightCount = 0;

// Skybox
Skybox skybox;

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
Model building0;
Model building1;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Yumi/src/Procedural/Shaders/finalShader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Yumi/src/Procedural/Shaders/finalShader.frag";

// Comtains making manual objects
void createObjects() {
    // Index Array
    unsigned int indicesFloor[] = {
        0, 1, 2,
        1, 2, 3
    };

    GLfloat verticesFloor[] = {
        20.0f, 0.0f, -20.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, -20.0f,    10.0f, 0.0f,    0.0f, -1.0f, 0.0f,
        20.0f, 0.0f, 0.0f,     0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,      10.0f, 10.0f,   0.0f, -1.0f, 0.0f
    };

    // Object 3
    Mesh* floor = new Mesh();
    floor->createMesh(verticesFloor, indicesFloor, 32, 6);

    // Adding to our meshlist
    meshList.push_back(floor);
}

// Contains preparing base shaders
void createShaders() {
    Shader* shader1 = new Shader();
    shader1->createFromFiles(vertexShader, fragmentShader);

    shaderList.push_back(*shader1);
}

// Contains the properties for lights and the Skybox
void createLights() {
    // Setting up lights
    mainLight = DirectionalLight( 1.0f, 0.85f, 0.65f,
                                  0.5f, 0.35f,
                                  2.0f, -1.0f, -2.0f );

    // Point Lights
    // pointLights[0] = PointLight( 1.0f, 0.0f, 0.0f,
    //                              0.35f, 0.25,
    //                              -2.0f, 0.0f, 0.0f,
    //                              0.3f, 0.2f, 0.1f );
    // pointLightCount++;

    // This is our torch
    spotLights[0] = SpotLight(  1.0f, 1.0f, 1.0f,
                                0.25f, 1.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f );
    spotLightCount++;

    // Skybox==========================================================================================================
    std::vector<std::string> skyboxFaces;
    // Pushing the textures in a particular order
    // Right, Left
    // Up, Down
    // Back, Front
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_rt.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_lf.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_up.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_dn.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_bk.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Procedural/Textures/Skybox/cloudtop_ft.tga");

    skybox = Skybox(skyboxFaces);
}

// Contains textures, objects and materials
void prepareObjects() {
    // Setting up Textures=============================================================================================
    brickTexture = Texture("D:/Programs/C++/Yumi/src/Procedural/Textures/brickHi.png");
    brickTexture.loadTextureA();

    // Generated Noise Texture
    // Parameters - Width, Height, Channels = 3 (Use 3 channels - RGB)
    // noiseTexture = Texture();
    // noiseTexture.generateRandomTexture(2048, 2048, 3);

    // Setting up Materials============================================================================================
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(1.0f, 256, 0.1f);
    roughMat = Material(0.5f, 32, 0.8f);
    extraRoughMat = Material(0.125f, 2, 1.0f);
    extraShinyMat = Material(1.0f, 1024, 0.0125f);

    // Loading Models==================================================================================================
    building0 = Model();
    building0.loadModel("D:/Programs/C++/Yumi/src/Procedural/Models/buildings.obj");

    building1 = Model();
    building1.loadModel("D:/Programs/C++/Yumi/src/Procedural/Models/buildings_2.obj");
}

// We are replacing all the texture with realisitc textures to show-case PBR
void renderScene() {
    // Floor
    glm::mat4 floor = glm::mat4(1.0f);
    floor = glm::translate(floor, glm::vec3(mainGUI.getFloorOffset()[0],
                                            mainGUI.getFloorOffset()[1],
                                            mainGUI.getFloorOffset()[2]));
    floor = glm::scale(floor, glm::vec3(mainGUI.getFloorScale()[0],
                                        mainGUI.getFloorScale()[1],
                                        mainGUI.getFloorScale()[2]));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(floor));
    brickTexture.useTexture();
    roughMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformRoughness);
    meshList[0]->renderMesh();

    // Buildings=======================================================================================================
    // Randomly placing the buildings
    for (size_t i=0; i < randomPoints.size(); i++) {
        const auto& point = randomPoints[i];
        const auto& scale = randomScales[i];
        const auto& height = randomHeights[i];

        for (int j = 0; j < height + 1; j++) {
            float heightOffset = 0.0f;
            // TODO : Optimize
            if (point.first % 3 == 0) {
                heightOffset = 2.64f;
            }

            else if (point.first % 3 == 1) {
                heightOffset = 3.29f;
            }

            else {
                continue;
            }

            glm::mat4 buil = glm::mat4(1.0f);
            buil = glm::translate(buil, glm::vec3(point.first, j * (heightOffset * scale.y), -point.second));
            buil = glm::scale(buil, scale);

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(buil));
            extraRoughMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformRoughness);

            // Randomly pick a model
            // int modelIndex = modelDistribution(gen);

            // Determine which model to render based on some criteria (e.g., point coordinates)
            if (point.first % 3 == 0) {
                building0.renderModel();
            }

            else if (point.first % 3 == 1) {
                building1.renderModel();
            }

            else {
                continue;
            }
        }
    }
}

void setShadingModeName() {
    if(mainGUI.getMaterialPreview()) {
        shadingMode = "Material Preview";
    }

    else if(mainGUI.getIsWireframe()) {
        shadingMode = "Wireframe";
    }

    else if(shadingModel == 0) {
        shadingMode = "Phong Illumination";
    }

    else if(shadingModel == 1) {
        shadingMode = "Blinn-Phong Illumination";
    }

    else if(shadingModel == 2) {
        shadingMode = "Gooch Shading";
    }

    else if(shadingModel == 3) {
        shadingMode = "Minnaert Shading";
    }

    else {
        shadingMode = "None";
    }
}

// Render Pass - Renders all data in the scene
void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // Setting initial GLFW Window
    // glViewport(0, 0, 1366, 768);

    // Clear window====================================================================================================
    glClearColor( mainGUI.getBackgroundColor().x,
                  mainGUI.getBackgroundColor().y,
                  mainGUI.getBackgroundColor().z,
                  mainGUI.getBackgroundColor().w );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Checking for Skybox parameter in UI
    if(mainGUI.getIsSkyBox()) {
        // Drawing the Skybox before everything else
        skybox.drawSkybox(viewMatrix, projectionMatrix);
    }

    // UI - Render Frame===============================================================================================
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

    // Updating the camera speed
    camera.updateMoveSpeed(mainGUI.getCameraSpeed());

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

    // Switching modes - Wireframe/Normal
    if(mainGUI.getIsWireframe()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Updating the Procedural Content on button press
    if(mainGUI.getUpdate()) {
        // Debugging
        // printf("In update!\n");
        // printf("Grid : %i\nPoint : %i\nNum : %i\nSeed : %i\n",mainGUI.getGridSize(),
        //                                                     mainGUI.getPointSize(),
        //                                                     mainGUI.getNumPoints(),
        //                                                     mainGUI.getSeed() );

        generateRandomPoints(randomPoints,
                             mainGUI.getGridSize(),
                             mainGUI.getPointSize(),
                             mainGUI.getNumPoints(),
                             mainGUI.getSeed() );

        generateRandomScales(randomScales, randomPoints.size(), seed);
        generateRandomHeights(randomHeights, randomPoints.size(), seed);

        // To prevent multiple clicking
        mainGUI.setUpdate(false);
    }

    // Binding the program=============================================================================================
    shaderList[0].useShader();
    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();

    // Specular Light
    uniformEyePosition = shaderList[0].getEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();
    uniformRoughness = shaderList[0].getRoughnessLocation();

    // Getting Shading Mode
    uniformshadingModel = shaderList[0].getShadingModelLocation();

    /*
    Getting UI Elements
    1. isWireframe - Enable/Disable Wireframe
    2. isShaded - Enbable/Disable Textures
    3. wireframeColor - Sets the color of the wireframe
    4. objectColor - Sets the color of the objects in the scene
    5. materialPreview - Disables all lighting to view the textures only
    */
    uniformMaterialPreview = shaderList[0].getMaterialPreviewLocation();
    uniformIsShaded = shaderList[0].getIsShadedLocation();
    uniformIsWireframe = shaderList[0].getIsWireframeLocation();
    uniformWireframeColor = shaderList[0].getWireframeColourLocation();
    uniformObjectColor = shaderList[0].getObjectColorLocation();
    uniformEnvMapping = shaderList[0].getEnvMappingLocation();
    uniformSkybox = shaderList[0].getSkyboxLocation();
    uniformBackgroundColor = shaderList[0].getBackgroundColourLocation();

    // Getting the texture locations
    uniformMainTexture = shaderList[0].getMainTextureLocation();

    // Binding the texture to correct texture units
    shaderList[0].setTexture(uniformMainTexture, 0);

    // TODO : Intergrate this to work like a proper roughness map
    // uniformNoiseTexture = shaderList[0].getNoiseTextureLocation();
    // shaderList[0].setTexture(uniformNoiseTexture, 1);

    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.369f;
    // Getting torch control
    spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

    // Lights
    // Directional Light
    mainLight.setDirLight( mainGUI.getDirectionalLightColor()[0],
                           mainGUI.getDirectionalLightColor()[1],
                           mainGUI.getDirectionalLightColor()[2],
                           mainGUI.getDirectionalLightAmbient(),
                           mainGUI.getDirectionalLightDiffuse(),
                           mainGUI.getDirectionalLightDirection()[0],
                           mainGUI.getDirectionalLightDirection()[1],
                           mainGUI.getDirectionalLightDirection()[2] );

    shaderList[0].setDirectionalLight(&mainLight);

    // Currently disabling the Point and Spot lights based off a boolean
    // Point Lights
    if(mainGUI.getIsPointLights()) {
        shaderList[0].setPointLight(pointLights, pointLightCount);
    }

    else {
        shaderList[0].setPointLight(pointLights, 0);
    }

    // Spot Lights
    if(mainGUI.getIsSpotLights()) {
        shaderList[0].setSpotLight(spotLights, spotLightCount);
    }

    else {
        shaderList[0].setSpotLight(spotLights, 0);
    }

    // Binding to uniforms in the shader
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
                                    camera.getCameraPosition().y,
                                    camera.getCameraPosition().z);

    // Setting the shading model
    glUniform1i(uniformshadingModel, shadingModel);

    // Setting the triggers from UI Elements
    glUniform1i(uniformMaterialPreview, mainGUI.getMaterialPreview());
    glUniform1i(uniformIsWireframe, mainGUI.getIsWireframe());
    glUniform1i(uniformIsShaded, mainGUI.getIsShaded());

    glUniform4f(uniformObjectColor, mainGUI.getObjectColor().x,
                                    mainGUI.getObjectColor().y,
                                    mainGUI.getObjectColor().z,
                                    mainGUI.getObjectColor().w );

    glUniform4f(uniformWireframeColor, mainGUI.getWireframeColor().x,
                                       mainGUI.getWireframeColor().y,
                                       mainGUI.getWireframeColor().z,
                                       mainGUI.getWireframeColor().w );

    glUniform1i(uniformEnvMapping, mainGUI.getIsEnvMapping());
    glUniform1i(uniformSkybox, mainGUI.getIsSkyBox());
    glUniform3f(uniformBackgroundColor, mainGUI.getBackgroundColor().x,
                                        mainGUI.getBackgroundColor().y,
                                        mainGUI.getBackgroundColor().z);

    // Added the texture disabling functionality in the shader
    renderScene();

    // Drawing the UI
    setShadingModeName();
    mainGUI.render(shadingMode);
}

int main() {
    mainWindow = Window(1366, 768);
    mainWindow.initialize();

    createObjects();
    createShaders();

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);

    // Setting up objects==============================================================================================
    prepareObjects();

    // Creating the lights=============================================================================================
    createLights();

    glm::mat4 projection = glm::perspective(45.0f, GLfloat(mainWindow.getBufferWidht())/GLfloat(mainWindow.getBufferHeight()), 0.1f, 100.0f);

    // ImGUI===========================================================================================================
    mainGUI.initialize(mainWindow.getWindow());

    // Generating random points - Initially
    generateRandomPoints(randomPoints, gridSize, pointSize, numPoints, seed);
    generateRandomScales(randomScales, randomPoints.size(), seed);
    generateRandomHeights(randomHeights, randomPoints.size(), seed);

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

    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}