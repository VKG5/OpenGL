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
    float directionalLightAmbient = 0.125f;
    float directionalLightDiffuse = 0.250f;

    // Skybox
    int skyboxIndex = 1;

    // Camera
    float cameraSpeed = 10.0f;
    float cameraPos[3] = {-0.151f, 1.412f, 19.340f};

    // Point Lights
    bool isPointLights = false;

    // Spot Lights
    bool isSpotLights = false;

    // Skybox
    bool isSkyBox = false;

    // Environment Mapping
    bool isEnvMapping = false;
    bool isReflection = true;
    bool isRefraction = false;
    float ior = 1.52f;
    float f0 = 0.03f;
    float dispersion = 0.03f;

    // Controls
    bool isEuler = true;
    float planePosition[3] = {0.0f, 0.0f, 0.0f};
    float planeRotation[3] = {0.0f, 0.0f, 0.0f};
    float planeRotationQuat[3] = {0.0f, 0.0f, 0.0f};
    float planeSpeed = 5.0f;
    float planeTurnSpeed = 0.01f;
    bool isPropeller = true;
    float propellerSpeed = 0.5f;
    ImVec2 firstPersonOffset = ImVec2(4.0f, -0.25f);
    ImVec2 thirdPersonOffset = ImVec2(-6.69f, 25.69f);

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
    void elements(std::string& shadingMode);
    void render(std::string& shadingMode);
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

    // Environment Mapping Parameters
    bool getIsEnvMapping() const { return isEnvMapping; }
    bool getIsReflection() const { return isReflection; }
    bool getIsRefraction() const { return isRefraction; }
    float getIOR() const { return ior; }
    float getFresnelReflectance() const { return f0; }
    float getDispersion() const { return dispersion; }

    // Controls
    bool getIsEuler() const { return isEuler; }
    const float* getPlanePosition() const { return planePosition; }
    const float* getPlaneRotation() const { return planeRotation; }
    const float* getPlaneRotationQuat() const { return planeRotationQuat; }
    float getPlaneSpeed() const { return planeSpeed; }
    float getPlaneTurnSpeed() const { return planeTurnSpeed; }
    bool getIsPropeller() const { return isPropeller; }
    float getPropellerSpeed() const { return propellerSpeed; }
    ImVec2 getFPOffset() const { return firstPersonOffset; }
    ImVec2 getTPOffset() const { return thirdPersonOffset; }

    // PCG
    GLuint getSeed() const { return seed; }
    const float* getFloorOffset() const { return floorOffset; }
    const float* getFloorScale() const { return floorScale; }
    GLuint getGridSize() const { return gridSize; }
    GLuint getPointSize() const { return pointSize; }
    GLuint getNumPoints() const { return numPoints; }
    bool getUpdate() const { return update; }

    // Setters=========================================================================================================
    void setPlanePosition(float x, float y, float z);
    void setPlaneRotation(float x, float y, float z);
    void setCameraPosition(float x, float y, float z);

    // Setter to reset the button press value
    void setUpdate(bool updateValue);

    // Destructor
    ~GUI();
};
