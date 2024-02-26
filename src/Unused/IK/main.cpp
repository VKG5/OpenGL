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
#include "MathFuncs.h"
#include "Material.h"

// Custom Models
#include "Model.h"

// Skybox
#include "Skybox.h"

// Procedural Content Generation
#include "randomDistribute.h"

// Get the full path of the current source file
const std::filesystem::path currentSourcePath = __FILE__;

// Extract the directory containing the source file
const std::filesystem::path currentSourceDir = currentSourcePath.parent_path();

// Defining all necessary variables
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
        uniformMaterialPreview = 0, uniformSpecularPreview = 0, uniformNormalPreview = 0,
        uniformDiffuseTexture = 0, uniformAmbientOcclusionTexture = 0, uniformSpecularTexture = 0, uniformNormalTexture = 0,
        uniformNoiseTexture = 0,
        uniformEnvMapping = 0, uniformSkybox = 0, uniformBackgroundColor = 0,
        uniformIsReflection = 0, uniformIsRefraction = 0, uniformIOR = 0, uniformFresnelReflectance = 0, uniformDispersion = 0,
        uniformNormalStrength = 0, uniformSpecularStrength = 0;

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
Texture whiteTexture;

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
// 4. Minnaert Shading
int shadingModel = 0;

// Models
Model* bone;
Model* monkey;
Model* tracker;
Model* animTarget;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
std::string vertexShaderPath = (currentSourceDir / "Shaders/BRDF_Normals.vert").string();
std::string vertexShaderFormatted = removeBackslash(vertexShaderPath.c_str());
static const char* vertexShader = vertexShaderFormatted.c_str();

// Fragment Shader
std::string fragmentShaderPath = (currentSourceDir / "Shaders/BRDF_Normals.frag").string();
std::string fragmentShaderFormatted = removeBackslash(fragmentShaderPath.c_str());
static const char* fragmentShader = fragmentShaderFormatted.c_str();


// Creating Objects====================================================================================================
// Contains making manual objects
const float planeSize = 5.0f;
const float planeUV = 2.5f;

// Function to create Manual Objects
void createObjects() {
    unsigned int indicesFloor[] = {
        0, 1, 2,
        1, 2, 3
    };

    GLfloat verticesFloor[] = {
        // x, y, z                      u, v              Nx, Ny, Nz              Tx, Ty, Tz
        -planeSize, 0.0f, -planeSize,   0.0f, 0.0f,       0.0f, -1.0f, 0.0f,      0.0f, 0.0f, 0.0f,
        planeSize, 0.0f, -planeSize,    planeUV, 0.0f,    0.0f, -1.0f, 0.0f,      0.0f, 0.0f, 0.0f,
        -planeSize, 0.0f, planeSize,    0.0f, planeUV,    0.0f, -1.0f, 0.0f,      0.0f, 0.0f, 0.0f,
        planeSize, 0.0f, planeSize,     planeUV, planeUV, 0.0f, -1.0f, 0.0f,      0.0f, 0.0f, 0.0f
    };

    // Floor
    Mesh* floor = new Mesh();
    floor->createMesh(verticesFloor, indicesFloor, 44, 6);

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
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Clouds/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_clouds = Skybox(skyboxFaces);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/right.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/left.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/top.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/bottom.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/front.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Lake/back.jpg");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_lake = Skybox(skyboxFaces);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Christmas/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_christmas = Skybox(skyboxFaces);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Fireplace/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_fireplace = Skybox(skyboxFaces);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Shanghai/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_shanghai = Skybox(skyboxFaces);

    // Clearing old data
    skyboxFaces.clear();
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/right.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/left.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/top.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/bottom.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/front.png");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/Skybox/Sky_Fire/back.png");

    // Parameters - Skybox faces, RGB/RGBA
    skybox_sky_fire = Skybox(skyboxFaces);
}


// Preparing Objects===================================================================================================
// Contains textures, objects and materials
void prepareObjects() {
    // Setting up Textures=============================================================================================
    whiteTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/white.jpg");
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
    bone = new Model();
    bone->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/bone.obj");

    monkey = new Model();
    monkey->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/monkey.obj");

    tracker = new Model();
    tracker->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/tracker.obj");

    animTarget = new Model();
    animTarget->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/cube.obj");
}


