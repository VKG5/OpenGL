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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>

// Custom Libraries
#include "Mesh.h"
#include "Bones.h"
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

// Movement
#include "Movement.h"

// Defining all necessary variables
// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

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
std::vector<Bones*> boneList;

// Camera
Camera camera;

// Movement
Movement move;

// Switching between GLFW (True) and ImGUI (False)
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
Model boneModel;
Model tracker;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Rendering/OpenGL/src/Animation/Shaders/finalShader.vert";
static const char* boneVertexShader = "D:/Programs/C++/Rendering/OpenGL/src/Animation/Shaders/boneShader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Rendering/OpenGL/src/Animation/Shaders/finalShader.frag";
static const char* boneFragmentShader = "D:/Programs/C++/Rendering/OpenGL/src/Animation/Shaders/boneShader.frag";

// Creating Objects====================================================================================================
// Comtains making manual objects
void createObjects() {
    // // Objects
    // // Index Array
    // // Index of the vertices that are being drawn
    // unsigned int indicesFloor[] = {
    //     0, 1, 2,
    //     1, 2, 3
    // };

    // GLfloat verticesFloor[] = {
    //     // x,y,z                Ux, Uv          Nx, Ny, Nz
    //     -10.0f, 0.0f, -10.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
    //     10.0f, 0.0f, -10.0f,    10.0f, 0.0f,    0.0f, -1.0f, 0.0f,
    //     -10.0f, 0.0f, 10.0f,    0.0f, 10.0f,    0.0f, -1.0f, 0.0f,
    //     10.0f, 0.0f, 10.0f,     10.0f, 10.0f,   0.0f, -1.0f, 0.0f
    // };

    // // Object 3
    // Mesh* floor = new Mesh();
    // floor->createMesh(verticesFloor, indicesFloor, 32, 6);

    // // Adding to our meshlist
    // meshList.push_back(floor);
}


// Preparing Objects===================================================================================================
// Contains preparing base shaders
void createShaders() {
    // Shader 1
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
    // Back, Front
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/right.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/left.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/top.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/bottom.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/front.jpg");
    skyboxFaces.push_back("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/Skybox/skybox/back.jpg");

    skybox = Skybox(skyboxFaces);
}

// Contains textures, objects and materials
void prepareObjects() {
    // Setting up Textures=============================================================================================
    whiteTexture = Texture("D:/Programs/C++/Rendering/OpenGL/src/Animation/Textures/white.jpg");
    whiteTexture.loadTexture();

    // Setting up Materials============================================================================================
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(0.869f, 256, 0.25f);
    roughMat = Material(0.5f, 32, 0.8f);
    extraRoughMat = Material(0.125f, 2, 1.0f);
    extraShinyMat = Material(1.0f, 1024, 0.0125f);

    // Loading Models==================================================================================================
    boneModel = Model();
    boneModel.loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/bone.obj");

    tracker = Model();
    tracker.loadModel("D:/Programs/C++/Rendering/OpenGL/src/Animation/Models/tracker.obj");
}

// General settings for all projects
void generalElements(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
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
        skybox.drawSkybox(viewMatrix, projectionMatrix);
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

            // Set camera position using GUI
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

// Getting all the uniform values from the current shader
void getUniforms(Shader& shader) {
    uniformModel = shader.getModelLocation();

    // Debugging
    // printf("Shader 0 : (%i)\n", shader.getModelLocation());

    uniformProjection = shader.getProjectionLocation();
    uniformView = shader.getViewLocation();

    // Specular Light
    uniformEyePosition = shader.getEyePositionLocation();
    uniformSpecularIntensity = shader.getSpecularIntensityLocation();
    uniformShininess = shader.getShininessLocation();
    uniformRoughness = shader.getMetalnessLocation();

    // Getting Shading Mode
    uniformshadingModel = shader.getShadingModelLocation();

    /*
    Getting UI Elements
    1. isWireframe - Enable/Disable Wireframe
    2. isShaded - Enbable/Disable Textures
    3. wireframeColor - Sets the color of the wireframe
    4. objectColor - Sets the color of the objects in the scene
    5. materialPreview - Disables all lighting to view the textures only
    */
    uniformMaterialPreview = shader.getMaterialPreviewLocation();
    uniformIsShaded = shader.getIsShadedLocation();
    uniformIsWireframe = shader.getIsWireframeLocation();
    uniformWireframeColor = shader.getWireframeColourLocation();
    uniformObjectColor = shader.getObjectColorLocation();
    uniformEnvMapping = shader.getEnvMappingLocation();
    uniformSkybox = shader.getSkyboxLocation();
    uniformBackgroundColor = shader.getBackgroundColourLocation();

    // Getting the texture locations
    uniformMainTexture = shader.getMainTextureLocation();
}

// Setting the uniform values from the current shader
void setUniforms(glm::mat4 projectionMatrix, Shader& shader) {
    // Binding the texture to correct texture units
    shader.setTexture(uniformMainTexture, 0);

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
}

// Setting correct Shading Mode name in the UI
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


// IK - CCD/FABRIK========================================================================================================
float step = 0.005f;
// Bone Offset is the initial bone's offset, hence the START of the IK Chain
float boneLength = 1.0f;

// Tolerance for increasing target reach range
float tolerance = 0.01f;

// Max iterations
int maxIterations = 64;

glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 0.0f);

