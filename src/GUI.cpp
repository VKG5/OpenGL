#include "GUI.h"

GUI::GUI(/* args */)
{
}

void GUI::initialize() {
    // // Setup ImGUI Context
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();
    // (void)io;

    // // Setup Platform/Renderer bindings
    // ImGui_ImplGlfw_InitForOpenGL(mainWindow.getGLFWwindow(), true);
    // ImGui_ImplOpenGL3_Init("#version 460");

    // // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
}

void GUI::newFrame() {
    // // Start a new ImGui frame
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
}

void GUI::render() {
    // // Render ImGui elements here
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown() {
    // // Cleanup ImGui
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
}

GUI::~GUI()
{
}
