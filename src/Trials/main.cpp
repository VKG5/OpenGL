#include <iostream>

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <glfw/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>
// Including this file to prevent ImGui::showDemoWindow() error
#include "imgui_demo.cpp"

// C++ libraries for accessing time
#include <chrono>
#include <thread>

using glm::mat4;

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

		layout (location = 0) in vec2 Position;
		layout (location = 1) in vec2 UV;
		layout (location = 2) in vec4 Color;

		layout(std140, binding = 0) uniform PerFrameData
		{
			uniform mat4 MVP;
		};

		out vec2 Frag_UV;
		out vec4 Frag_Color;

		void main()
		{
			Frag_UV = UV;
			Frag_Color = Color;
			gl_Position = MVP * vec4(Position.xy,0,1);
		}
	)";

/*
Fragment Shader
The location value of Fragment Shader should match the
corresponding location value in the Vertex Shader
*/
static const char* shaderCodeFragment = R"(
		#version 460 core

		in vec2 Frag_UV;
		in vec4 Frag_Color;

		layout (binding = 0) uniform sampler2D Texture;
		layout (location = 0) out vec4 Out_Color;

		void main()
		{
			Out_Color = Frag_Color * texture(Texture, Frag_UV.st);
		}
	)";

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
        1024, 768, "ImGui", nullptr, nullptr
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

    // Cursor Position Callback
    glfwSetCursorPosCallback( window,
        [] (auto* window, double x, double y){
            ImGui::GetIO().MousePos = ImVec2(x, y);
        }
    );

    // Mouse Position Callback
    glfwSetMouseButtonCallback( window,
        [] (auto* window, int button, int action, int mods){
            auto& io = ImGui::GetIO();
            int idx = button == GLFW_MOUSE_BUTTON_LEFT ? 0 : button == GLFW_MOUSE_BUTTON_RIGHT ? 2 : 1;
            io.MouseDown[idx] = action == GLFW_PRESS;
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

    // Creating an index buffer - Vertex Buffer Object
    GLuint handleVBO;
    glCreateBuffers(1, &handleVBO);

    // Using upper limit of 256 kilobytes (256 * 1 kilobyte = 1024 bytes)
    glNamedBufferStorage(
        handleVBO, 256 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT
    );

    GLuint handleElements;
    glCreateBuffers(1, &handleElements);

    glNamedBufferStorage(
        handleElements, 256 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT
    );

    // Configuring Vector attributes
    glVertexArrayElementBuffer(VAO, handleElements);

    // ImDrawVert - Structure, part of ImGui
    /*
    struct ImDrawVert {
        ImVec2 pos;
        ImVec2 uv;
        ImU32 col;
    };
    */
    glVertexArrayVertexBuffer(
        VAO, 0, handleVBO,0, sizeof(ImDrawVert)
    );
    glEnableVertexArrayAttrib(
        VAO, 0
    );
    glEnableVertexArrayAttrib(
        VAO, 1
    );
    glEnableVertexArrayAttrib(
        VAO, 2
    );

    // Setting up the vertex attributes
    glVertexArrayAttribFormat(
        VAO, 0, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, pos)
    );
    glVertexArrayAttribFormat(
        VAO, 1, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, uv)
    );
    glVertexArrayAttribFormat(
        VAO, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, IM_OFFSETOF(ImDrawVert, col)
    );

    // Binding the Attributes and Vertex Array Object
    glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribBinding(VAO, 2, 0);

    glBindVertexArray ( VAO );

    // Compiling and Linking to a Shader Program
    const GLuint handleVertex =
        glCreateShader ( GL_VERTEX_SHADER );

    glShaderSource (
        handleVertex, 1, &shaderCodeVertex, nullptr
    );

    glCompileShader (
        handleVertex
    );

    const GLuint handleFragment =
        glCreateShader ( GL_FRAGMENT_SHADER );

    glShaderSource (
        handleFragment, 1, &shaderCodeFragment, nullptr
    );

    glCompileShader (
        handleFragment
    );

    // Creating a program for attaching and rendering the shaders
    const GLuint program = glCreateProgram();

    glAttachShader ( program, handleVertex );
    glAttachShader ( program, handleFragment );
    glLinkProgram ( program );
    glUseProgram ( program );

    /*
    We will use the Direct State Access (DSA) methods from OpenGL
    Instead of classic bind-to-edit approach
    We will also be using the Unifrom Buffer for this
    */
    const GLsizeiptr kBufferSize = sizeof(mat4);
    GLuint perFrameDataBuf;
    glCreateBuffers( 1, &perFrameDataBuf );
    glNamedBufferStorage( perFrameDataBuf, kBufferSize, nullptr, GL_DYNAMIC_STORAGE_BIT );
    glBindBufferBase( GL_UNIFORM_BUFFER, 0, perFrameDataBuf );

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // To tell ImGui to render more than 65,535 vertices with 16-bit indices
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    // Creating Texture Atlas for font rendering
    ImFontConfig cfg = ImFontConfig();

    // Managing memory manually for now
    cfg.FontDataOwnedByAtlas = false;

    // Brighten up font
    cfg.RasterizerMultiply = 1.5f;

    // Pixel Height of font
    cfg.SizePixels = 768.0f / 32.0f;

    // Limit every glyph to pixel boundary and rasterize them at higher quality for sub-pixel plotting
    cfg.PixelSnapH = true;
    cfg.OversampleH = 4;
    cfg.OversampleV = 4;

    ImFont* Font = io.Fonts->AddFontFromFileTTF(
        "C:/Users/hkshi/Downloads/Fonts/monterchi/Monterchi-Book-trial.ttf",
        cfg.SizePixels,
        &cfg
    );

    // Font atlas bitmap from ImGui in 32-bit format
    unsigned char* pixels = nullptr;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(
        &pixels, &width, &height
    );

    // Texture creation
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureParameteri(
        texture, GL_TEXTURE_MAX_LEVEL, 0
    );
	glTextureParameteri(
        texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR
    );
	glTextureParameteri(
        texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR
    );
	glTextureStorage2D(
        texture, 1, GL_RGBA8, width, height
    );
	glPixelStorei(
        GL_UNPACK_ALIGNMENT, 1
    );
	glTextureSubImage2D(
        texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels
    );
	glBindTextures(
        0, 1, &texture
    );

    // Passing handle to ImGui
	io.Fonts->TexID = (ImTextureID)(intptr_t)texture;
	io.FontDefault = Font;
	io.DisplayFramebufferScale = ImVec2(1, 1);

    /*
    GLFW Main Loop and Rendering
    */
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    bool _showDemoWindow = true;

    // Set your desired target frame rate (e.g., 60 FPS)
    const int targetFPS = 60;
    const std::chrono::milliseconds frameTime(1000 / targetFPS);

	while (!glfwWindowShouldClose(window))
	{
        // Limiting the framerate to prevent flickering
        auto frameStart = std::chrono::high_resolution_clock::now();

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

        // Starting a demo UI window
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)width, (float)height);
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(&_showDemoWindow);

        // Geometry data is generated in ImGui::Render()
        ImGui::Render();

        // Retrieving geometry data using GetDrawData()
		const ImDrawData* draw_data = ImGui::GetDrawData();

        // Othographic project matrix
		const float L = draw_data->DisplayPos.x;
		const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		const float T = draw_data->DisplayPos.y;
		const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		const mat4 orthoProjection = glm::ortho(L, R, B, T);

		glNamedBufferSubData(perFrameDataBuf, 0, sizeof(mat4), glm::value_ptr(orthoProjection));

        // Going thorugh the ImGui commands and updating the content of index and vertex buffers
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
            // Each list has vertex and index data associated with it
            // Updating the OpenGL buffers with the data
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			glNamedBufferSubData(handleVBO, 0, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), cmd_list->VtxBuffer.Data);
			glNamedBufferSubData(handleElements, 0, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), cmd_list->IdxBuffer.Data);

            // Rendering commands are stored within the command buffer
            // Iterating over them are rendering actual geometry
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				const ImVec4 cr = pcmd->ClipRect;
				glScissor((int)cr.x, (int)(height - cr.w), (int)(cr.z - cr.x), (int)(cr.w - cr.y));
				glBindTextureUnit(0, (GLuint)(intptr_t)pcmd->TextureId);
				glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT,
					(void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
			}
		}

        // glScissor(x, y, width, height)
        // x,y are the lower left coordinates
		glScissor(0, 0, width, height);

		glfwSwapBuffers(window);
		glfwPollEvents();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);

        // We are only going to update the UI at a certain interval, not every frame
        if (frameDuration < frameTime) {
            std::this_thread::sleep_for(frameTime - frameDuration);
        }
	}

	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
