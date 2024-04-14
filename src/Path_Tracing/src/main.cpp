// STB
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

// Including all necessary headers
// ImGUI
#include "GUI.h"

// Basic C++ Libraries for various operations
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

// Always include GLFW after GLAD/GLEW - Core Libraries
//#include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Utilities
// This header contains common include files such as stb and macros for ease of access
#include "Utilities.h"

// Custom Libraries
#include "Scene.h"
#include "Procedural_scenes.h"

// Global booleans to account for various actions performed by the user
bool mouseAbsorbed = false;
bool refreshRequired = false;

glm::mat4 rotationMatrix(1);
glm::vec3 forwardVector(0, 0, -1);

// Setting the variables
GLuint shaderProgram, screenTexture,
       directOutPassUniformLocation, accumulatedPassesUniformLocation,
       timeUniformLocation, camPosUniformLocation,
       rotationMatrixUniformLocation,
       aspectRatioUniformLocation, uniformFBO;


// Helper Functions====================================================================================================
// These functions are used to manipulate images, such as load texture, HDRIs and save renders
float* load_image_data(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels) {
	return stbi_loadf(filename, x, y, channels_in_file, desired_channels);
}

void free_image_data(void* imageData) {
	stbi_image_free(imageData);
}

// *Reference: https://lencerf.github.io/post/2019-09-21-save-the-opengl-rendering-to-image-file/
void saveImage(GLFWwindow* w, int accumulatedPasses, const char* filepath) {
    int width, height;

    // Get the size of the framebuffer
    glfwGetFramebufferSize(w, &width, &height);

    // Number of color channels (RGB)
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;

    // Align stride to 4 bytes
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;

    float* floatBuffer = new float[bufferSize];
    if (!floatBuffer) {
        std::cerr << "Error: Unable to allocate memory for the float buffer.\n";
        return;
    }

    // Set pixel storage mode
    glPixelStorei(GL_PACK_ALIGNMENT, 4);

    // Read from the front buffer
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, floatBuffer);
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << glError << " occurred while reading pixels.\n";
        delete[] floatBuffer;
        return;
    }

    unsigned char* byteBuffer = new unsigned char[bufferSize];
    if (!byteBuffer) {
        std::cerr << "Error: Unable to allocate memory for the byte buffer.\n";
        delete[] floatBuffer;
        return;
    }

    for (int i = 0; i < bufferSize; i++) {
        byteBuffer[i] = static_cast<unsigned char>(std::min(floatBuffer[i] / accumulatedPasses, 1.0f) * 255.0f);
    }

    // Flip vertically on write
    stbi_flip_vertically_on_write(true);
    int result = stbi_write_png(filepath, width, height, nrChannels, byteBuffer, stride);

    if (result) {
        std::cout << "Successfully saved image to '" << filepath << "'.\n";
    }

    else {
        std::cerr << "Failed to write PNG to '" << filepath << "'. Check if the directory exists, the path is correct, and the application has write permissions.\n";
    }

    delete[] floatBuffer;
    delete[] byteBuffer;
}


// Callback Functions==================================================================================================
// Various callback functions for our GLFW context
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;

	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	refreshRequired = true;
}

void mousebuttonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && !mouseAbsorbed && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

        // Press G to add an object to the cursor
		if (glfwGetKey(window, GLFW_KEY_G)) {
			Scene::mousePlace(mouseX, mouseY, screenWidth, screenHeight, Scene::cameraPosition, rotationMatrix);
		}
		else {
			Scene::selectHovered(mouseX, mouseY, screenWidth, screenHeight, Scene::cameraPosition, rotationMatrix);
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_C) {
			if (mouseAbsorbed) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				mouseAbsorbed = false;
			}

			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				glfwSetCursorPos(window, screenWidth / 2.0, screenHeight / 2.0);
				mouseAbsorbed = true;

				Scene::selectedObjectIndex = -1;
				glUniform1i(glGetUniformLocation(shaderProgram, "u_selectedSphereIndex"), -1);
			}
		}

        if (key == GLFW_KEY_F) {
            // saveImage takes a GLFWwindow*, an int quality, and a const char* filename
            saveImage(window, 1, "D:\\Programs\\C++\\Rendering\\OpenGL\\src\\Ray_Tracing\\src\\renders\\output.png");
        }
	}
}

