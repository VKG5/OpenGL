// Basic C++ Libraries for various operations
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <GLFW/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Custom Libraries
#include "Mesh.h"

// Width and Height
const GLint width = 800, height = 600;

// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

// Creating a vector of the meshes
std::vector<Mesh*> meshList;

GLuint shader, uniformModel, uniformProjection;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float currAngle = 0.0f;
float currScale = 0.0f;

// Vertex Shader
// Uniform - Global to shader, not associated with a particular vertex
// Bind data to uniform to get location
static const char* vertexShader = R"(
    #version 460 core

    layout (location = 0) in vec3 pos;

    out vec4 col;

    // MVP - Model, View, Projection Structure
    uniform mat4 model;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * model * vec4(pos, 1.0);
        col = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    }
)";

// Fragment Shader
static const char* fragmentShader = R"(
    #version 460 core

    in vec4 col;

    out vec4 color;

    void main() {
        color = col;
    }
)";

void createTriangle() {
    // Index Array
    // Index of the vertices that are being drawn
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    // A VAO can hold multiple VBOs and other types of buffers
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Object 1
    Mesh* obj1 = new Mesh();
    obj1->createMesh(vertices, indices, 12, 12);

    // Adding to our meshlist
    meshList.push_back(obj1);

    // Object 2
    Mesh* obj2 = new Mesh();
    obj2->createMesh(vertices, indices, 12, 12);

    // Adding to our meshlist
    meshList.push_back(obj2);
}

void addShader(GLuint program, const char* shaderCode, GLenum shaderType) {
    // Creating an empty shader
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    // Creating this for logging/debugging
    GLint result = 0;
    GLchar eLog[1024] = {0};

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if(!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        std::cout<<"Error compiling the "<<shaderType<<" shader: "<<eLog<<"\n";
        return;
    }

    glAttachShader(program, theShader);
}

void compileShaders() {
    // Creating the program to run the shaders on the GPU
    shader = glCreateProgram();

    if(!shader) {
        std::cout<<"Error creating shader program";
        return;
    }

    addShader(shader, vertexShader, GL_VERTEX_SHADER);
    addShader(shader, fragmentShader, GL_FRAGMENT_SHADER);

    // Creating this for logging/debugging
    GLint result = 0;
    GLchar eLog[1024] = {0};

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if(!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        std::cout<<"Error linking program: "<<eLog<<"\n";
        return;
    }

    // Checking if valid in current context
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if(!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        std::cout<<"Error validating program: "<<eLog<<"\n";
        return;
    }

    // Name in the vertex shader
    uniformModel = glGetUniformLocation(shader, "model");
    uniformProjection = glGetUniformLocation(shader, "projection");
}

int main() {
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

    GLFWwindow* mainWindow = glfwCreateWindow( width, height, "Basic", NULL, NULL);

    if(!mainWindow) {
        std::cout<<"GLFW Window creation failed!";
        glfwTerminate();
        return 1;
    }

    // Get buffer size info
    int bufferWidth, bufferHeight;
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

    createTriangle();
    compileShaders();

    glm::mat4 projection = glm::perspective(45.0f, GLfloat(bufferWidth)/GLfloat(bufferHeight), 0.1f, 100.0f);

    // Main Loop - Running till the window is open
    while(!glfwWindowShouldClose(mainWindow)) {
        // Get + Handle user input events
        glfwPollEvents();

        // Direction true = Right
        if(direction){
            triOffset += triIncrement;
        }

        else{
            triOffset -= triIncrement;
        }

        // Switching Direction upon hitting boundary
        if(abs(triOffset) >= triMaxOffset) {
            direction = !direction;
        }

        // Updating angle
        currAngle += 0.01f;
        if(currAngle >= 360) {
            currAngle -= 360;
        }

        // Updaating scale
        currScale += 0.001f;
        if(currScale >= 1.0f) {
            currScale -= 1.0f;
        }

        // Clear window
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Binding the program
        glUseProgram(shader);

            glm::mat4 model = glm::mat4(1.0f);
            // Happens in a reverse order
            // Translate
            model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            meshList[0]->renderMesh();

            // Clearing out the properties
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-triOffset, 1.0f, -2.5f));
            model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            meshList[1]->renderMesh();

        // Un-Binding the program
        glUseProgram(0);

        // We have 2 scenes, one which we are drawing to and one current
        glfwSwapBuffers(mainWindow);
    }

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
