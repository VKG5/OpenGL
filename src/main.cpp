// Basic C++ Libraries for various operations
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <cmath>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <GLFW/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Width and Height
const GLint width = 800, height = 600;
// Converting to Radians
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

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
    uniform mat4 model;

    void main() {
        gl_Position = model * vec4(pos, 1.0);

    }
)";

// Fragment Shader
static const char* fragmentShader = R"(
    #version 460 core

    out vec4 color;

    void main() {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }
)";

void createTriangle() {
    // A VAO can hold multiple VBOs and other types of buffers
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Creating and gettting the vertex ID of a VAO
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

        // Creating the buffer object instide VAO
        glCreateBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // STATIC DRAW - Not chaning the values in the array
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // Location = 0
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(0);

        // Un-Binding Buffer Array
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Un-Binding Vertex Array
    glBindVertexArray(0);
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

    // Setup Viewport size
    // x,y - Top left corner position
    // bufferWidth and bufferHeight - Give us the height in between the window
    glViewport( 0, 0, bufferWidth, bufferHeight );

    createTriangle();
    compileShaders();

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
        glClear(GL_COLOR_BUFFER_BIT);

        // Binding the program
        glUseProgram(shader);

            glm::mat4 model = glm::mat4(1.0f);
            // Happens in a reverse order
            // Rotate
            // model = glm::rotate(model, currAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

            // Translate
            // model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));

            // Scale
            model = glm::scale(model, glm::vec3(currScale, currScale, 1.0f));

            // Binding the uniform using pointer (v)
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

            glBindVertexArray(VAO);

                glDrawArrays(GL_TRIANGLES, 0, 3);

            glBindVertexArray(0);
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