// General Settings for all projects===================================================================================
void generalElements(glm::mat4& projectionMatrix, glm::mat4& viewMatrix) {
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

    // Projection Matrix
    camera.setCameraParameters( mainGUI.getCameraFOV(),
                                mainGUI.getCameraScale(),
                                mainGUI.getCameraNearClipping(),
                                mainGUI.getCameraFarClipping() );

    // Numpad 5 for switching between Ortho and Perspective
    if(mainWindow.getKeys()[GLFW_KEY_KP_5]) {
        if(mainGUI.getCameraIsPerspective()) {
            mainGUI.setCameraIsPerspective(false);
            mainGUI.setCameraIsOrthographic(true);
        }

        else {
            mainGUI.setCameraIsOrthographic(false);
            mainGUI.setCameraIsPerspective(true);
        }

        mainWindow.getKeys()[GLFW_KEY_KP_5] = false;
    }

    // Switching to Perspective
    if(mainGUI.getCameraIsPerspective()) {
        mainGUI.setCameraIsOrthographic(false);
        projectionMatrix = camera.calculatePerspectiveProjectionMatrix(mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
    }

    // Switching to Orthographic
    if(mainGUI.getCameraIsOrthographic()) {
        mainGUI.setCameraIsPerspective(false);
        projectionMatrix = camera.calculateOrthographicProjectionMatrix();
    }

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
}

void getUniforms(Shader& shader) {
    // Binding the program
    shader.useShader();

    uniformModel = shader.getModelLocation();
    uniformProjection = shader.getProjectionLocation();
    uniformView = shader.getViewLocation();

    // Specular Light
    uniformEyePosition = shader.getEyePositionLocation();
    uniformSpecularIntensity = shader.getSpecularIntensityLocation();
    uniformShininess = shader.getShininessLocation();
    uniformMetalness = shader.getMetalnessLocation();

    // Getting Shading Mode
    uniformshadingModel = shader.getShadingModelLocation();

    /*
    Getting UI Elements
    */
    // Object Properties
    uniformIsShaded = shader.getIsShadedLocation();
    uniformIsWireframe = shader.getIsWireframeLocation();
    uniformWireframeColor = shader.getWireframeColourLocation();
    uniformObjectColor = shader.getObjectColorLocation();
    uniformEnvMapping = shader.getEnvMappingLocation();
    uniformSkybox = shader.getSkyboxLocation();
    uniformBackgroundColor = shader.getBackgroundColourLocation();

    // Material Preview - Maps
    uniformMaterialPreview = shader.getMaterialPreviewLocation();
    uniformSpecularPreview = shader.getSpecularPreviewLocation();
    uniformNormalPreview = shader.getNormalPreviewLocation();

    // Material Properties
    uniformIsReflection = shader.getIsReflectionLocation();
    uniformIsRefraction = shader.getIsRefractionLocation();
    uniformIOR = shader.getIORLocation();
    uniformFresnelReflectance = shader.getFresnelReflectance();
    uniformDispersion = shader.getDispersionLocation();
    uniformNormalStrength = shader.getNormalStrengthLocation();
    uniformSpecularStrength = shader.getSpecularStrengthLocation();

    // Texture
    uniformDiffuseTexture = shader.getMainTextureLocation();
    uniformSpecularTexture = shader.getSpecularTextureLocation();
    uniformAmbientOcclusionTexture = shader.getAmbientOcclusionTextureLocation();
    uniformNormalTexture = shader.getNormalTextureLocation();
}

void setUniforms(glm::mat4 projectionMatrix, Shader& shader) {
    // Binding the texture to correct texture units
    shader.setTexture(uniformDiffuseTexture, 0);
    shader.setTexture(uniformSpecularTexture, 1);
    shader.setTexture(uniformNormalTexture, 2);

    // TODO : Intergrate this to work like a proper roughness map
    // uniformNoiseTexture = shader.getNoiseTextureLocation();
    // shader.setTexture(uniformNoiseTexture, 1);

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

    shader.setDirectionalLight(&mainLight);

    // Currently disabling the Point and Spot lights based off a boolean
    // Point Lights
    if(mainGUI.getIsPointLights()) {
        shader.setPointLight(pointLights, pointLightCount);
    }

    else {
        shader.setPointLight(pointLights, 0);
    }

    // Spot Lights
    if(mainGUI.getIsSpotLights()) {
        shader.setSpotLight(spotLights, spotLightCount);
    }

    else {
        shader.setSpotLight(spotLights, 0);
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
    // Material Preview
    glUniform1i(uniformMaterialPreview, mainGUI.getMaterialPreview());
    glUniform1i(uniformSpecularPreview, mainGUI.getSpecularPreview());
    glUniform1i(uniformNormalPreview, mainGUI.getNormalPreview());

    // Object Properties
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

    // Material Properties
    glUniform1i(uniformIsReflection, mainGUI.getIsReflection());
    glUniform1i(uniformIsRefraction, mainGUI.getIsRefraction());
    glUniform1f(uniformIOR, mainGUI.getIOR());
    glUniform1f(uniformFresnelReflectance, mainGUI.getFresnelReflectance());
    glUniform1f(uniformDispersion, mainGUI.getDispersion());
    glUniform1f(uniformNormalStrength, mainGUI.getNormalStrength());
    glUniform1f(uniformSpecularStrength, mainGUI.getSpecularStrength());
}


// IK==================================================================================================================
// Helper functions
// Bone Offset is the initial bone's offset, hence the START of the IK Chain
float boneLength = 1.0f;

// Tolerance for increasing target reach range
float tolerance = 0.01f;

// Max iterations
int maxIterations = 64;

// Speed
float step = 0.005f;

// Children Pointers
std::vector<Model*> bonesList;

// Start Position/Origin
glm::vec3 startPos = glm::vec3(0.0f);
std::vector<glm::mat4> transformMatrices(1, glm::mat4(1.0f));
std::vector<glm::vec3> boneOffsets(1, glm::vec3(0.0f));
std::vector<glm::vec3> rotationAxis(1, glm::vec3(0.0f, 1.0f, 0.0f));
std::vector<float> rotationAngle(1, 0.0f);

// GUI for Debugging
void debuggingGUI( glm::vec3 targetPos,
                   glm::vec3 vecA, glm::vec3 vecB,
                   glm::vec3 rotationAxis, float angle, int boneIndex) {
        ImGui::Begin("Debug Window");
        // Debugging
        ImGui::Spacing();
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Debugging Bone %i", boneIndex);

        ImGui::Spacing();
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("Target Position : %.2f, %.2f, %.2f", targetPos.x, targetPos.y, targetPos.z);
        ImGui::Text("Point A : %.2f, %.2f, %.2f", vecA.x, vecA.y, vecA.z);
        ImGui::Text("Point B : %.2f, %.2f, %.2f", vecB.x, vecB.y, vecB.z);
        ImGui::Text("Rotation Axis : %.2f, %.2f, %.2f", rotationAxis.x, rotationAxis.y, rotationAxis.z);
        ImGui::Text("Current Angle : %.2f", glm::degrees(angle));
        ImGui::End();
}

// Return the total length of the chain
float calcChainLength() {
    return (mainGUI.getChainLength() * boneLength) + tolerance;
}

void backwardPass(const glm::vec3& targetPos) {
    // Debugging
    // printf("Bone Offset Length : %zi\n", boneOffsets.size());
    // for(int i=0; i<boneOffsets.size(); i++) {
    //     printVec3GUI(boneOffsets[i], i+1);
    // }

    // for(int i=0; i<transformMatrices.size(); i++) {
    //     printMat4GUI(transformMatrices[i], i+1);
    // }
    int lastBoneIndex = boneOffsets.size() - 1;

    boneOffsets[lastBoneIndex] = targetPos;

    for(int i = lastBoneIndex - 1; i >= 0; i--) {
        float distance = glm::distance(boneOffsets[i], boneOffsets[i+1]);

        // Optimal Position reached
        if(distance < mainGUI.getEpsilon()) {
            return;
        }

        float length = boneLength;
        boneOffsets[i] = boneOffsets[i+1] + (boneOffsets[i] - boneOffsets[i+1]) * length /  distance;
    }
}

void forwardPass(const glm::vec3& targetPos) {
    int firstBoneIndex = 0;

    boneOffsets[firstBoneIndex] = startPos;

    for(int i = firstBoneIndex + 1; i < boneOffsets.size(); i++) {
        float distance = glm::distance(boneOffsets[i], boneOffsets[i-1]);

        // Optimal Position reached
        if(distance < mainGUI.getEpsilon()) {
            return;
        }

        float length = boneLength;
        boneOffsets[i] = boneOffsets[i-1] + (boneOffsets[i] - boneOffsets[i-1]) * length /  distance;
    }
}

void calculateIK() {
    glm::vec3 targetPos = glm::vec3(mainGUI.getTargetLocation()[0],
                                    mainGUI.getTargetLocation()[1],
                                    mainGUI.getTargetLocation()[2]);

    if(glm::distance(targetPos, bonesList.back()->getPosition()) <= mainGUI.getEpsilon()) {
        return;
    }

    // Initial condition
    // Checking if the total bone length is greater than target position
    // If yes, continue
    // If no, return
    if(calcChainLength() < calcMagnitude(calculateVector(startPos, targetPos))) {
        mainGUI.warningMessage("Target out of range!");
        return;
    }

    // If the chainLength is greater than target position (Target within range)
    for(int i=0; i<maxIterations; i++) {
        backwardPass(targetPos);
        forwardPass(targetPos);
    }

    // if(runIK) {
    for(int i=0; i<bonesList.size() - 1; i++) {
        glm::vec3 pointO = boneOffsets[i];
        glm::vec3 pointA = bonesList[i+1]->getPosition();
        glm::vec3 pointB = boneOffsets[i+1];

        glm::vec3 vecA = calculateVector(pointO, pointA);
        glm::vec3 vecB = calculateVector(pointO, pointB);

        rotationAngle[i] = glm::clamp(calculateAngleBetween(vecA, vecB), -1.0f, 1.0f);
        rotationAxis[i] = calculateRotationAxis(vecA, vecB);

        // Check for NaN Angle
        if (std::isnan(rotationAngle[i])) {
            rotationAngle[i] = 0.0f;
        }

        // Check NaN for axis
        for (int j = 0; j < 3; ++j) {
            if (std::isnan(rotationAxis[i][j])) {
                rotationAxis[i][j] = (j == 1) ? 1.0f : 0.0f;
            }
        }

        // Setting initial matrix
        // Updating the rotation at each iteration
        if(bonesList[i]) {
            bonesList[i]->setInitialTransformMatrix();
            bonesList[i]->updateRotation(rotationAngle[i], rotationAxis[i], true);
            bonesList[i]->updateTransform();
        }

        else {
            mainGUI.errorMessage("Bone invalid!");
        }

        // Debugging
        printMat4GUI(bonesList[i]->getAccumulateTransformMatrix(), i+1);
        debuggingGUI(targetPos, vecA, vecB, rotationAxis[i], rotationAngle[i], i+1);
    }
}

// Scene Properties====================================================================================================
// We are replacing all the texture with realisitc textures to show-case PBR
void renderScene() {
    // Objects=========================================================================================================
    // To check environment mapping
    shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);

    /*
    General Structure :
    Root -> Set Initial -> Root Transforms -> (Child -> Child Transforms) -> Update Transform -> Root Render
    All root functions iterate over the child functions (Updating transforms)
    */

    // Root============================================================================================================
    // Root and End Effector are always present
    // Re-creating the list
    if(bone->calculateChainLength() - 1 != mainGUI.getChainLength()) {
        printf("Chain length : %i\n", bone->calculateChainLength());
        printf("Clearing children\n");
        bone->clearChildren();

        // Clearing out lists
        bonesList.clear();
        boneOffsets.clear();
        transformMatrices.clear();
        rotationAngle.clear();
        rotationAxis.clear();

        // Last Bone
        Model* lastBone = bone;
        bonesList.push_back(bone);
        boneOffsets.push_back(glm::vec3(0.0f));
        transformMatrices.push_back(bone->getAccumulateTransformMatrix());
        rotationAngle.push_back(0.0f);
        rotationAxis.push_back(glm::vec3(0.0f));

        for(int i=0; i<mainGUI.getChainLength() - 1; i++) {
            printf("Adding bone\n");
            Model* newChild = new Model();
            newChild->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/bone.obj");

            // Attaching new bone to previous parent
            newChild->attachParent(lastBone);
            newChild->setLocalTransforms(glm::vec3(0.0f, boneLength, 0.0f));
            newChild->setInitialTransformMatrix();
            lastBone->attachChild(newChild);

            // Adding the bone
            bonesList.push_back(newChild);
            boneOffsets.push_back(glm::vec3(0.0f, boneLength * (i + 1), 0.0f));
            transformMatrices.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, boneLength * (i + 1), 0.0f)));
            rotationAngle.push_back(0.0f);
            rotationAxis.push_back(glm::vec3(0.0f));

            // Making new bone the current parent
            lastBone = newChild;
        }


        // End effector
        monkey->attachParent(lastBone);
        monkey->setLocalTransforms(glm::vec3(0.0f, boneLength, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.25f));
        monkey->setInitialTransformMatrix();
        lastBone->attachChild(monkey);

        bonesList.push_back(monkey);
        boneOffsets.push_back(glm::vec3(0.0f, boneLength * mainGUI.getChainLength(), 0.0f));
        transformMatrices.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, boneLength * mainGUI.getChainLength(), 0.0f)));
        rotationAngle.push_back(0.0f);
        rotationAxis.push_back(glm::vec3(0.0f));
    }

    if(mainGUI.getIsIK()) {
        bone->updateTransform();
        bone->renderModel(uniformModel);
    }

    else {
        bone->setInitialTransformMatrix();
        bone->updateTransform();
        bone->renderModel(uniformModel);
    }


    // Tracker=========================================================================================================
    tracker->setInitialTransformMatrix();
    tracker->updateTranslation(glm::vec3(mainGUI.getTargetLocation()[0],
                                        mainGUI.getTargetLocation()[1],
                                        mainGUI.getTargetLocation()[2]));
    tracker->updateTransform();
    tracker->renderModel(uniformModel);

    // Anim Target
    animTarget->setInitialTransformMatrix();
    animTarget->updateTranslation(glm::vec3(mainGUI.getTargetEndLocation()[0],
                                        mainGUI.getTargetEndLocation()[1],
                                        mainGUI.getTargetEndLocation()[2]));
    animTarget->updateScale(glm::vec3(0.125f));
    animTarget->updateTransform();
    animTarget->renderModel(uniformModel);
}


