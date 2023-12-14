#pragma once

#include<stdio.h>
#include<string>
#include<iostream>
#include<fstream>

#include <GLFW/glfw3.h>

class Shader {
private:
    GLuint shaderID, uniformProjection, uniformModel;

    void compileShader(const char* vertexCode, const char* fragmentCode);
    void addShader(GLuint program, const char* shaderCode, GLenum shaderType);

public:
    Shader();

    void createFromString(const char* vertexCode, const char* fragmentCode);

    GLuint getProjectionLocation();
    GLuint getModelLocation();

    void useShader();
    void cleanShader();

    ~Shader();
};
