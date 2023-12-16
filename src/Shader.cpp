#include "Shader.h"

// Constructor
Shader::Shader() {
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
    uniformView = 0;
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode) {
    compileShader(vertexCode, fragmentCode);
}

void Shader::compileShader(const char* vertexCode, const char* fragmentCode) {
    // Creating the program to run the shaders on the GPU
    shaderID = glCreateProgram();

    if(!shaderID) {
        std::cout<<"Error creating shader program";
        return;
    }

    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    // Creating this for logging/debugging
    GLint result = 0;
    GLchar eLog[1024] = {0};

    glLinkProgram(shaderID);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);

    if(!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout<<"Error linking program: "<<eLog<<"\n";
        return;
    }

    // Checking if valid in current context
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

    if(!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout<<"Error validating program: "<<eLog<<"\n";
        return;
    }

    // Name in the vertex shader
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformModel = glGetUniformLocation(shaderID, "model");

    // Getting the locations for the particular "directionalLight" light
    // Ambient Light
    uniformAmbientColour = glGetUniformLocation(shaderID, "directionalLight.colour");
    uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.ambientIntensity");

    // Diffuse Light
    uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.diffuseIntensity");
}

void Shader::addShader(GLuint program, const char* shaderCode, GLenum shaderType) {
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

void Shader::createFromFiles(const char* vertexLocation, const char* fragmentLocation) {
    std::string vertexString = readFile(vertexLocation);
    std::string fragmentString = readFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    compileShader(vertexCode, fragmentCode);
}

std::string Shader::readFile(const char* filePath) {
    std::string content;

    // Source of the files
    std::fstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        printf("Failed to read '%s'! File doesn't exist.\n", filePath);
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

// Getter for Uniform Projection
GLuint Shader::getProjectionLocation() {
    return uniformProjection;
}

// Getter for Uniform Model
GLuint Shader::getModelLocation() {
    return uniformModel;
}

// Getter for Uniform View
GLuint Shader::getViewLocation() {
    return uniformView;
}

// Getter for Ambient Colour
GLuint Shader::getAmbientColourLocation() {
    return uniformAmbientColour;
}

// Getter for Ambient Intensity
GLuint Shader::getAmbientIntensityLocation() {
    return uniformAmbientIntensity;
}

// Getter for Light Direction
GLuint Shader::getDiffuseDirectionLocation() {
    return uniformDirection;
}

// Getter for Diffuse Intensity
GLuint Shader::getDiffuseIntensityLocation() {
    return uniformDiffuseIntensity;
}

void Shader::useShader() {
    if(shaderID) {
        glUseProgram(shaderID);
    }

    else {
        std::cout<<"Error in attaching shaderID "<<shaderID<<" to program";
    }
}

void Shader::cleanShader() {
    if(shaderID) {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
}

// Destructor
Shader::~Shader() {
    cleanShader();
}
