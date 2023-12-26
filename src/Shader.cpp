#include "Shader.h"

// Constructor
Shader::Shader() {
    shaderID = 0;
    uniformModel = 0;
    uniformProjection = 0;
    uniformView = 0;

    pointLightCount = 0;
    spotLightCount = 0;
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode) {
    compileShader(vertexCode, fragmentCode);
}

void Shader::validate() {
    // Creating this for logging/debugging
    GLint result = 0;
    GLchar eLog[1024] = {0};

    // Checking if valid in current context
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);

    if(!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cout<<"Error validating program: "<<eLog<<"\n";
        return;
    }
}

void Shader::compileProgram() {
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

    // Name in the vertex shader
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformModel = glGetUniformLocation(shaderID, "model");

    // Getting the locations for the particular "directionalLight" light
    // Ambient Light
    uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");

    // Diffuse Light
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");

    // Specular Light
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

    // Point Light
    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

    for(size_t i = 0; i<MAX_POINT_LIGHTS; i++) {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].base.colour", i);
        uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLight[%zd].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
    }

    // Spot Light
    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLight[%zd].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
	}

    for(size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++) {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%zd].shadowMap", i);
        uniformOmniShadowMap[i].shadowMap = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "omniShadowMaps[%zd].farPlane", i);
        uniformOmniShadowMap[i].farPlane = glGetUniformLocation(shaderID, locBuff);
    }

    // Shadows
    uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
    uniformTexture = glGetUniformLocation(shaderID, "theTexture");
    uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

    // Omni Directional Shadows
    uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
    uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");

    for(size_t i = 0; i < 6; i++) {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "lightMatrices[%zd]", i);
        uniformLightMatrices[i] = glGetUniformLocation(shaderID, locBuff);
    }
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

    compileProgram();
}

void Shader::compileShader(const char * vertexCode, const char * fragmentCode, const char * geometryCode) {
    // Creating the program to run the shaders on the GPU
    shaderID = glCreateProgram();

    if(!shaderID) {
        std::cout<<"Error creating shader program";
        return;
    }

    // Geometry shader is between the vertex and fragment shader
    // Hence compiling/adding before fragment and after vertex
    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    compileProgram();
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

void Shader::createFromFiles(const char * vertexLocation, const char * fragmentLocation, const char * geometryLocation) {
    std::string vertexString = readFile(vertexLocation);
    std::string fragmentString = readFile(fragmentLocation);
    std::string geometryString = readFile(geometryLocation);

    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();
    const char* geometryCode = geometryString.c_str();

    compileShader(vertexCode, fragmentCode, geometryCode);
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
    return uniformDirectionalLight.uniformColour;
}

// Getter for Ambient Intensity
GLuint Shader::getAmbientIntensityLocation() {
    return uniformDirectionalLight.uniformAmbientIntensity;
}

// Getter for Diffuse Intensity
GLuint Shader::getDiffuseIntensityLocation() {
    return uniformDirectionalLight.uniformDiffuseIntensity;
}

// Getter for Light Direction
GLuint Shader::getDiffuseDirectionLocation() {
    return uniformDirectionalLight.uniformDirection;
}

// Getter for Specular Intensity
GLuint Shader::getSpecularIntensityLocation() {
    return uniformSpecularIntensity;
}

// Getter for Shininess
GLuint Shader::getShininessLocation() {
    return uniformShininess;
}

// Getter for Eye Position
GLuint Shader::getEyePositionLocation() {
    return uniformEyePosition;
}

// Getter for Omni Directional Light Position
GLuint Shader::getOmniLightPosLocation() {
    return uniformOmniLightPos;
}

// Getter for Omni directional Far Plane
GLuint Shader::getFarPlaneLocation() {
    return uniformFarPlane;
}

void Shader::setDirectionalLight(DirectionalLight * dLight) {
    dLight->useLight( uniformDirectionalLight.uniformAmbientIntensity,
                      uniformDirectionalLight.uniformColour,
                      uniformDirectionalLight.uniformDiffuseIntensity,
                      uniformDirectionalLight.uniformDirection );
}

void Shader::setPointLight(PointLight * pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {
    // Limiting number of point lights due to shader structure
    if(lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;

    glUniform1i(uniformPointLightCount, lightCount);

    for(size_t i = 0; i < lightCount; i++) {
        pLight[i].useLight( uniformPointLight[i].uniformAmbientIntensity,
                            uniformPointLight[i].uniformColour,
                            uniformPointLight[i].uniformDiffuseIntensity,
                            uniformPointLight[i].uniformPosition,
                            uniformPointLight[i].uniformConstant,
                            uniformPointLight[i].uniformLinear,
                            uniformPointLight[i].uniformExponent );

        // Iterating over the Textures assigned
        pLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);

        // For iterating over the i + pointLightCount functionality in our shader
        glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowMap[i + offset].farPlane, pLight[i].getFarPlane());
    }
}

void Shader::setSpotLight(SpotLight * sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) {
    // Limiting number of point lights due to shader structure
    if(lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

    glUniform1i(uniformSpotLightCount, lightCount);

    for(size_t i = 0; i < lightCount; i++) {
        sLight[i].useLight( uniformSpotLight[i].uniformAmbientIntensity,
                            uniformSpotLight[i].uniformColour,
                            uniformSpotLight[i].uniformDiffuseIntensity,
                            uniformSpotLight[i].uniformPosition,
                            uniformSpotLight[i].uniformDirection,
                            uniformSpotLight[i].uniformConstant,
                            uniformSpotLight[i].uniformLinear,
                            uniformSpotLight[i].uniformExponent,
                            uniformSpotLight[i].uniformEdge );

        // Iterating over the Textures assigned
        sLight[i].getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);

        // For iterating over the i + pointLightCount functionality in our shader
        glUniform1i(uniformOmniShadowMap[i + offset].shadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowMap[i + offset].farPlane, sLight[i].getFarPlane());
    }
}

void Shader::setTexture(GLuint textureUnit) {
    glUniform1i(uniformTexture, textureUnit);
}

void Shader::setDirectionalShadowMap(GLuint textureUnit) {
    glUniform1i(uniformDirectionalShadowMap, textureUnit);
}

void Shader::setDirectionalLightTransform(glm::mat4 * lTransform) {
    glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));
}

void Shader::setLightMatrices(std::vector<glm::mat4> lightMatrices) {
    for(size_t i = 0; i < 6; i++) {
        glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
    }
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
