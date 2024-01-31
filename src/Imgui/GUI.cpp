#include "GUI.h"

GUI::GUI() {
}

void GUI::InitializeImGuiIO() {
    // Set all members to default or NULL values
    io = ImGui::GetIO();
    (void)io;
}

void GUI::initialize(GLFWwindow* mainWindow) {
    // Setup ImGUI Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    InitializeImGuiIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // You can customize ImGuiIO settings here if needed
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
}

void GUI::newFrame() {
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::render() {
    // Render ImGui elements here
    ImGui::Begin("OpenGL UI");
    ImGui::Text("Welcome to this simple program!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

GUI::~GUI() {
}
