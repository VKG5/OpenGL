#include "Camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = 0;
    pitch = 0;

    front = glm::vec3(0.0f, 0.0f, 1.0f);

    moveSpeed = 5.0f;
    turnSpeed = 1.0f;

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

    update();
}

void Camera::keyControl(bool* keys) {
    // Move Forward
    if(keys[GLFW_KEY_W]) {
        position += front * moveSpeed;
    }

    // Move Backward
    if(keys[GLFW_KEY_S]) {
        position -= front * moveSpeed;
    }

    // Move Right
    if(keys[GLFW_KEY_D]) {
        position += right * moveSpeed;
    }

    // Move Left
    if(keys[GLFW_KEY_A]) {
        position -= right * moveSpeed;
    }
}

glm::mat4 Camera::calculateViewMatrix() {
    // Position, where we are look at from
    // Thing we are looking at
    // Where up is in the world
    return glm::lookAt(position, position + front, up);
}

void Camera::update() {
    // Calculating the front of the camera
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

Camera::~Camera() {
}