// These are the bone offsets (By which we displace the bones)
std::vector<glm::vec3> boneOffsets;
std::vector<glm::mat4> boneTransformations;

// Helper functions
// GUI for Debugging
void debuggingGUI( glm::vec3 targetPos,
                   glm::vec3 vecB, glm::vec3 vecA,
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

// Return absolute difference between 2 vectors : A - B
float calcDifferenceMagnitude(const glm::vec3& vecA, const glm::vec3& vecB) {
    return glm::length(glm::abs(vecA - vecB));
}

// Return the total length of the chain
float calcChainLength() {
    return (mainGUI.getChainLength() * boneLength) + tolerance;
}

// Return the length/magnitude of a vector
float calcMagnitude(const glm::vec3& vecA) {
    return glm::length(vecA);
}

// Calculate angle betweem two vector relative to an origin
float calculateAngleBetween(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3& origin = glm::vec3(0.0f)) {
    return glm::acos(glm::dot(glm::normalize(vecA - vecB), glm::normalize(vecB - origin)));
}

// Calculate axis of rotation
glm::vec3 calculateRotationAxis(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3& origin = glm::vec3(0.0f)) {
    return glm::cross(glm::normalize(vecB - origin), glm::normalize(vecA - vecB));
}

// Invert the Y-axis
glm::vec3 invertYAxis(const glm::vec3& originalVec) {
    return glm::vec3(originalVec.x, -originalVec.y, originalVec.z);
}

// Print a glm::vec3
void printVec3(const glm::vec3& vector, const int& index) {
    ImGui::Text("Index : %i Vector components: (%.2f, %.2f, %.2f)\n", index, vector.x, vector.y, vector.z);
}

// Print a glm::mat4 in terminal
void printMat4(const glm::mat4& mat, const int& index) {
    printf("Index : %i Matrix:\n", index);
    for (int row = 0; row < 4; row++) {
        // Indent for better readability
        printf("   ");
        for (int col = 0; col < 4; col++) {
            // Print element and a trailing space only after last column
            printf("%.2f%s", mat[col][row], (col == 3) ? "" : " ");
        }
        printf("\n");
    }
}

// Print a glm::mat4 in GUI
void printMat4GUI(const glm::mat4& mat, const int& index) {
    ImGui::Text("Index : %i Matrix:\n", index);
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            // Print element and a trailing space only after last column
            ImGui::Text("%.2f%s", mat[col][row], (col == 3) ? "" : " ");
            ImGui::SameLine();
        }
        ImGui::Text("");
    }
}

void backwardPass(const glm::vec3& targetPos) {
    int lastBoneIndex = boneTransformations.size() - 1;

    // Making a temporary copy
    std::vector<glm::vec3> boneOffsetsLocal(boneOffsets);
    std::vector<glm::mat4> boneTransformationsLocal(boneTransformations);

    boneOffsetsLocal[lastBoneIndex] = targetPos;

    // Iterate backwards in the chain
    for(int i = lastBoneIndex; i > 0; i--) {
        // Apply previous bone transformation to target position
        glm::vec4 targetLocal = boneTransformationsLocal[i] * glm::vec4(boneOffsetsLocal[lastBoneIndex], 1.0);

        // Extract the translational component and use it
        glm::vec3 updatedTargetLocal = glm::vec3(targetLocal.x, targetLocal.y, targetLocal.z);

        // Perform calculations using updatedTargetLocal
        glm::vec3 pointAPrime = updatedTargetLocal;

        glm::vec3 pointB = boneOffsetsLocal[i-1];
        glm::vec3 pointA = boneOffsets[i];

        float angle = calculateAngleBetween(pointAPrime, pointB, pointA);
        glm::vec3 rotationAxis = calculateRotationAxis(pointAPrime, pointB, pointA);

        // Debugging UI
        debuggingGUI(pointAPrime, pointB, pointA, rotationAxis, angle, i);

        // Create a transformation matrix with rotation and translation
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), pointB);

        // Update the bone transformation with rotation and translation
        boneTransformationsLocal[i-1] = translationMatrix * rotationMatrix;

        // Debugging
        printVec3(boneOffsetsLocal[i-1], i-1);
        printMat4GUI(boneTransformationsLocal[i-1], i-1);
    }

    // Updating the original vector
    // boneOffsets.assign(boneOffsetsLocal.begin(), boneOffsetsLocal.end());
    boneTransformations = boneTransformationsLocal;
}

