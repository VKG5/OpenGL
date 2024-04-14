#pragma once

// ImGui libraries
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Scene Header for operations
#include "Scene.h"

// Basic C++ Libraries for various operations
#include <string>

namespace GUI {
	extern GLFWwindow* window;
	extern bool shouldQuit;
	extern bool animationRenderWindowVisible;

	void initialize(GLFWwindow* window);
    void newFrame();
    void elements();
    void render();
    void warningMessage(const std::string &message);
    void errorMessage(const std::string &message);
    void shutdown();
};
