#include "GUI.h"

// Global variables for stride speed
float sliderSpeed = 0.01f;

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

void GUI::elements(std::string& shadingMode) {
    // Current Shading Mode
    // Set the font size
    ImGui::SetWindowFontScale(1.15f);
    ImGui::Text("Preview Mode:\n%s", shadingMode.c_str());

    // Material Preview
    // Spacing
    ImGui::Spacing();
    // Set the font size to default
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Checkbox("Material Preview", &materialPreview);

    // Spacing
    ImGui::Spacing();
    ImGui::Checkbox("Wireframe", &wireframe);
    // Options based on wireframe
    if(wireframe) {
        ImGui::ColorEdit4("Wire Color", (float*)&wireframeColor);
    }

    // Spacing
    // You can also use - ImGui::Dummy(ImVec2(0.0f, 20.0f)); for custom height parameters (horizontal, vertical)
    ImGui::Spacing();
    ImGui::Checkbox("Shaded", &shaded);
    // Options based on wireframe
    if(!shaded) {
        ImGui::ColorEdit4("Object Color", (float*)&objectColor);
    }

    // Spacing
    ImGui::Spacing();
    ImGui::ColorEdit4("Background Color", (float*)&bgColor);

    // Lighting
    // Spacing
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::SetWindowFontScale(1.15f);
    ImGui::Text("Lighting");

    // Directional Light Controls
    ImGui::Spacing();
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("Color", directionalLightColor);
    ImGui::DragFloat3("Direction", directionalLightDirection, sliderSpeed, -360.0f, 360.0f, "%.3f");
    ImGui::DragFloat("Ambient", &directionalLightAmbient, sliderSpeed);
    ImGui::DragFloat("Diffuse", &directionalLightDiffuse, sliderSpeed);

    // TODO : Implement Point Light Controls
    ImGui::Spacing();
    ImGui::Text("Point Lights");
    ImGui::Checkbox("Point Active", &isPointLights);

    // TODO : Implement Spot Light Controls
    ImGui::Spacing();
    ImGui::Text("Spot Lights");
    ImGui::Checkbox("Spot Active", &isSpotLights);
}

void GUI::render(std::string& shadingMode) {
    // Render ImGui elements here
    ImGui::Begin("OpenGL UI");

    // Elements of the UI
    elements(shadingMode);

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
    shutdown();
}
