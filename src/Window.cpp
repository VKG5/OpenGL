#include <iostream>
#include "Window.h"

Window::Window() {
    width = 800;
    height = 600;
}

Window::Window(GLint windowWidth, GLint windowHeight) {
    width = windowWidth;
    height = windowHeight;
}

int Window::initialize() {
    // Creating a lambda callback function for GLFW
    glfwSetErrorCallback(
        [] ( int error, const char* description ) {
            std::cout << stderr << ", Error: " << description;
        }
    );

    // Making a basic window in GLFW
    if(!glfwInit()) {
        std::cout<<"GLFW Failed to initialize";
        glfwTerminate();
        exit( EXIT_FAILURE );
    }

    // Setting up GLFW Window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE
    );

    // Core profile = No backwards compatiblity
    glfwWindowHint (
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE
    );

    mainWindow = glfwCreateWindow( width, height, "Basic", NULL, NULL);

    if(!mainWindow) {
        std::cout<<"GLFW Window creation failed!";
        glfwTerminate();
        return -1;
    }

    // Get buffer size info
    glfwGetFramebufferSize( mainWindow, &bufferWidth, &bufferHeight );

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Initialize the OpenGL context using Glad or GLEW
    // Initialize Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize Glad" << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return -1;
    }

    // Enabling Depth Buffer
    glEnable(GL_DEPTH_TEST);

    // Setup Viewport size
    // x,y - Top left corner position
    // bufferWidth and bufferHeight - Give us the height in between the window
    glViewport( 0, 0, bufferWidth, bufferHeight );

    return 0;
}

Window::~Window() {
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}
