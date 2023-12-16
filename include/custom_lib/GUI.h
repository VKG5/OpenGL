#pragma once

// ImGui libraries
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class GUI
{
public:
    // Constructor
    GUI();

    void initialize();
    void newFrame();
    void render();
    void shutdown();

    // Destructor
    ~GUI();
};
