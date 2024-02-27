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
std::vector<Shader*> shaderList;

// Camera
Camera camera;

// Switching between GLFW and ImGUI
bool cursorDisabled = true;

// Textures
// TODO : Vectors don't work with these textures? Weird
Texture whiteTexture;
Texture ckbSimple;
Texture ckbNearestMIP;
Texture ckbLineartMIPNearest;
Texture ckbLinearMIPLinear;

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

// We have 3 rendering modes at the moment
// 1. Normal
// 2. Toed-in Asymmetric
// 3. Asymmetric Frustum
int renderingMode = 0;

// Models
Model* monkey;
Model* sphere;
Model* teapot;
Model* cube;
Model* crate;
Model* ground;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Creating Objects====================================================================================================
// Contains making manual objects
const float planeSize = 5.0f;
const float planeUV = 2.5f;

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
    // Shader 1
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

    Shader* shader1 = new Shader();
    shader1->createFromFiles(vertexShader, fragmentShader);

    shaderList.push_back(shader1);

    // Shader 2
    // Vertex Shader
    vertexShaderPath = (currentSourceDir / "Shaders/Simple.vert").string();
    vertexShaderFormatted = removeBackslash(vertexShaderPath.c_str());
    vertexShader = vertexShaderFormatted.c_str();

    // Fragment Shader
    fragmentShaderPath = (currentSourceDir / "Shaders/Simple.frag").string();
    fragmentShaderFormatted = removeBackslash(fragmentShaderPath.c_str());
    fragmentShader = fragmentShaderFormatted.c_str();

    Shader* shader2 = new Shader();
    shader2->createFromFiles(vertexShader, fragmentShader);

    shaderList.push_back(shader2);
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

    // Choices - Parameter : Give the kind of loading you want to do for the particular texture
    // 1. No Interpolation and No MIP Maps
    // 2. No Interpolation and MIP Maps
    // 3. Interpolation and MIP Maps Near
    // 4. Intepolation and MIP Maps Interpolation
    ckbSimple = Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/checkboard.png");
    ckbSimple.loadTexture(1);

    ckbNearestMIP = Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/checkboard.png");
    ckbNearestMIP.loadTexture(2);

    ckbLineartMIPNearest = Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/checkboard.png");
    ckbLineartMIPNearest.loadTexture(3);

    ckbLinearMIPLinear = Texture("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Textures/checkboard.png");
    ckbLinearMIPLinear.loadTexture(4);

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
    monkey = new Model();
    monkey->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/monkey.obj");

    sphere = new Model();
    sphere->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/sphere.obj");

    teapot = new Model();
    teapot->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/teapot.obj");

    cube = new Model();
    cube->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/cube.obj");

    crate = new Model();
    crate->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/crate.obj");

    ground = new Model();
    ground->loadModel("D:/Programs/C++/Rendering/OpenGL/src/Rendering/Models/floor.obj");
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
    if(mainGUI.getIsSkyBox() && mainGUI.getDrawSkyBox()) {
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

            // Turn off Anaglyph for Orthographic view
            mainGUI.setIsAnaglyph(false);
            mainGUI.setCameraIsOrthographic(true);
        }

        else {
            mainGUI.setCameraIsOrthographic(false);
            mainGUI.setCameraIsPerspective(true);
        }

        mainWindow.getKeys()[GLFW_KEY_KP_5] = false;
    }

    // Setting Rendering Mode - 'M'
    if(mainWindow.getKeys()[GLFW_KEY_M]) {
        renderingMode = (renderingMode + 1) % 3;

        // Debugging
        // printf("%i", renderingMode);

        // Default Rendering
        if(renderingMode == 0) {
            mainGUI.setIsAnaglyph(false);
            mainGUI.setIsToedInRendering(false);
            mainGUI.setIsAsymmetricFrustumRendering(false);
        }

        // Anaglyph - Toed-In Rendering
        else if(renderingMode == 1) {
            mainGUI.setIsAnaglyph(true);
            mainGUI.setIsToedInRendering(true);
            mainGUI.setIsAsymmetricFrustumRendering(false);
        }

        // Anaglyph - Asymmetric Frustum
        else {
            mainGUI.setIsAnaglyph(true);
            mainGUI.setIsToedInRendering(false);
            mainGUI.setIsAsymmetricFrustumRendering(true);
        }

        // Stopping multiple inputs from the same key
        mainWindow.getKeys()[GLFW_KEY_M] = false;
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

void getUniforms(Shader* shader) {
    // Binding the program
    shader->useShader();

    uniformModel = shader->getModelLocation();
    uniformProjection = shader->getProjectionLocation();
    uniformView = shader->getViewLocation();

    // Specular Light
    uniformEyePosition = shader->getEyePositionLocation();
    uniformSpecularIntensity = shader->getSpecularIntensityLocation();
    uniformShininess = shader->getShininessLocation();
    uniformMetalness = shader->getMetalnessLocation();

    // Getting Shading Mode
    uniformshadingModel = shader->getShadingModelLocation();

    /*
    Getting UI Elements
    */
    // Object Properties
    uniformIsShaded = shader->getIsShadedLocation();
    uniformIsWireframe = shader->getIsWireframeLocation();
    uniformWireframeColor = shader->getWireframeColourLocation();
    uniformObjectColor = shader->getObjectColorLocation();
    uniformEnvMapping = shader->getEnvMappingLocation();
    uniformSkybox = shader->getSkyboxLocation();
    uniformBackgroundColor = shader->getBackgroundColourLocation();

    // Material Preview - Maps
    uniformMaterialPreview = shader->getMaterialPreviewLocation();
    uniformSpecularPreview = shader->getSpecularPreviewLocation();
    uniformNormalPreview = shader->getNormalPreviewLocation();

    // Material Properties
    uniformIsReflection = shader->getIsReflectionLocation();
    uniformIsRefraction = shader->getIsRefractionLocation();
    uniformIOR = shader->getIORLocation();
    uniformFresnelReflectance = shader->getFresnelReflectance();
    uniformDispersion = shader->getDispersionLocation();
    uniformNormalStrength = shader->getNormalStrengthLocation();
    uniformSpecularStrength = shader->getSpecularStrengthLocation();

    // Texture
    uniformDiffuseTexture = shader->getMainTextureLocation();
    uniformSpecularTexture = shader->getSpecularTextureLocation();
    uniformAmbientOcclusionTexture = shader->getAmbientOcclusionTextureLocation();
    uniformNormalTexture = shader->getNormalTextureLocation();
}

void setUniforms(glm::mat4 projectionMatrix, glm::mat4 viewMatrix, Shader* shader) {
    // Binding the texture to correct texture units
    shader->setTexture(uniformDiffuseTexture, 0);
    shader->setTexture(uniformSpecularTexture, 1);
    shader->setTexture(uniformNormalTexture, 2);

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

    shader->setDirectionalLight(&mainLight);

    // Currently disabling the Point and Spot lights based off a boolean
    // Point Lights
    if(mainGUI.getIsPointLights()) {
        shader->setPointLight(pointLights, pointLightCount);
    }

    else {
        shader->setPointLight(pointLights, 0);
    }

    // Spot Lights
    if(mainGUI.getIsSpotLights()) {
        shader->setSpotLight(spotLights, spotLightCount);
    }

    else {
        shader->setSpotLight(spotLights, 0);
    }

    // Binding to uniforms in the shader
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
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


// Scene Properties====================================================================================================
// Global parameter for rotating the objects
float rotationAngle = 0.0f;
float step = 0.001f;
float size = 12.0f;

// Details about objects that need to be rendered
void renderScene() {
    // Objects=========================================================================================================
    // Grounds
    glm::mat4 base = glm::mat4(1.0f);

    // Displacing the ground
    base = glm::translate(base, glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));

    ckbSimple.useTexture(0);
    ckbSimple.useTexture(1);
    ckbSimple.useTexture(2);

    extraShinyMat.useMaterial(uniformSpecularIntensity, uniformShininess, uniformMetalness);
    meshList[0]->renderMesh();
}

// Calculate Toed-in or Asymmetric Frustum Anaglyph
void calculateAnaglyph(glm::mat4& projectionMatrix, glm::mat4& viewMatrix) {
    /* Toed-In Method of Anaglyphical rendering
    In this method, we change the view matrix to point towards a custom target and offset the
    Render passes based off of the Inter Ocular Distance (IOD) and Convergence Distance (CD)
    * Only works for perspective renders
    */
    //Red pass - Left Eye
    glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    if(mainGUI.getIsToedInRendering()) {
        // Modifying the view matrix for left eye here
        viewMatrix = camera.calculateViewMatrix(true, mainGUI.getInterOcularDistance(), mainGUI.getCovergenceDistance());
    }

    else if(mainGUI.getIsAsymmetricFrustumRendering()) {
        // Modifying the projection matrix for left eye here
        projectionMatrix = camera.calculateAsymmetricFrustum(true,
                                                            mainGUI.getInterOcularDistance(),
                                                            mainGUI.getCovergenceDistance(),
                                                            mainWindow.getBufferWidth(),
                                                            mainWindow.getBufferHeight());
    }

    // Setting Uniforms for a shader
    getUniforms(shaderList[0]);
    setUniforms(projectionMatrix, viewMatrix, shaderList[0]);

    // Rendering the scene
    renderScene();

    // Cyan pass - Right Eye
    glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_DEPTH_BUFFER_BIT);

    if(mainGUI.getIsToedInRendering()) {
        // Modifying the view matrix for right eye here
        viewMatrix = camera.calculateViewMatrix(false, mainGUI.getInterOcularDistance(), mainGUI.getCovergenceDistance());
    }

    else if(mainGUI.getIsAsymmetricFrustumRendering()) {
        // Modifying the projection matrix for right eye here
        projectionMatrix = camera.calculateAsymmetricFrustum(false,
                                                            mainGUI.getInterOcularDistance(),
                                                            mainGUI.getCovergenceDistance(),
                                                            mainWindow.getBufferWidth(),
                                                            mainWindow.getBufferHeight());
    }

    // Setting Uniforms for a shader
    getUniforms(shaderList[0]);
    setUniforms(projectionMatrix, viewMatrix, shaderList[0]);

    // Rendering the scene
    renderScene();

    // Resetting the color pass to render all colors
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
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

    // Anaglyph Rendering
    if(mainGUI.getIsAnaglyph()) {
        calculateAnaglyph(projectionMatrix, viewMatrix);
    }

    else {
        // Setting Uniforms for a shader
        getUniforms(shaderList[0]);
        setUniforms(projectionMatrix, viewMatrix, shaderList[0]);

        // Rendering the scene
        renderScene();
    }

    // Drawing the UI
    setShadingModeName(mainGUI, shadingModel, shadingMode);
    mainGUI.render(shadingMode);
}


// Main Function
int main()
{
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
