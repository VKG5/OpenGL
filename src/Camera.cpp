#include "Camera.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90;
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

void Camera::keyControl(bool* keys, GLfloat deltaTime) {
    GLfloat velocity = moveSpeed * deltaTime;
    GLfloat speedUp = 5.0f;

    // Move Forward
    if(keys[GLFW_KEY_W]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position += front * velocity;
    }

    // Move Backward
    if(keys[GLFW_KEY_S]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position -= front * velocity;
    }

    // Move Right
    if(keys[GLFW_KEY_D]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position += right * velocity;
    }

    // Move Left
    if(keys[GLFW_KEY_A]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position -= right * velocity;
    }

    // Move Up
    if(keys[GLFW_KEY_Q]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position += worldUp * velocity;
    }

    // Move Down
    if(keys[GLFW_KEY_E]) {
        // Fast Move
        if(keys[GLFW_KEY_LEFT_SHIFT])  {
            velocity *= speedUp;
        }

        position -= worldUp * velocity;
    }

    // Resetting to origin with default camera position
    if(keys[GLFW_KEY_0]) {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        pitch = 0.0f;
        yaw = -90.0f;
    }
}

glm::mat4 Camera::calculateViewMatrix() {
    // Position, where we are look at from
    // Thing we are looking at
    // Where up is in the world
    return glm::lookAt(position, position + front, up);
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

Camera::~Camera() {
}
