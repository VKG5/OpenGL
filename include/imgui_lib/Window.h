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

    // Covering all ASCII characters
    bool keys[1024];

    // Checking mouse movement
    GLfloat lastX;
    GLfloat lastY;
    GLfloat xChange;
    GLfloat yChange;
    bool mouseFirstMoved;

    // Check for custom callbacks
    void createCallbacks();

    // Need static for callback
    static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
    static void handleMouse(GLFWwindow* window, double xPos, double yPos);

public:
    // Constructor
    Window();

    // Overloaded Constructor
    Window(GLint windowWidth, GLint windowHeight);

    int initialize();

    GLint getBufferWidth() {
        return bufferWidth;
    }

    GLint getBufferHeight() {
        return bufferHeight;
    }

    bool getShouldClose() {
        return glfwWindowShouldClose(mainWindow);
    }

    // Returning all keys, because we are passing the keys to the Camera class
    bool* getKeys() {
        return keys;
    }

    GLfloat getXChange();
    GLfloat getYChange();
    void changeState(bool enable);

    void swapBuffers() {
        glfwSwapBuffers(mainWindow);
    }

    // Getter for Window
    GLFWwindow* getWindow() { return mainWindow; }

    // Callback function to handle resizing events
    // static void resizeWindow(GLFWwindow* window, int width, int height);

    // Destructor
    ~Window();
};
