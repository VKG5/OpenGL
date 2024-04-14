// Custom Libraries
#include "MathFuncs.h"
#include "GUI.h"

// Basic C++ Libraries for various operations
#include <string>
#include <iostream>
#include <algorithm>

// Functions used to load textures and skybox + Save the render as an image file ont the disk
extern float* load_image_data(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels);
extern void free_image_data(void* imageData);
extern bool refreshRequired;

// Global control for slider speed in the GUI
float sliderSpeed = 0.005f;

namespace GUI {
	GLFWwindow* window;

	bool shouldQuit = false;

    constexpr char FONT_PATH[] = "OpenSans-Bold.ttf";
    constexpr float FONT_SIZE = 15.0f;
    constexpr char GLSL_VERSION[] = "#version 460 core";

	void initialize(GLFWwindow *window) {
		GUI::window = window;

		// Setup ImGUI Context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

        // Set all members to default or NULL values
        ImGuiIO io = ImGui::GetIO();
        (void)io;

        // Setting a custom font for ImGUI
        if (!io.Fonts->AddFontFromFileTTF(FONT_PATH, FONT_SIZE)) {
            std::cerr << "Error: Failed to load font '" << FONT_PATH << "'\n";
        }

		// Setup Dear ImGui style
        // ImGui::StyleColorsClassic();
		ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true) ||
            !ImGui_ImplOpenGL3_Init(GLSL_VERSION)) {
            std::cerr << "Error: ImGui GLFW or OpenGL3 initialization failed\n";
        }

        // You can customize ImGuiIO settings here if needed
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	}

    void newFrame() {
        // Start a new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    // These functions are used to create various sliders/elements and bind the values to shaders
	void shaderFloatParameter(const char* name, const char* displayName, float* floatPtr) {
		ImGui::Text(displayName);
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::DragFloat(std::string("##").append(name).c_str(), floatPtr, sliderSpeed)) {
			if (Scene::shaderID) {
                glUniform1f(glGetUniformLocation(Scene::shaderID, name), *floatPtr);
            }

            refreshRequired = true;
		}
	}

	void shaderSliderParameter(const char* name, const char* displayName, float* floatPtr) {
		ImGui::Text(displayName);
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::DragFloat(std::string("##").append(name).c_str(), floatPtr, sliderSpeed, 0.0f, 1.0f)) {
			if (Scene::shaderID) {
                glUniform1f(glGetUniformLocation(Scene::shaderID, name), *floatPtr);
            }

            refreshRequired = true;
		}
	}

	void shaderVecParameter(const char* name, const char* displayName, float* floatPtr) {
		ImGui::Text(displayName);
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::DragFloat3(std::string("##").append(name).c_str(), floatPtr, sliderSpeed)) {
			if (Scene::shaderID) {
                glUniform3f(glGetUniformLocation(Scene::shaderID, name), *(floatPtr + 0), *(floatPtr + 1), *(floatPtr + 2));
            }

            refreshRequired = true;
		}
	}

	void shaderColorParameter(const char* name, const char* displayName, float* floatPtr) {
		ImGui::Text(displayName);
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::ColorEdit3(name, floatPtr)) {
			if (Scene::shaderID) {
                glUniform3f(glGetUniformLocation(Scene::shaderID, name), *(floatPtr + 0), *(floatPtr + 1), *(floatPtr + 2));
            }

            refreshRequired = true;
		}
	}

    // Properties for selected object
    // If none is selected, it will display the properties for the floor
	void objectSettingsUI() {
		// ImGui::Begin("Selected object");

		ImGui::PushItemWidth(-1);
		if (Scene::selectedObjectIndex != -1) {
			int i = Scene::selectedObjectIndex;
			std::string indexStr = std::to_string(i);

			ImGui::Text(std::string("Object #").append(indexStr).c_str());

			shaderVecParameter(arrayElementName("u_objects", i, "position").c_str(), "Position", Scene::objects[i].position);

			ImGui::Text("Is Cube");
			ImGui::SameLine();
			bool isCube = Scene::objects[i].type == 2;
			std::string typeVariableName = arrayElementName("u_objects", i, "type");
			std::string scaleVariableName = arrayElementName("u_objects", i, "scale");

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::Checkbox(std::string("##").append(typeVariableName).c_str(), &isCube)) {
				Scene::objects[i].type = isCube ? 2 : 1;
				if (Scene::shaderID) glUniform1ui(glGetUniformLocation(Scene::shaderID, typeVariableName.c_str()), Scene::objects[i].type);
				refreshRequired = true;

				if (isCube) {
					Scene::objects[i].scale[0] *= 2.0f;
					Scene::objects[i].scale[1] *= 2.0f;
					Scene::objects[i].scale[2] *= 2.0f;
				}
				else {
					float minDimension = std::min({ Scene::objects[i].scale[0], Scene::objects[i].scale[1], Scene::objects[i].scale[2] });
					Scene::objects[i].scale[0] = minDimension / 2.0f;
					Scene::objects[i].scale[1] = minDimension / 2.0f;
					Scene::objects[i].scale[2] = minDimension / 2.0f;
				}

				if (Scene::shaderID) glUniform3f(glGetUniformLocation(Scene::shaderID, scaleVariableName.c_str()), Scene::objects[i].scale[0], Scene::objects[i].scale[1], Scene::objects[i].scale[2]);
			}

			if (Scene::objects[i].type == 1) {
				ImGui::Text("Radius");
				ImGui::SameLine();

                // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
				if (ImGui::InputFloat(std::string("##").append(scaleVariableName).c_str(), &Scene::objects[i].scale[0])) {
					Scene::objects[i].scale[1] = Scene::objects[i].scale[0];
					Scene::objects[i].scale[2] = Scene::objects[i].scale[0];
					if (Scene::shaderID) glUniform3f(glGetUniformLocation(Scene::shaderID, scaleVariableName.c_str()), Scene::objects[i].scale[0], Scene::objects[i].scale[1], Scene::objects[i].scale[2]);
					refreshRequired = true;
				}
			}

			else if (Scene::objects[i].type == 2) {
				shaderVecParameter(scaleVariableName.c_str(), "Scale", Scene::objects[i].scale);
			}

			shaderColorParameter(arrayElementName("u_objects", i, "material.albedo").c_str(), "Albedo", Scene::objects[i].material.albedo);
			shaderColorParameter(arrayElementName("u_objects", i, "material.specular").c_str(), "Specular", Scene::objects[i].material.specular);
			shaderColorParameter(arrayElementName("u_objects", i, "material.emission").c_str(), "Emission", Scene::objects[i].material.emission);
			shaderFloatParameter(arrayElementName("u_objects", i, "material.emissionStrength").c_str(), "Emission Strength", &Scene::objects[i].material.emissionStrength);

			shaderSliderParameter(arrayElementName("u_objects", i, "material.roughness").c_str(), "Roughness", &Scene::objects[i].material.roughness);
			shaderSliderParameter(arrayElementName("u_objects", i, "material.specularHighlight").c_str(), "Highlight", &Scene::objects[i].material.specularHighlight);
			shaderSliderParameter(arrayElementName("u_objects", i, "material.specularExponent").c_str(), "Exponent", &Scene::objects[i].material.specularExponent);

			ImGui::NewLine();
		}

		else {
		    ImGui::Text("Plane");

			ImGui::Text("Visible");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::Checkbox("##plane_visible", &Scene::planeVisible)) {
				if (Scene::shaderID) {
                    glUniform1ui(glGetUniformLocation(Scene::shaderID, "u_planeVisible"), Scene::planeVisible);
                }

                refreshRequired = true;
			}

			shaderColorParameter("u_planeMaterial.albedo", "Albedo", Scene::planeMaterial.albedo);
			shaderColorParameter("u_planeMaterial.specular", "Specular", Scene::planeMaterial.specular);
			shaderColorParameter("u_planeMaterial.emission", "Emission", Scene::planeMaterial.emission);
			shaderFloatParameter("u_planeMaterial.emissionStrength", "Emission Strength", &Scene::planeMaterial.emissionStrength);

			shaderSliderParameter("u_planeMaterial.roughness", "Roughness", &Scene::planeMaterial.roughness);
			shaderSliderParameter("u_planeMaterial.specularHighlight", "Highlight", &Scene::planeMaterial.specularHighlight);
			shaderSliderParameter("u_planeMaterial.specularExponent", "Exponent", &Scene::planeMaterial.specularExponent);
		}

		ImGui::PopItemWidth();
		// ImGui::End();
	}

	void lightSettingsUI() {
		// ImGui::Begin("Light settings");

		ImGui::PushItemWidth(-1);
		for (int i = 0; i < Scene::lights.size(); i++) {
			std::string indexStr = std::to_string(i);

			ImGui::Text(std::string("Light #").append(indexStr).c_str());
			ImGui::Text("Position");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::InputFloat3(std::string("##light_pos_").append(indexStr).c_str(), Scene::lights[i].position)) {
				if (Scene::shaderID) glUniform3f(glGetUniformLocation(Scene::shaderID, std::string("u_lights[").append(std::to_string(i)).append("].position").c_str()), Scene::lights[i].position[0], Scene::lights[i].position[1], Scene::lights[i].position[2]);
				refreshRequired = true;
			}

			ImGui::Text("Radius");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::InputFloat(std::string("##light_radius_").append(indexStr).c_str(), &Scene::lights[i].radius)) {
				if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, std::string("u_lights[").append(std::to_string(i)).append("].radius").c_str()), Scene::lights[i].radius);
				refreshRequired = true;
			}

			ImGui::Text(std::string("Light #").append(indexStr).c_str());
			ImGui::Text("Color");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::ColorEdit3(std::string("##light_color_").append(indexStr).c_str(), Scene::lights[i].color)) {
				if (Scene::shaderID) glUniform3f(glGetUniformLocation(Scene::shaderID, std::string("u_lights[").append(std::to_string(i)).append("].color").c_str()), Scene::lights[i].color[0], Scene::lights[i].color[1], Scene::lights[i].color[2]);
				refreshRequired = true;
			}

			ImGui::Text("Power");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::DragFloat(std::string("##light_power_").append(indexStr).c_str(), &Scene::lights[i].power, sliderSpeed)) {
				if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, std::string("u_lights[").append(std::to_string(i)).append("].power").c_str()), Scene::lights[i].power);
				refreshRequired = true;
			}

			ImGui::Text("Reach");
			ImGui::SameLine();

            // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
			if (ImGui::DragFloat(std::string("##light_reach_").append(indexStr).c_str(), &Scene::lights[i].reach, sliderSpeed)) {
				if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, std::string("u_lights[").append(std::to_string(i)).append("].reach").c_str()), Scene::lights[i].reach);
				refreshRequired = true;
			}

			if (i < 2) {
                ImGui::NewLine();
            }
        }

		ImGui::PopItemWidth();
		// ImGui::End();
	}

	void appSettingsUI() {
		// ImGui::Begin("App");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::PushItemWidth(-1);

        // Changing Rendering Mode
        ImGui::Text("Ray Tracing");
		ImGui::SameLine();
        if(ImGui::Checkbox("##ray_tracing", &Scene::isRayTracing)) {
            if (Scene::shaderID) {
                glUniform1i(glGetUniformLocation(Scene::shaderID, "u_useBlinnPhong"), Scene::isRayTracing);
            }

            // Debugging
            // std::cout<<Scene::isRayTracing<<'\n';

            refreshRequired = true;
        }

		ImGui::Text("Shadow resolution");
		ImGui::SameLine();
		if (ImGui::InputInt("##shadowResolution", &Scene::shadowResolution)) {
			if (Scene::shaderID) glUniform1i(glGetUniformLocation(Scene::shaderID, "u_shadowResolution"), Scene::shadowResolution);
			refreshRequired = true;
		}

		ImGui::Text("Light bounces");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputInt("##lightBounces", &Scene::lightBounces)) {
			if (Scene::shaderID) glUniform1i(glGetUniformLocation(Scene::shaderID, "u_lightBounces"), Scene::lightBounces);
			refreshRequired = true;
		}

		ImGui::Text("Passes per frame");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputInt("##framePasses", &Scene::framePasses)) {
			if (Scene::shaderID) glUniform1i(glGetUniformLocation(Scene::shaderID, "u_framePasses"), Scene::framePasses);
			refreshRequired = true;
		}

		ImGui::Text("Blur");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##blur", &Scene::blur)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_blur"), Scene::blur);
			refreshRequired = true;
		}

		ImGui::Text("Bloom Radius");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##bloomRadius", &Scene::bloomRadius)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_bloomRadius"), Scene::bloomRadius);
			refreshRequired = true;
		}

		ImGui::Text("Bloom Intensity");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##bloomIntensity", &Scene::bloomIntensity)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_bloomIntensity"), Scene::bloomIntensity);
			refreshRequired = true;
		}

		if (ImGui::Button("Quit")) {
			shouldQuit = true;
		}

        ImGui::PopItemWidth();

		// ImGui::End();
	}

	void skyboxSettingsUI() {
		// ImGui::Begin("Skybox");

		ImGui::PushItemWidth(-1);
		ImGui::Text("Intensity");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##skyboxStrength", &Scene::skyboxStrength)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_skyboxStrength"), Scene::skyboxStrength);
			refreshRequired = true;
		}

		ImGui::Text("Gamma");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##skyboxGamma", &Scene::skyboxGamma)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_skyboxGamma"), Scene::skyboxGamma);
			refreshRequired = true;
		}

		ImGui::Text("Ceiling");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		if (ImGui::InputFloat("##skyboxCeiling", &Scene::skyboxCeiling)) {
			if (Scene::shaderID) glUniform1f(glGetUniformLocation(Scene::shaderID, "u_skyboxCeiling"), Scene::skyboxCeiling);
			refreshRequired = true;
		}

		static char skyboxFilename[64];

		ImGui::Text("Filename");
		ImGui::SameLine();

        // '##' is used to hide the label for the particular element and provide unique identifiers to ImGui's internal state management
		ImGui::InputText("##skyboxFileName", skyboxFilename, 64);

		if (ImGui::Button("Load")) {
			int sbWidth, sbHeight, sbChannels;
			float* skyboxData = load_image_data(std::string("skyboxes\\").append(skyboxFilename).c_str(), &sbWidth, &sbHeight, &sbChannels, 0);
			if (skyboxData) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, Scene::skyboxTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sbWidth, sbHeight, 0, GL_RGB, GL_FLOAT, skyboxData);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glActiveTexture(GL_TEXTURE0);

				free_image_data(skyboxData);

				skyboxFilename[0] = 0;

				refreshRequired = true;
			}

			else {
				std::cout << "Failed to load skyboxes\\" << skyboxFilename << std::endl;
			}
		}

		ImGui::PopItemWidth();
		// ImGui::End();
	}

	void cameraSettingsUI() {
		// ImGui::Begin("Camera");
		ImGui::PushItemWidth(-1);
		ImGui::Text("Position");
		ImGui::SameLine();
		if (ImGui::InputFloat3("##cameraPosition", &Scene::cameraPosition.x)) {
			refreshRequired = true;
		}

		ImGui::Text("Orientation (Yaw-Pitch)");
		ImGui::SameLine();
		float cameraOrientation[2] = {Scene::cameraYaw, Scene::cameraPitch};
		if (ImGui::InputFloat2("##cameraOrientation", cameraOrientation)) {
			Scene::cameraYaw = cameraOrientation[0];
			Scene::cameraPitch = cameraOrientation[1];

			refreshRequired = true;
		}

		ImGui::PopItemWidth();
		// ImGui::End();
	}

    void elements() {
        // Current Rendering Mode
        // Set the font size
        ImGui::SetWindowFontScale(1.15f);
        ImGui::Text("Preview Mode:\n%s", "Ray Tracing");

        // Material Preview
        // Spacing
        ImGui::Spacing();
        // Set the font size to default
        ImGui::SetWindowFontScale(1.0f);

        // Add all elements that need to be rendered here
        if(ImGui::BeginTabBar("Properties")) {
            // Object Properties
            if (ImGui::BeginTabItem("Object")) {
                objectSettingsUI();

                // End Current Tab Item
                ImGui::EndTabItem();
            }

            // Light Properties
            if (ImGui::BeginTabItem("Light")) {
                lightSettingsUI();

                // End Current Tab Item
                ImGui::EndTabItem();
            }

            // Skybox Properties
            if (ImGui::BeginTabItem("Skybox")) {
                skyboxSettingsUI();

                // End Current Tab Item
                ImGui::EndTabItem();
            }

            // Camera Properties
            if (ImGui::BeginTabItem("Camera")) {
                cameraSettingsUI();

                // End Current Tab Item
                ImGui::EndTabItem();
            }

            // End Current Tab Bar
            ImGui::EndTabBar();
        }

        if(ImGui::BeginTabBar("System Properties")) {
            // Render Properties
            if(ImGui::BeginTabItem("Render")) {
                appSettingsUI();

                // End Current Tab Item
                ImGui::EndTabItem();
            }

            // End Current Tab Bar
            ImGui::EndTabBar();
        }
    }

    // This function renders all required elements onto the screen
	void render() {
        // Render ImGui elements here
        ImGui::Begin("Yumi");

        // Elements of the UI
        GUI::elements();

        ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

    // This function prints a message in yellow to indicate a warning
    void GUI::warningMessage(const std::string &message) {
        ImGui::Begin("Warning");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), message.c_str());
        ImGui::End();
    }

    // This function prints a message in red to indicate an error
    void GUI::errorMessage(const std::string &message) {
        ImGui::Begin("Error");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), message.c_str());
        ImGui::End();
    }

 	void shutdown() {
        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}
