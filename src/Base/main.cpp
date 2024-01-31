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

// Custom Models
#include "Model.h"

// Skybox
#include "Skybox.h"

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Setting uniforms
GLuint  uniformProjection = 0, uniformModel = 0, uniformView = 0,
        uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0,
        uniformDirectionalLightTransform = 0,
        uniformOmniLightPos = 0, uniformFarPlane = 0;

// Our main window
Window mainWindow;

// Creating a vector of the meshes and shaders
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

// Camera
Camera camera;

// Textures
Texture brickTexture;
Texture dirtTexture;

// Lights - 1 Directional, Multiple Point
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

// Materials===========================================================================================================
Material shinyMat;
Material roughMat;
Material normalMat;
Material extraRoughMat;

// Models - Using ASSIMP
Model cityMain;
Model rocks;
Model trees;
Model someBuildings;
Model walls;
Model fillersFinal;

Model waterWalls;
Model waterWay;
Model roads;
Model pavement;
Model bridge;

Model fillers2;
Model fillers;
Model zombie;

// Cube Models=========================================================================================================
Model cube1;
Model cube2;
Model cube3;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = "D:/Programs/C++/Yumi/src/Base/Shaders/shader.vert";

// Fragment Shader
static const char* fragmentShader = "D:/Programs/C++/Yumi/src/Base/Shaders/shader.frag";

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
        -100.0f, 0.0f, -100.0f,   0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        100.0f, 0.0f, -100.0f,    100.0f, 0.0f,    0.0f, -1.0f, 0.0f,
        -100.0f, 0.0f, 100.0f,    0.0f, 100.0f,    0.0f, -1.0f, 0.0f,
        100.0f, 0.0f, 100.0f,     100.0f, 100.0f,   0.0f, -1.0f, 0.0f
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

    directionalShadowShader = Shader();
    directionalShadowShader.createFromFiles("D:/Programs/C++/Yumi/src/Base/Shaders/directionalShadowMap.vert",
                                            "D:/Programs/C++/Yumi/src/Base/Shaders/directionalShadowMap.frag");

    omniShadowShader = Shader();
    omniShadowShader.createFromFiles("D:/Programs/C++/Yumi/src/Base/Shaders/omniShadowMap.vert",
                                     "D:/Programs/C++/Yumi/src/Base/Shaders/omniShadowMap.frag",
                                     "D:/Programs/C++/Yumi/src/Base/Shaders/omniShadowMap.geom");
}

float translateVal = 0.0f;
float val = 0.01f;

void renderScene() {
    // Happens in a reverse order
    // Translate
    glm::mat4 model = glm::mat4(1.0f);

    // // Trial Triangles=================================================================================================
    // // Object 1
    // model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
    // glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // // Texturing the Mesh
    // brickTexture.useTexture();
    // shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    // meshList[0]->renderMesh();

    // // Clearing out the properties
    // // Object 2
    // model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, 1.0f, -5.0f));
    // glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // // Texturing the Mesh
    // dirtTexture.useTexture();
    // roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    // meshList[1]->renderMesh();

    // Object 3
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    // Texturing the Mesh
    brickTexture.useTexture();
    extraRoughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->renderMesh();

    // Cubes===========================================================================================================
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, -1.0f, 3.0f));
    model = glm::scale(model, glm::vec3(0.125f / 4, 0.125f / 4, 0.125f / 4));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    extraRoughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    zombie.renderModel();

    translateVal += val;

    if(translateVal > 15.f) {
        val *= -1;
    }

    if(translateVal < -15.f) {
        val *= -1;
    }

    float translateY = 1.0f;
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(translateVal, translateY, 0.0f));
    model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    shinyMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    cube1.renderModel();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(3.0f, translateY, 4.0f));
    // model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    normalMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    cube2.renderModel();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, translateY, -4.0f));
    // model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    roughMat.useMaterial(uniformSpecularIntensity, uniformShininess);
    cube3.renderModel();
}

