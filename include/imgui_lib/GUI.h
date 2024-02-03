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
    bool shaded = true;
    ImVec4 objectColor = ImVec4(151.0f / 255.0f, 153.0f / 255.0f, 153.0f / 255.0f, 1.0f);

    // Background Color
    ImVec4 bgColor = ImVec4(61.0f / 255.0f, 61.0f / 255.0f, 61.0f / 255.0f, 1.0f);

    // Material Preview Mode
    bool materialPreview = false;

    // Light settings
    // Directional Light
    float directionalLightColor[3] = {1.0f, 1.0f, 1.0f};
    float directionalLightDirection[3] = {2.0f, -1.0f, -2.0f};
    float directionalLightAmbient = 0.05f;
    float directionalLightDiffuse = 0.35f;

    // Point Lights
    bool isPointLights = true;

    // Spot Lights
    bool isSpotLights = true;

    // Skybox
    bool isSkyBox = false;

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

    // Getters for UI elements
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

    // Destructor
    ~GUI();
};
