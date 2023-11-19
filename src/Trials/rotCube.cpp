#include <iostream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <glfw/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using glm::mat4;
using glm::vec3;

// Generating the vertex data for our triangle
/*
Vertex Shader
We are manually inputting location = 0, so that it
matches the Fragment shader
We are stirubg the position and color of cube vertices in 2 arrays
Share 8 vertices among 6 faces
*/
static const char* shaderCodeVertex = R"(
#version 460 core
layout (std140, binding = 0) uniform perFrameData {
    uniform mat4 mvp;
    uniform int isWireFrame;
};

layout (location = 0) out vec3 color;

const vec3 pos[8] = vec3[8] (
    vec3(-1.0, -1.0, 1.0),
    vec3(1.0, -1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(-1.0, 1.0, 1.0),

    vec3(-1.0, -1.0, -1.0),
    vec3(1.0, -1.0, -1.0),
    vec3(1.0, 1.0, -1.0),
    vec3(-1.0, 1.0, -1.0)
);

const vec3 col[8] = vec3[8] (
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 1.0, 0.0),

    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),
    vec3(0.0, 1.0, 0.0),
    vec3(1.0, 0.0, 0.0)
);

// Indices to contruct the cube face
const int indices[36] = int[36] (
    // Front
    0, 1, 2, 2, 3, 0,
    // Right
    1, 5, 6, 6, 2, 1,
    //Back
    7, 6, 5, 5, 4, 7,
    // Left
    4, 0, 3, 3, 7, 4,
    // Bottom
    4, 5, 1, 1, 0, 4,
    // Top
    3, 2, 6, 6, 7, 3
);

void main() {
    int idx = indices[gl_VertexID];
    gl_Position = mvp * vec4( pos[idx], 1.0 );
    // Black color if we are rendering the wireframe otherwise, the assigned color
    color = isWireFrame > 0 ? vec3(0.0) : col[ idx ];
}
)";

/*
Fragment Shader
The location value of Fragment Shader should match the
corresponding location value in the Vertex Shader
*/
static const char* shaderCodeFragment = R"(
    #version 460 core
    layout (location = 0) in vec3 color;
    layout (location = 0) out vec4 out_FragColor;

    void main() {
        out_FragColor = vec4( color, 1.0 );
    }
)";

/* Struct to hold data
1. MVP - Multi View Projection Matrix
2. isWireFrame - Extra functionality, setting the color of wireframe rendering
*/
struct perFrameData {
    mat4 mvp;
    int isWireFrame;
};

// Main function
int main() {
    // Creating a lambda callback function for GLFW
    glfwSetErrorCallback(
        [] ( int error, const char* description ) {
            std::cout << stderr << ", Error: " << description;
        }
    );

    if( !glfwInit() )
        exit( EXIT_FAILURE );

    // Setting OpenGL Version - 4.6
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint ( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint (
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        1024, 768, "Default Cube", nullptr, nullptr
    );

    if( !window ) {
        glfwTerminate();
        exit ( EXIT_FAILURE );
    }

    // Setting a lambda callback for key events
    glfwSetKeyCallback( window,
        [] ( GLFWwindow* window,
             int key, int scancode, int action, int mods ) {
                if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
                    glfwSetWindowShouldClose( window, GLFW_TRUE );
            }
    );

    // Preparing OpenGL context using GLAD library
    glfwMakeContextCurrent ( window );
    gladLoadGLLoader ( (GLADloadproc) glfwGetProcAddress );
    glfwSwapInterval ( 1 );

    /*
    Now we want OpenGL to actually render some basic graphics
    We'll make a colored triangle for that
    1. Vertex Shader
    2. Fragment Shader
    Vertex Array Object (VAO)
    */
    GLuint VAO;
    glCreateVertexArrays ( 1, &VAO );
    glBindVertexArray ( VAO );

    // Compiling and Linking to a Shader Program
    const GLuint shaderVertex =
        glCreateShader ( GL_VERTEX_SHADER );

    glShaderSource (
        shaderVertex, 1, &shaderCodeVertex, nullptr
    );

    glCompileShader (
        shaderVertex
    );

    const GLuint shaderFragment =
        glCreateShader ( GL_FRAGMENT_SHADER );

    glShaderSource (
        shaderFragment, 1, &shaderCodeFragment, nullptr
    );

    glCompileShader (
        shaderFragment
    );

    // Creating a program for attaching and rendering the shaders
    const GLuint program = glCreateProgram();

    glAttachShader ( program, shaderVertex );
    glAttachShader ( program, shaderFragment );
    glLinkProgram ( program );
    glUseProgram ( program );

    /*
    We will use the Direct State Access (DSA) methods from OpenGL
    Instead of classic bind-to-edit approach
    We will also be using the Unifrom Buffer for this
    */
    const GLsizeiptr kBufferSize = sizeof( perFrameData );
    GLuint perFrameDataBuf;
    glCreateBuffers( 1, &perFrameDataBuf );
    glNamedBufferStorage( perFrameDataBuf, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT );
    glBindBufferRange( GL_UNIFORM_BUFFER, 0, perFrameDataBuf, 0, kBufferSize );

    /*
    GLFW Main Loop
    */
    // Clearing the screen to create the triangle
    // Updating the buffer twice per frame
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    /*
    Since we are rendering a cube
    We need a depth pass (Z-Buffer) and polygon offset for rendering
    Wireframe over the solid cube
    */
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POLYGON_OFFSET_LINE );
    glPolygonOffset( -1.0f, -1.0f );

    while( !glfwWindowShouldClose( window ) ) {
        // Making resizable window
        /*
        We can also use
        glfwSetWindowSizeCallback() to set a GLFW window resize callback
        */
        int width, height;

        glfwGetFramebufferSize (
            window, &width, &height
        );

        const float ratio = width / (float)height;

        glViewport( 0, 0, width, height );

        // Clearing the depth and color buffer (Since we are using both now)
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Rotation around diagonal (1, 1, 1) axis
        // Angle of rotation  = System Time (glfwGetTime())
        const mat4 m = glm::rotate (
            glm::translate ( mat4(1.0f), vec3( 0.0f, 0.0f, -3.5f ) ),
            (float)glfwGetTime(), vec3 ( 1.0f, 1.0f, 1.0f )
        );

        const mat4 p = glm::perspective (
            45.0f, ratio, 0.1f, 1000.0f
        );

        perFrameData PerFrameData = { p*m, false };

        glNamedBufferSubData (
            perFrameDataBuf, 0, kBufferSize, &PerFrameData
        );

        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

        // We can also pass the empty VAO object we created earlier
        // We are starting at triangle 0, and going up till 36 (Total number of triangles)
        glDrawArrays( GL_TRIANGLES, 0, 36 );

        // Updating the buffer to render the wireframe
        PerFrameData.isWireFrame = true;
        glNamedBufferSubData (
            perFrameDataBuf, 0, kBufferSize, &PerFrameData
        );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
        glDrawArrays( GL_TRIANGLES, 0, 36 );

        /*
        The fragment shader output has been rendered into the back buffer.
        We swap the front and back buffers to make the triangle visible.
        Do not forget to poll the events (Otherwise the loop will never end)
        */
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    /* Memory management
    1. Deleting in reverse order (Similar to how destructors work)
    */
    glDeleteBuffers(1, &perFrameDataBuf);
    glDeleteProgram(program);
    glDeleteShader(shaderFragment);
    glDeleteShader(shaderVertex);
    glDeleteVertexArrays(1, &VAO);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