bool handleMovementInput(GLFWwindow* window, double deltaTime, glm::vec3& cameraPosition, float& cameraYaw, float& cameraPitch, glm::mat4* rotationMatrix) {
	bool moved = false;

	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	glfwSetCursorPos(window, screenWidth / 2.0, screenHeight / 2.0);

	float xOffset = (float)(mouseX - screenWidth/2.0);
	float yOffset = (float)(mouseY - screenHeight/2.0);

	if (xOffset != 0.0F || yOffset != 0.0F) moved = true;

	cameraYaw += xOffset * 0.002F;
	cameraPitch += yOffset * 0.002F;

	if (cameraPitch > 1.5707F)
		cameraPitch = 1.5707F;
	if (cameraPitch < -1.5707F)
		cameraPitch = -1.5707F;

	*rotationMatrix = glm::rotate(glm::rotate(glm::mat4(1), cameraPitch, glm::vec3(1, 0, 0)), cameraYaw, glm::vec3(0, 1, 0));

	glm::vec3 forward = glm::vec3(glm::vec4(0, 0, -1, 0) * (*rotationMatrix));

	glm::vec3 up(0, 1, 0);
	glm::vec3 right = glm::cross(forward, up);

	glm::vec3 movementDirection(0);
	float multiplier = 1;

	if (glfwGetKey(window, GLFW_KEY_W)) {
		movementDirection += forward;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		movementDirection -= forward;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		movementDirection += right;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		movementDirection -= right;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		movementDirection += up;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		movementDirection -= up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		multiplier = 5;
	}

	if (glm::length(movementDirection) > 0.0f) {
		cameraPosition += glm::normalize(movementDirection) * (float)deltaTime * (float)multiplier;
		moved = true;
	}

	return moved;
}


