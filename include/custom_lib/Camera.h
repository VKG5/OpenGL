#pragma once

// Always include GLFW after GLAD - Core Libraries
#include <glad.h>
#include <glfw/glfw3.h>

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    // Global up
    glm::vec3 worldUp;

    GLfloat roll;
    GLfloat pitch;
    GLfloat yaw;

    GLfloat moveSpeed;
    GLfloat turnSpeed;

    void update();

public:
    // Constructor
    Camera();

    // Yaw - Front, Pitch - Right
    Camera( glm::vec3 initialPosition, glm::vec3 initialUp, GLfloat initialYaw,
            GLfloat initialPitch, GLfloat initialTurnSpeed, GLfloat initialMoveSpeed );

    void keyControl(bool* keys, GLfloat deltaTime);
    void mouseControl(GLfloat xChange, GLfloat yChange);

    // Getter for specular highlight data
    glm::vec3 getCameraPosition();

    glm::mat4 calculateViewMatrix();

    // Destructor
    ~Camera();
};
