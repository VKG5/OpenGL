#include <iostream>

// TODO - Install GLEW
// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <glfw/glfw3.h>

// GLM for mathematics
#include <glm/glm.hpp>
#include <glm/ext.hpp>

// Assimp
#include <assimp/scene.h>

// Generating the vertex data for our triangle
/*
Vertex Shader
We are manually inputting location = 0, so that it
matches the Fragment shader
*/
static const char* shaderVertexCode = R"(
#version 460 core
layout (location = 0) out vec3 color;
const vec2 pos[3] = vec2[3] (
    vec2( -0.6, -0.4 ),
    vec2( 0.6, 0.4 ),
    vec2( 0.0, 0.6 )
);
const vec3 col[3] = vec3[3] (
    vec3( 1.0, 0.0, 0.0 ),
    vec3( 0.0, 1.0, 0.0 ),
    vec3( 0.0, 0.0, 1.0 )
);
void main() {
    gl_Position = vec4( pos[gl_VertexID], 0.0, 1.0 );
    color = col[ gl_VertexID ];
}
)";

/*
Fragment Shader
The location value of Fragment Shader should match the
corresponding location value in the Vertex Shader
*/
static const char* shaderFragmentCode = R"(
    #version 460 core
    layout (location = 0) in vec3 color;
    layout (location = 0) out vec4 out_FragColor;
    void main() {
        out_FragColor = vec4( color, 1.0 );
    }
)";

using glm::mat4;
using glm::vec3;

int main() {
    glfwSetErrorCallback(
        [] ( int error, const char* description ) {
            fprintf( stderr, "Error: %s\n", description);
        }
    );

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    // Using OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        1024, 768, "Computer Graphics", nullptr, nullptr
    );

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(
        window,
        [] (GLFWwindow* window, int key, int scancode, int action, int mode) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        }
    );

    // Creating GLFW context using GLAD
    glfwMakeContextCurrent( window );
    gladLoadGLLoader ((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval ( 1 );

    // Making a triangle
    GLuint VAO;
    glCreateVertexArrays ( 1, &VAO );
    glBindVertexArray ( VAO );

    // Compiling and linking the shader to the program
    const GLuint shaderVertex =
        glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (
        shaderVertex, 1, &shaderVertexCode, nullptr
    );

    glCompileShader (shaderVertex);

    const GLuint shaderFragment =
        glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (
        shaderFragment, 1, &shaderFragmentCode, nullptr
    );

    glCompileShader (shaderFragment);

    // Creating a program for attaching and rendering the shaders
    const GLuint program = glCreateProgram();

    glAttachShader ( program, shaderVertex );
    glAttachShader ( program, shaderFragment );
    glLinkProgram ( program );
    glUseProgram ( program );

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
        glViewport( 0, 0, width, height );
        glClear( GL_COLOR_BUFFER_BIT );

        // We can also pass the empty VAO object we created earlier
        glDrawArrays( GL_TRIANGLES, 0, 3 );

        /*
        The fragment shader output has been rendered into the back buffer.
        We swap the front and back buffers to make the triangle visible.
        Do not forget to poll the events (Otherwise the loop will never end)
        */
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glDeleteProgram ( program );
    glDeleteShader ( shaderFragment );
    glDeleteShader ( shaderVertex );
    glDeleteVertexArrays ( 1, &VAO );
    glfwDestroyWindow ( window );
    glfwTerminate ();
    return 0;
}