// Animation
// Functions to animate the selected object (According to render pass)
// 2 Interpolation Type - 1. Linear, 2. Cubic Bezier
bool isAnimating = false;
float animationProgress = 0.0f;

void animateTarget() {
    // Debugging
    // printf("Animating\n");
    glm::vec3 currentPosition = moveFromAToB(mainGUI.getInterpolationType(),
                                             glm::vec3(mainGUI.getTargetLocation()[0], mainGUI.getTargetLocation()[1], mainGUI.getTargetLocation()[2]),
                                             glm::vec3(mainGUI.getTargetEndLocation()[0], mainGUI.getTargetEndLocation()[1], mainGUI.getTargetEndLocation()[2]),
                                             animationProgress);

    // Debugging
    // printVec3GUI(currentPosition, animationProgress);
    ImGui::Text("Animating!");

    // Type of interpolation
    if(mainGUI.getInterpolationType() == 1) {
        ImGui::Text("Linear Interpolation");
    }

    else if (mainGUI.getInterpolationType() == 2) {
        ImGui::Text("Bezier Cubic Interpolation");
    }

    else {
        ImGui::Text("!No such interpolation type!");
    }

    // Update target's position to currentPosition
    mainGUI.setTargetLocation(
        currentPosition.x,
        currentPosition.y,
        currentPosition.z
    );
}

