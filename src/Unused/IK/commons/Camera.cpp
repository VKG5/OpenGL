#include "Camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90;
    pitch = 0;

    front = glm::vec3(0.0f, 0.0f, 1.0f);

    moveSpeed = 5.0f;
    turnSpeed = 1.0f;

    // Projection
    FOV = 45.0f;
    scale = 7.135f;
    nearClipping = 0.1f;
    farClipping = 100.f;

    update();
}

Camera::Camera( glm::vec3 initialPosition, glm::vec3 initialUp, GLfloat initialYaw,
                GLfloat initialPitch, GLfloat initialTurnSpeed, GLfloat initialMoveSpeed ) {
    position = initialPosition;
    worldUp = initialUp;
    yaw = initialYaw;
    pitch = initialPitch;

    front = glm::vec3(0.0f, 0.0f, 1.0f);

    moveSpeed = initialMoveSpeed;
    turnSpeed = initialTurnSpeed;

    // Projection
    FOV = 45.0f;
    scale = 7.135f;
    nearClipping = 0.1f;
    farClipping = 100.f;

    update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime) {
    GLfloat velocity = moveSpeed * deltaTime;

    // Move Forward
    if(keys[GLFW_KEY_W]) {
        position += front * velocity;
    }

    // Move Backward
    if(keys[GLFW_KEY_S]) {
        position -= front * velocity;
    }

    // Move Right
    if(keys[GLFW_KEY_D]) {
        position += right * velocity;
    }

    // Move Left
    if(keys[GLFW_KEY_A]) {
        position -= right * velocity;
    }

    // Move Up
    if(keys[GLFW_KEY_Q]) {
        position += worldUp * velocity;
    }

    // Move Down
    if(keys[GLFW_KEY_E]) {
        position -= worldUp * velocity;
    }

    // Resetting to origin with default camera position
    if(keys[GLFW_KEY_0]) {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        pitch = 0.0f;
        yaw = -90.0f;
    }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    // Limiting the Pitch
    if(pitch > 89.0f) {
        pitch = 89.0f;
    }

    if(pitch < -89.0f) {
        pitch = -89.0f;
    }

    update();
}

// Updating the Up, Front, and Right
void Camera::update() {
    // Calculating the front of the camera
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

// Getters=============================================================================================================
glm::vec3 Camera::getCameraPosition() {
    return position;
}

glm::vec3 Camera::getCameraDirection() {
    return glm::normalize(front);
}

glm::vec3 Camera::getCameraDirectionMagnitude() {
    // Function to map values from [-1.0, 1.0] to [-360.0, 360.0]
    return glm::vec3( glm::clamp((front.x + 1.0f) * 360.0f, 0.0f, 360.0f),
                      glm::clamp((front.y + 1.0f) * 360.0f, 0.0f, 360.0f),
                      glm::clamp((front.z + 1.0f) * 360.0f, 0.0f, 360.0f) );
}

glm::mat4 Camera::calculateViewMatrix() {
    // Position, where we are look at from
    // Thing we are looking at
    // Where up is in the world
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::calculatePerspectiveProjectionMatrix(const GLint& width, const GLint& height) {
    return glm::perspective(glm::radians(FOV), GLfloat(width)/GLfloat(height), nearClipping, farClipping);
}

glm::mat4 Camera::calculateOrthographicProjectionMatrix() {
    return glm::ortho(-scale, scale, -scale, scale, nearClipping, farClipping);
}

// Setters=============================================================================================================
void Camera::setPosition(glm::vec3 currPos) {
    position = currPos;
}

// Changing Move Speed
void Camera::updateMoveSpeed(GLfloat newSpeed) {
    moveSpeed = newSpeed;
}

void Camera::setCameraParameters(float fieldOfView, float orthoScale, float near, float far) {
    FOV = fieldOfView;
    scale = orthoScale;
    nearClipping = near;
    farClipping = far;
}

Camera::~Camera() {
}