void forwardPass(const glm::vec3& targetPos) {
    int lastBoneIndex = boneTransformations.size() - 1;

    // Set the end effector position to the target position
    boneOffsets[lastBoneIndex] = targetPos;

    // Iterate forward in the chain
    for (int i = lastBoneIndex; i > 0; i--) {
        // Calculate the vector from the current bone to the next bone
        glm::vec3 boneVector = boneOffsets[i] - boneOffsets[i - 1];

        // Normalize the vector and scale it to the original length
        boneVector = glm::normalize(boneVector) * glm::length(boneOffsets[i - 1] - boneOffsets[i]);

        // Update the position of the previous bone
        boneOffsets[i - 1] = boneOffsets[i] - boneVector;
    }

    // Debugging
    for (int i = 0; i <= lastBoneIndex; i++) {
        printVec3(boneOffsets[i], i);
    }
}

void calculateIK() {
    glm::vec3 targetPos = glm::vec3(mainGUI.getTargetLocation()[0],
                                    mainGUI.getTargetLocation()[1],
                                    mainGUI.getTargetLocation()[2]);

    // Initial condition
    // Checking if the total bone length is greater than target position
    // If yes, continue
    // If no, return
    if(calcChainLength() < calcDifferenceMagnitude(targetPos, startPosition)) {
        mainGUI.warningMessage("Target out of range!");
        return;
    }

    // If the chainLength is greater than target position (Target within range)
    for(int i=0; i<maxIterations; i++) {
        backwardPass(targetPos);
        forwardPass(targetPos);
    }
}

bool flag = true;

// Rendering Objects===================================================================================================
// This function is responsible for loading, texturing and rendering the individual objects in the scene
void renderScene() {
    // Objects
    // Root Bone
    glm::mat4 base = glm::mat4(1.0f);

    // Debugging
    // printf("Chain Length : %i\n", mainGUI.getChainLength());

    // Initializing the vectors/refresh
    if(boneTransformations.size() != mainGUI.getChainLength() + 1 || boneOffsets.size() != mainGUI.getChainLength() + 1) {
        printf("Resetting transforms\n");
        boneTransformations.clear();
        boneOffsets.clear();

        // Adding 1 extra value for effector point
        // We do not render this point
        for(int i=0; i<mainGUI.getChainLength() + 1; i++) {
            boneOffsets.push_back(glm::vec3(0.0f, boneLength, 0.0f));
            base = glm::translate(base, boneOffsets[i]);

            boneTransformations.push_back(base);

            // Debugging
            printMat4(base, i);

            flag = true;
        }
    }

    // Rendering multiple bones
    for(int i=0; i<mainGUI.getChainLength(); i++) {
        if(flag) {
            printf("Bone %i\n", i);
            printMat4(boneTransformations[i], i);
        }

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(boneTransformations[i]));
        boneModel.renderModel();
    }

    flag = false;
    // Tracker
    base = glm::mat4(1.0f);
    base = glm::translate(base, glm::vec3( mainGUI.getTargetLocation()[0],
                                           mainGUI.getTargetLocation()[1],
                                           mainGUI.getTargetLocation()[2]) );

    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
    tracker.renderModel();
}

// Render Pass - Renders all data in the scene=========================================================================
// Handle Inputs and Bindings
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

    // Binding the program - Shader 0
    shaderList[0].useShader();

    // Getting & Setting uniforms for the passed shader
    getUniforms(shaderList[0]);
    setUniforms(projectionMatrix, shaderList[0]);

    // Movment of the target cube
    step = mainGUI.getTargetSpeed();

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

    // Forward
    else if(mainWindow.getKeys()[GLFW_KEY_Z]) {
        mainGUI.setTargetLocation(
            mainGUI.getTargetLocation()[0],
            mainGUI.getTargetLocation()[1],
            mainGUI.getTargetLocation()[2] + step
        );
    }

    // Backward
    else if(mainWindow.getKeys()[GLFW_KEY_X]) {
        mainGUI.setTargetLocation(
            mainGUI.getTargetLocation()[0],
            mainGUI.getTargetLocation()[1],
            mainGUI.getTargetLocation()[2] - step
        );
    }

    if(mainGUI.getIsIK()) {
        tolerance = mainGUI.getTolerance();
        maxIterations = mainGUI.getMaxIterations();

        // Calculating Inverse Kinematics
        calculateIK();
    }

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
    // And Movement class
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.125f, 3.0f);
    move = Movement();

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
