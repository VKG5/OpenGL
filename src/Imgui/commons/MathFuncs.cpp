#include "MathFuncs.h"

void printVec3(const glm::vec3& vector, const int& index) {
    printf("Vector components: (%.2f, %.2f, %.2f)\n", vector.x, vector.y, vector.z);
}

void printMat4(const glm::mat4& mat, const int& index) {
    printf("Matrix:\n");
    for (int row = 0; row < 4; row++) {
        // Indent for better readability
        printf("   ");
        for (int col = 0; col < 4; col++) {
            // Print element and a trailing space only after last column
            printf("%.2f%s", mat[col][row], (col == 3) ? "" : " ");
        }
        printf("\n");
    }
}
glm::vec3 invertAxis(const glm::vec3& originalVec, const int& axis) {
    if(axis == 0) {
        return glm::vec3(-originalVec.x, originalVec.y, originalVec.z);
    }

    if(axis == 1) {
        return glm::vec3(originalVec.x, -originalVec.y, originalVec.z);
    }

    if(axis == 2) {
        return glm::vec3(originalVec.x, originalVec.y, -originalVec.z);
    }

    // If none, return the original vector
    return originalVec;
}

float calcMagnitude(const glm::vec3& vecA) {
    return glm::length(vecA);
}

glm::vec3 calculateVector(const glm::vec3 & pointA, const glm::vec3 & pointB) {
    return glm::vec3(pointA - pointB);
}

float calculateAngleBetween(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3& origin) {
    return glm::acos(glm::dot(glm::normalize(vecA - vecB), glm::normalize(vecB - origin)));
}

glm::vec3 calculateRotationAxis(const glm::vec3& vecA, const glm::vec3& vecB, const glm::vec3& origin) {
    return glm::cross(glm::normalize(vecB - origin), glm::normalize(vecA - vecB));
}

float calcDifferenceMagnitude(const glm::vec3& vecA, const glm::vec3& vecB) {
    return glm::length(glm::abs(vecA - vecB));
}