void directionalShadowMapPass(DirectionalLight* light) {
    directionalShadowShader.useShader();

    glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    light->getShadowMap()->write();

    // Clear existing depth buffer information
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.getModelLocation();

    directionalShadowShader.setDirectionalLightTransform(&light->calculateLightTransform());

    directionalShadowShader.validate();

    renderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void omniShadowMapPass(PointLight* light) {
    omniShadowShader.useShader();

    glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    light->getShadowMap()->write();

    // Clear existing depth buffer information
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = omniShadowShader.getModelLocation();

    uniformOmniLightPos = omniShadowShader.getOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.getFarPlaneLocation();

    glUniform3f(uniformOmniLightPos, light->getPosition().x, light->getPosition().y, light->getPosition().z);
    glUniform1f(uniformFarPlane, light->getFarPlane());

    omniShadowShader.setLightMatrices(light->calculateLightTransform());

    omniShadowShader.validate();

    renderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
    // Setting initial GLFW Window
    glViewport(0, 0, 1366, 768);

    // Clear window
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Drawing the Skybox before everything else
    skybox.drawSkybox(viewMatrix, projectionMatrix);

    // Making sure we are using the right shader
    // Binding the shader to program
    shaderList[0].useShader();

    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();

    // Specular Light
    uniformEyePosition = shaderList[0].getEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
                                    camera.getCameraPosition().y,
                                    camera.getCameraPosition().z);

    // Lights
    shaderList[0].setDirectionalLight(&mainLight);
    shaderList[0].setPointLight(pointLights, pointLightCount, 3, 0);
    shaderList[0].setSpotLight(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
    shaderList[0].setDirectionalLightTransform(&mainLight.calculateLightTransform());

    mainLight.getShadowMap()->read(GL_TEXTURE2);

    // Setting the maps to proper texture units
    shaderList[0].setTexture(1);
    shaderList[0].setDirectionalShadowMap(2);

    // Setting Torch Control
    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= 0.369f;

    // Getting torch control
    spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

    shaderList[0].validate();

    renderScene();
}

int main() {
    mainWindow = Window(1366, 768);
    mainWindow.initialize();

    createObjects();
    createShaders();

    // Initializing Camera - Y is UP
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 3.0f, 0.125f );

    // Setting up Textures
    brickTexture = Texture("D:/Programs/C++/Yumi/src/Base/Textures/brickHi.png");
    brickTexture.loadTextureA();
    dirtTexture = Texture("D:/Programs/C++/Yumi/src/Base/Textures/brick.png");
    dirtTexture.loadTextureA();

    // Setting up Materials
    // Make the second parameter (Shine) to be powers of 2
    shinyMat = Material(10.0f, 1024);
    extraRoughMat = Material(0.5f, 4);
    roughMat = Material(2.5f, 4);
    normalMat = Material(5.0f, 64);

    // Cubes===========================================================================================================
    cube1 = Model();
    cube1.loadModel("D:/Programs/C++/Yumi/src/Base/Models/cube.obj");

    cube2 = Model();
    cube2.loadModel("D:/Programs/C++/Yumi/src/Base/Models/cube1.obj");

    cube3 = Model();
    cube3.loadModel("D:/Programs/C++/Yumi/src/Base/Models/cube2.obj");

    // zombie = Model();
    zombie.loadModel("D:/Programs/C++/Yumi/src/Base/Models/Zombie Walk.dae");

    // Setting up lights
    // Since we will be using cube map, we are using square values for texture
    int shadowMapResScale = 8;
    mainLight = DirectionalLight( 1024 * shadowMapResScale, 1024 * shadowMapResScale,
								  1.0f, 1.0f, 1.0f,
								  .1f, 0.5f,
								  0.0f, -15.0f, -5.0f );
    // Point Lights
    pointLights[0] = PointLight( 1024, 1024,
                                 0.01f, 1000.0f,
                                 0.0f, 0.0f, 1.0f,
								 1.0f, 1.5f,
								 0.0f, 2.0f, 0.0f,
								 0.3f, 0.2f, 0.1f );
    pointLightCount++;

    pointLights[1] = PointLight( 1024, 1024,
                                 0.01f, 1000.0f,
                                 0.0f, 1.0f, 0.0f,
								 1.0f, 1.5f,
								 -4.0f, 3.0f, 0.0f,
								 0.3f, 0.2f, 0.1f );
    pointLightCount++;

    pointLights[2] = PointLight( 1024, 1024,
                                 0.01f, 1000.0f,
                                 1.0f, 0.0f, 0.0f,
								 1.0f, 1.5f,
								 -4.0f, 2.0f, -4.0f,
								 0.3f, 0.2f, 0.1f );
    pointLightCount++;

    // Spot Lights
    // This is our torch
    spotLights[0] = SpotLight(  1024, 1024,
                                0.01f, 1000.0f,
                                1.0f, 1.0f, 1.0f,
                                0.5f, 2.0f,
                                0.0f, 0.0f, 0.0f,
                                0.0f, -1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f );
    spotLightCount++;

    spotLights[1] = SpotLight(  1024, 1024,
                                0.01f, 1000.0f,
                                1.0f, 1.0f, 1.0f,
                                0.75f, 1.0f,
                                3.0f, -1.5f, -3.0f,
                                0.0f, -1.0f, -45.0f,
                                1.0f, 0.0f, 0.0f,
                                20.0f );

	spotLightCount++;

    // Skybox
    std::vector<std::string> skyboxFaces;
    // Pushing the textures in a particular order
    // Right, Left
    // Up, Down
    // Back, Front
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_rt.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_lf.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_up.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_dn.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_bk.tga");
    skyboxFaces.push_back("D:/Programs/C++/Yumi/src/Base/Textures/Skybox/cloudtop_ft.tga");

    skybox = Skybox(skyboxFaces);

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), GLfloat(mainWindow.getBufferWidht())/GLfloat(mainWindow.getBufferHeight()), 0.1f, 100.0f);

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

        // Toggling Spot Light on pressing L
        if(mainWindow.getKeys()[GLFW_KEY_L]) {
            // printf("Pressed L!");

            spotLights[0].toggle();
            mainWindow.getKeys()[GLFW_KEY_L] = false;
        }

        // Renders the pass to frame buffer which stores it in a texture
        directionalShadowMapPass(&mainLight);

        // Omni Shadow Passes
        for(size_t i = 0; i < pointLightCount; i++) {
            omniShadowMapPass(&pointLights[i]);
        }

        for(size_t i = 0; i < spotLightCount; i++) {
            omniShadowMapPass(&spotLights[i]);
        }

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
