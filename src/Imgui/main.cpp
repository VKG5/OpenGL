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
        uniformSpecularIntensity = 0, uniformShininess = 0, uniformMetalness = 0,
        uniformshadingModel = 0,
        uniformIsShaded = 0, uniformIsWireframe = 0, uniformObjectColor = 0, uniformWireframeColor = 0,
        uniformMaterialPreview = 0,
        uniformMainTexture = 0, uniformNoiseTexture = 0,
        uniformEnvMapping = 0, uniformSkybox = 0, uniformBackgroundColor = 0,
        uniformIsReflection = 0, uniformIsRefraction = 0, uniformIOR = 0, uniformFresnelReflectance = 0, uniformDispersion = 0;

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
Skybox skybox_clouds;
Skybox skybox_lake;
Skybox skybox_christmas;
Skybox skybox_fireplace;
Skybox skybox_shanghai;
Skybox skybox_sky_fire;

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
Model monkey;
Model sphere;
Model teapot;
Model cube;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Yumi/src/Imgui/Shaders/finalShader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Yumi/src/Imgui/Shaders/finalShader.frag";

// Comtains making manual objects
void createObjects() {
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

    // Floor
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
    pointLights[0] = PointLight( 1.0f, 0.0f, 0.0f,
                                 0.35f, 0.25,
                                 -2.0f, 0.0f, 0.0f,
                                 0.3f, 0.2f, 0.1f );
    pointLightCount++;

    pointLights[1] = PointLight( 0.0f, 1.0f, 0.0f,
                                 0.35f, 0.25,
                                 0.0f, 0.0f, 0.0f,
                                 0.3f, 0.1f, 0.1f );
    pointLightCount++;

    pointLights[2] = PointLight( 0.0f, 0.0f, 1.0f,
                                 0.35f, 0.25,
                                 2.0f, 0.0f, 1.0f,
                                 0.3f, 0.1f, 0.1f );
    pointLightCount++;

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
    // Front, Back
    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Clouds/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_clouds = Skybox(skyboxFaces, true);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/right.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/left.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/top.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/bottom.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/front.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Lake/back.jpg");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_lake = Skybox(skyboxFaces, false);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Christmas/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_christmas = Skybox(skyboxFaces, true);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Fireplace/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_fireplace = Skybox(skyboxFaces, true);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Shanghai/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_shanghai = Skybox(skyboxFaces, true);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Imgui/Textures/Skybox/Sky_Fire/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_sky_fire = Skybox(skyboxFaces, true);
}

// Contains textures, objects and materials
void prepareObjects() {
    // Setting up Textures=============================================================================================
    whiteTexture = Texture("D:/Programs/C++/Yumi/src/Imgui/Textures/white.jpg");
    whiteTexture.loadTexture();

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
    monkey = Model();
    monkey.loadModel("D:/Programs/C++/Yumi/src/Imgui/Models/monkey.obj");

    sphere = Model();
    sphere.loadModel("D:/Programs/C++/Yumi/src/Imgui/Models/sphere.obj");

    teapot = Model();
    teapot.loadModel("D:/Programs/C++/Yumi/src/Imgui/Models/teapot.obj");

    cube = Model();
    cube.loadModel("D:/Programs/C++/Yumi/src/Imgui/Models/cube.obj");
}

// Global parameter for rotating the objects
float rotationAngle = 0.0f;
float step = 0.001f;
float size = 12.0f;

// We are replacing all the texture with realisitc textures to show-case PBR
void renderScene() {
    // Cubes=======================================================================================================
    // Monkey
    // Rotation values
    if(rotationAngle < 360.0f)
        rotationAngle += step;

    else
        rotationAngle = 0.0f;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

    // Rotating the model
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));

    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Texturing the Mesh
    extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);
    monkey.renderModel();

    // Sphere
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(6.0f, 1.0f, 0.0f));

    // Rotating the model
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Texturing the Mesh
    extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);
    sphere.renderModel();

    // Teapot
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-6.0f, -0.7f, 0.0f));

    // Rotating the model
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Texturing the Mesh
    extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);
    teapot.renderModel();

    // Cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-12.0f, 1.0f, 0.0f));

    // Rotating the model
    model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0, 0.0f));

    // Scaling the model
    model = glm::scale(model, glm::vec3(size, size, size));

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Texturing the Mesh
    extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);
    cube.renderModel();
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

    // Clear window
    glClearColor( mainGUI.getBackgroundColor().x,
                  mainGUI.getBackgroundColor().y,
                  mainGUI.getBackgroundColor().z,
                  mainGUI.getBackgroundColor().w );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Checking for Skybox parameter in UI
    if(mainGUI.getIsSkyBox()) {
        // Drawing the Skybox before everything else
        // Checking Skybox index
        switch(mainGUI.getSkyboxIndex()) {
            case 1 : {
                skybox_lake.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }

            case 2 : {
                skybox_christmas.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }

            case 3 : {
                skybox_fireplace.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }

            case 4 : {
                skybox_clouds.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }

            case 5 : {
                skybox_shanghai.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }

            case 6 : {
                skybox_sky_fire.drawSkybox(viewMatrix, projectionMatrix);
                break;
            }
        }
    }

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

    // Updating the camera speed
    camera.updateMoveSpeed(mainGUI.getCameraSpeed());

    // Setting camera using GUI
    camera.setPosition(glm::vec3(mainGUI.getCameraPosition()[0], mainGUI.getCameraPosition()[1], mainGUI.getCameraPosition()[2]));

    // Handling input events exclusive to the GLFW Window
    if(!mainGUI.getIO().WantCaptureMouse) {
        // If the cursor is disabled, don't update the camera
        if(cursorDisabled) {
            // Camera Key Controls
            camera.keyControl(mainWindow.getKeys(), deltaTime);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

            mainGUI.setCameraPosition( camera.getCameraPosition().x,
                                       camera.getCameraPosition().y,
                                       camera.getCameraPosition().z );
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

    // Binding the program
    shaderList[0].useShader();
    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();

    // Specular Light
    uniformEyePosition = shaderList[0].getEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();
    uniformMetalness = shaderList[0].getMetalnessLocation();

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

    // Transmission
    uniformIsReflection = shaderList[0].getIsReflectionLocation();
    uniformIsRefraction = shaderList[0].getIsRefractionLocation();
    uniformIOR = shaderList[0].getIORLocation();
    uniformFresnelReflectance = shaderList[0].getFresnelReflectance();
    uniformDispersion = shaderList[0].getDispersionLocation();

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

    // Transmission effects
    glUniform1i(uniformIsReflection, mainGUI.getIsReflection());
    glUniform1i(uniformIsRefraction, mainGUI.getIsRefraction());
    glUniform1f(uniformIOR, mainGUI.getIOR());
    glUniform1f(uniformFresnelReflectance, mainGUI.getFresnelReflectance());
    glUniform1f(uniformDispersion, mainGUI.getDispersion());

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
