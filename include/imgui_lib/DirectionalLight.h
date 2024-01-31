#pragma once
#include "Light.h"

class DirectionalLight : public Light {
private:
    glm::vec3 direction;

public:
    // Constructor
    DirectionalLight();

    DirectionalLight( GLfloat red, GLfloat green, GLfloat blue,
                      GLfloat ambIntensity, GLfloat diffIntensity,
                      GLfloat xDir, GLfloat yDir, GLfloat zDir );

    // Function override for different types of light
    void useLight(  GLuint ambientIntensityLocation, GLuint ambientColourLocation,
                    GLuint diffuseIntensityLocation, GLuint directionLocation  );

    // Destructor
    ~DirectionalLight();
};
