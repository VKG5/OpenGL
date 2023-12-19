#pragma once
#include "Light.h"

// GLM Files - Math Library
#include <glm/gtc/matrix_transform.hpp>

class DirectionalLight : public Light {
private:
    glm::vec3 direction;

public:
    // Constructor
    DirectionalLight();

    DirectionalLight( GLuint shadowWidth, GLuint shadowHeight,
                      GLfloat red, GLfloat green, GLfloat blue,
                      GLfloat ambIntensity, GLfloat diffIntensity,
                      GLfloat xDir, GLfloat yDir, GLfloat zDir );

    // Function override for different types of light
    void useLight(  GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                    GLuint diffuseIntensityLocation, GLuint directionLocation  );

    glm::mat4 calculateLightTransform();

    // Destructor
    ~DirectionalLight();
};
