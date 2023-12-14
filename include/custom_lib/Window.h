#pragma once

#include <stdio.h>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <GLFW/glfw3.h>

class Window {
private:
    GLFWwindow* mainWindow;
    GLint width, height;
    GLint bufferWidth, bufferHeight;

public:
    // Constructor
    Window();

    // Overloaded Constructor
    Window(GLint windowWidth, GLint windowHeight);

    int initialize();

    GLint getBufferWidht() {
        return bufferWidth;
    }

    GLint getBufferHeight() {
        return bufferHeight;
    }

    bool getShouldClose() {
        return glfwWindowShouldClose(mainWindow);
    }

    void swapBuffers() {
        glfwSwapBuffers(mainWindow);
    }

    // Destructor
    ~Window();
};