// Render Pass - Renders all data in the scene=========================================================================
void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // General Pass
    // Includes the general elements of the program
    /*
    1. Background Color, Depth Pass
    2. Skybox
    3. GUI - New Frame
    4. Camera Controls
    5. Wireframe
    */
    // Always called
    generalElements(projectionMatrix, viewMatrix);

    // Setting Uniforms for a shader
    getUniforms(shaderList[0]);
    setUniforms(projectionMatrix, shaderList[0]);

    // Rendering the scene
    renderScene();

    // Movment of the target cube
    step = mainGUI.getTargetSpeed();

    // Delta Time
    GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
    deltaTime = now - lastTime;  // (now - lastTime)*1000 / SDL_GetPerformanceFrequency(); - ms
    lastTime = now;

    isAnimating = mainGUI.getIsAnimate();

    // Animating
    // If animating, don't use key inputs
    if(isAnimating) {
        // Update animation progress
        animationProgress += deltaTime * mainGUI.getAnimationRate();

        // Stop animation after one whole loop
        // Or press F to stop the animation
        if( animationProgress > 1.0f ||
            mainWindow.getKeys()[GLFW_KEY_F] ||
            areFloatArraysEqual(mainGUI.getTargetLocation(), mainGUI.getTargetEndLocation()) ) {
            // Debugging
            // printf("Ending Animation\n");

            // Resetting the animation progress
            animationProgress = 0.0f;

            // Preventing multiple clicks
            mainWindow.getKeys()[GLFW_KEY_F] = false;
            mainGUI.setIsAnimate(false);
        }

        if(mainGUI.getIsAnimate()) {
            animateTarget();
        }
    }

    // Handling keys to move target
    else {
        // Up
        if(mainWindow.getKeys()[GLFW_KEY_UP]) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0],
                mainGUI.getTargetLocation()[1] + step,
                mainGUI.getTargetLocation()[2]
            );
        }

        // Down
        else if(mainWindow.getKeys()[GLFW_KEY_DOWN]) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0],
                mainGUI.getTargetLocation()[1] - step,
                mainGUI.getTargetLocation()[2]
            );
        }

        // Left
        else if(mainWindow.getKeys()[GLFW_KEY_LEFT]) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0] - step,
                mainGUI.getTargetLocation()[1],
                mainGUI.getTargetLocation()[2]
            );
        }

        // Right
        else if(mainWindow.getKeys()[GLFW_KEY_RIGHT]) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0] + step,
                mainGUI.getTargetLocation()[1],
                mainGUI.getTargetLocation()[2]
            );
        }

        // Checks for the 2D flag, if it is diabled, only then take the inputs
        // Forward
        else if(mainWindow.getKeys()[GLFW_KEY_Z] && !mainGUI.getIsTwoDIK()) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0],
                mainGUI.getTargetLocation()[1],
                mainGUI.getTargetLocation()[2] + step
            );
        }

        // Backward
        else if(mainWindow.getKeys()[GLFW_KEY_X] && !mainGUI.getIsTwoDIK()) {
            mainGUI.setTargetLocation(
                mainGUI.getTargetLocation()[0],
                mainGUI.getTargetLocation()[1],
                mainGUI.getTargetLocation()[2] - step
            );
        }
    }

    // Running IK
    if(mainGUI.getIsIK()) {
        tolerance = mainGUI.getTolerance();
        maxIterations = mainGUI.getMaxIterations();

        // Calculating Inverse Kinematics
        calculateIK();
    }

    // Drawing the UI
    setShadingModeName(mainGUI, shadingModel, shadingMode);
    mainGUI.render(shadingMode);
}

int main() {
    mainWindow = Window(1366, 768);
    mainWindow.initialize();

    // Debugging
    // printf("Current Path: %s\n", vertexShader);

    createObjects();
    createShaders();

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);

    // Setting up objects==============================================================================================
    prepareObjects();

    // Creating the lights=============================================================================================
    createLights();

    // Projection Matrix===============================================================================================
    glm::mat4 projection = camera.calculatePerspectiveProjectionMatrix(mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

    // ImGUI===========================================================================================================
    mainGUI.initialize(mainWindow.getWindow());

    // Main Loop - Running till the window is open=====================================================================
    while(!mainWindow.getShouldClose()) {
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