// Shader Functions====================================================================================================
// Loads the shader source from disk, replaces the constants and returns a new OpenGL Program
GLuint createShaderProgram(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Unable to open %s.\n", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// Uses createShaderProgram to create a program with the correct constants depending on the Scene and reassigns everything that needs to be. If a program already exists, it is deleted.
void recompileShader() {
	if (shaderProgram) glDeleteProgram(shaderProgram);
	shaderProgram = createShaderProgram("shaders\\RayTracing.vert", "shaders\\RayTracing.frag");
	glUseProgram(shaderProgram);
	Scene::bind(shaderProgram);

	directOutPassUniformLocation = glGetUniformLocation(shaderProgram, "u_directOutputPass");
	accumulatedPassesUniformLocation = glGetUniformLocation(shaderProgram, "u_accumulatedPasses");
	timeUniformLocation = glGetUniformLocation(shaderProgram, "u_time");
	camPosUniformLocation = glGetUniformLocation(shaderProgram, "u_cameraPosition");
	rotationMatrixUniformLocation = glGetUniformLocation(shaderProgram, "u_rotationMatrix");
	aspectRatioUniformLocation = glGetUniformLocation(shaderProgram, "u_aspectRatio");

	glUniform1i(glGetUniformLocation(shaderProgram, "u_screenTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "u_skyboxTexture"), 1);
}


// Main Function=======================================================================================================
int main() {
    // Loading HDRI texture for Skybox
	std::cout << "Loading skybox" << std::endl;
	int sbWidth, sbHeight, sbChannels;
	float* skyboxData = stbi_loadf("skyboxes\\the_sky_is_on_fire_4k.hdr", &sbWidth, &sbHeight, &sbChannels, 0);

    // Making a basic window in GLFW
    if (!glfwInit()) {
        std::cout << "GLFW Failed to initialize";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Setting up GLFW Window properties
    // OpenGL version
    // The following commands only work with GLAD!
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    //// Core profile = No backwards compatiblity
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //// Set the GLFW window hint for resizable
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(screenWidth, screenHeight, "~Yumi - Ray Tracing~", NULL, NULL);

    if (!mainWindow) {
        std::cout << "GLFW Window creation failed!";
        glfwTerminate();
        return -1;
    }

	glfwSetFramebufferSizeCallback(mainWindow, framebufferSizeCallback);
	glfwSetMouseButtonCallback(mainWindow, mousebuttonCallback);
	glfwSetKeyCallback(mainWindow, keyCallback);

	glfwMakeContextCurrent(mainWindow);

    // Initialize the OpenGL context using Glad or GLEW
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Objects=========================================================================================================
    // Initialize any resources here
    // Place a sphere and a plane in the scene
    // placeBasicScene();

    // Place the Cornell Box scene
    placeCornellBoxScene();

    // Place multiple spheres which represent mirrors
    //placeMirrorSpheres();

    // Generate random sphere with random materials in a defined grid
    // Parameters include number of sphere/objects and the size of grid across which they will be spread
    //generateRandomSpheres(16, 5);

    // Recompile Shader after loading and binding object + skybox
	recompileShader();

    // ImGUI===========================================================================================================
    // Initializing GUI before main loop
    GUI::initialize(mainWindow);

	glGenTextures(1, &Scene::skyboxTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Scene::skyboxTexture);

    // Load skybox texture
    // For HDRIs
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, sbWidth, sbHeight, 0, GL_RGB, GL_FLOAT, skyboxData);

    // For PNG
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sbWidth, sbHeight, 0, GL_RGBA, GL_FLOAT, skyboxData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(skyboxData);

	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(vertexArray);

	glGenTextures(1, &screenTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenFramebuffers(1, &uniformFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, uniformFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
		return -1;
	}

	glUniform1i(glGetUniformLocation(shaderProgram, "u_screenTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "u_skyboxTexture"), 1);

	glViewport(0, 0, screenWidth, screenHeight);
	glDisable(GL_DEPTH_TEST);

	double deltaTime = 0.0f;
	int freezeCounter = 0;
	int accumulatedPasses = 0;

    // Main Loop
    while (!glfwWindowShouldClose(mainWindow) && !GUI::shouldQuit)
    {
		double lastTime = glfwGetTime();
		glfwPollEvents();

        if (mouseAbsorbed) {
            if (handleMovementInput(mainWindow, deltaTime, Scene::cameraPosition, Scene::cameraYaw, Scene::cameraPitch, &rotationMatrix)) {
                refreshRequired = true;
            }
        }
        else {
            rotationMatrix = glm::rotate(glm::rotate(glm::mat4(1), Scene::cameraPitch, glm::vec3(1, 0, 0)), Scene::cameraYaw, glm::vec3(0, 1, 0));
        }

        // Quit program
        if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE)) {
            break;
        }

		if (refreshRequired) {
			accumulatedPasses = 0;
			refreshRequired = false;

             // If the shader receives a value of 0 for accumulatedPasses, it will discard the buffer and just output what it rendered on that frame.
			glUniform1i(accumulatedPassesUniformLocation, accumulatedPasses);
		}

		glUniform1f(timeUniformLocation, (float)lastTime);
		glUniform3f(camPosUniformLocation, Scene::cameraPosition.x, Scene::cameraPosition.y, Scene::cameraPosition.z);
		glUniformMatrix4fv(rotationMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(rotationMatrix));
		glUniform1f(aspectRatioUniformLocation, (float)screenWidth / screenHeight);

		// Step 1: Render to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, uniformFBO);
		glUniform1i(directOutPassUniformLocation, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		accumulatedPasses += 1;

		// Step 2: Render to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUniform1i(directOutPassUniformLocation, 1);
		glUniform1i(accumulatedPassesUniformLocation, accumulatedPasses);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (!mouseAbsorbed) {
            // UI - Render Frame
            // We cannot create a new frame is there is not GUI being rendered, it throws an error!
            GUI::newFrame();

            // UI - Render GUI
            GUI::render();
        }

		glfwSwapBuffers(mainWindow);

		deltaTime = glfwGetTime() - lastTime;

        // Edge case - We shut down if the window seems to be stuck
        // Wait time is 2 seconds, you can increase/decrease it here
		if (deltaTime > 2.0F) {
			freezeCounter += 1;
			if (freezeCounter >= 2) {
				std::cout << "Freeze detected. Shutting down..." << std::endl;
				break;
			}
		}

		else {
			freezeCounter = 0;
		}

        // TODO : Implement saving functionality!
        // saveImage(mainWindow, 1, std::string("render_output\\").append("output.png").c_str());
	}

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteProgram(shaderProgram);
	glDeleteFramebuffers(1, &uniformFBO);
	glDeleteTextures(1, &screenTexture);


	GUI::shutdown();

	glfwDestroyWindow(mainWindow);
	glfwTerminate();

	return 0;
}
