#pragma once

// ImGui libraries
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include <imgui_impl_opengl3.h>

#include <glad.h>
#include <GLFW/glfw3.h>

class GUI
{
private:
    ImGuiIO io;

public:
    // Constructor
    GUI();

    void InitializeImGuiIO();
    void initialize(GLFWwindow* mainWindow);
    void newFrame();
    void render();
    void shutdown();

    ImGuiIO getIO() { return io; }

    // Destructor
    ~GUI();
};
