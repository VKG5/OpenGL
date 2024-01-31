#pragma once

// ImGui libraries
#include <imgui.h>

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
