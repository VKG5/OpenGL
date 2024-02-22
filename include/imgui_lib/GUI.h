#pragma once

// ImGui libraries
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad.h>
#include <GLFW/glfw3.h>

#include <string>

class GUI
{
private:
    ImGuiIO io;

    // Element variables
    // Wireframe
    bool wireframe = false;
    ImVec4 wireframeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Texture/Object Color
    // Default color is Blender's default color
    bool shaded = true;
    ImVec4 objectColor = ImVec4(151.0f / 255.0f, 153.0f / 255.0f, 153.0f / 255.0f, 1.0f);

    // Background Color
    ImVec4 bgColor = ImVec4(61.0f / 255.0f, 61.0f / 255.0f, 61.0f / 255.0f, 1.0f);

    // Material Preview Mode
    bool materialPreview = false;
    bool specularPreview = false;
    bool normalPreview = false;

    // Light settings
    // Directional Light
    float directionalLightColor[3] = {1.0f, 1.0f, 1.0f};
    float directionalLightDirection[3] = {2.0f, -1.0f, -2.0f};
    float directionalLightAmbient = 0.350f;
    float directionalLightDiffuse = 0.500f;

    // Skybox
    int skyboxIndex = 1;

    // Camera
    bool cameraIsPerspective = true;
    bool cameraIsOrthographic = false;
    float cameraFOV = 45.0f;
    float cameraOrthoScale = 7.135f;
    float cameraNearClipping = 0.1f;
    float cameraFarClipping = 100.0f;
    float cameraSpeed = 5.0f;
    float cameraPos[3] = {0.0f, 1.75f, 4.0f};

    // Point Lights
    bool isPointLights = false;

    // Spot Lights
    bool isSpotLights = false;

    // Skybox
    bool isSkyBox = false;

    // Environment Mapping - Material Properties
    bool isEnvMapping = false;
    bool isReflection = true;
    bool isRefraction = false;
    float ior = 1.52f;
    float f0 = 0.03f;
    float dispersion = 0.03f;
    float normalStrength = 0.0f;
    float specularSrength = 0.0f;

    // IK Controls
    bool isIK = false;
    int chainLength = 2;
    int maxIterations = 1;
    float tolerance = 1.0f;
    float targetLocation[3] = {0.0f, 3.5f, 0.0f};
    float targetSpeed = 0.005f;

    // Procedural Content Generation
    GLuint seed = 0;
    float floorOffset[3] = {-2.5f, 0.0f, 2.0f};
    float floorScale[3] = {1.2f, 1.0f, 1.2f};
    GLuint gridSize = 10;
    GLuint pointSize = 2;
    GLuint numPoints = 15;
    bool update = false;

public:
    // Constructor
    GUI();

    void InitializeImGuiIO();
    void initialize(GLFWwindow* mainWindow);
    void newFrame();
    void elements(const std::string& shadingMode);
    void render(const std::string& shadingMode);
    void warningMessage(const std::string& message);
    void errorMessage(const std::string& message);
    void shutdown();

    ImGuiIO getIO() const { return io; }

    // Getters for UI elements=========================================================================================
    // Wireframe
    bool getIsWireframe() const { return wireframe; }
    ImVec4 getWireframeColor() const { return wireframeColor; }

    // Textures/Object Color
    bool getIsShaded() const { return shaded; }
    ImVec4 getObjectColor() const { return objectColor; }

    // Background Color
    ImVec4 getBackgroundColor() const { return bgColor; }

    // Material Preview Mode
    bool getMaterialPreview() const { return materialPreview; }
    bool getSpecularPreview() const { return specularPreview; }
    bool getNormalPreview() const { return normalPreview; }

    // Camera
    int getSkyboxIndex() const { return skyboxIndex; }
    bool getCameraIsPerspective() const { return cameraIsPerspective; }
    bool getCameraIsOrthographic() const { return cameraIsOrthographic; }
    float getCameraFOV() const { return cameraFOV; }
    float getCameraScale() const { return cameraOrthoScale; }
    float getCameraNearClipping() const { return cameraNearClipping; }
    float getCameraFarClipping() const { return cameraFarClipping; }
    float getCameraSpeed() const { return cameraSpeed; }
    const float* getCameraPosition() const { return cameraPos; }

    // Directional Light Parameters
    const float* getDirectionalLightColor() const { return directionalLightColor; }
    const float* getDirectionalLightDirection() const { return directionalLightDirection; }
    float getDirectionalLightAmbient() const { return directionalLightAmbient; }
    float getDirectionalLightDiffuse() const { return directionalLightDiffuse; }

    // Point Light Parameters
    bool getIsPointLights() const { return isPointLights; }

    // Spot Light Parameters
    bool getIsSpotLights() const { return isSpotLights; }

    // Skybox Parameters
    bool getIsSkyBox() const { return isSkyBox; }

    // Environment Mapping Parameters - Material Properties
    bool getIsEnvMapping() const { return isEnvMapping; }
    bool getIsReflection() const { return isReflection; }
    bool getIsRefraction() const { return isRefraction; }
    float getIOR() const { return ior; }
    float getFresnelReflectance() const { return f0; }
    float getDispersion() const { return dispersion; }
    float getNormalStrength() const { return normalStrength; }
    float getSpecularStrength() const { return specularSrength; }

    // IK
    bool getIsIK() const { return isIK; }
    int getMaxIterations() const { return maxIterations; }
    float getTolerance() const { return tolerance; }
    int getChainLength() const { return chainLength; }
    const float* getTargetLocation() const { return targetLocation; }
    float getTargetSpeed() const { return targetSpeed; }

    // PCG
    GLuint getSeed() const { return seed; }
    const float* getFloorOffset() const { return floorOffset; }
    const float* getFloorScale() const { return floorScale; }
    GLuint getGridSize() const { return gridSize; }
    GLuint getPointSize() const { return pointSize; }
    GLuint getNumPoints() const { return numPoints; }
    bool getUpdate() const { return update; }

    // Setters=========================================================================================================
    void setCameraIsPerspective(bool flag);
    void setCameraIsOrthographic(bool flag);
    void setCameraFOV(float fieldOfView);
    void setCameraScale(float scale);
    void setCameraClipping(float near, float far);
    void setTargetLocation(float x, float y, float z);
    void setCameraPosition(float x, float y, float z);

    // Setter to reset the button press value
    void setUpdate(bool updateValue);

    // Destructor
    ~GUI();
};
