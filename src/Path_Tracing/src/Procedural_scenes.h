#pragma once

// GLM Files - Math Library
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Scene Header for operations
#include "Scene.h"
#include "Utilities.h"

using namespace Scene;

void generateRandomSpheres(int numSpheres, float sceneRadius) {
    // Place sphere at a random location within a grid pattern with random material properties for each based on the seed
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);

    // Radius between 0.05 and 0.2 units
    std::uniform_real_distribution<float> radiusDist(0.05, 0.2);

    for (int i = 0; i < numSpheres; i++) {
        float radius = radiusDist(gen);
        glm::vec3 candidatePos(dist(gen) * sceneRadius, dist(gen) * sceneRadius, dist(gen) * sceneRadius);
        float x = candidatePos.x;
        float y = candidatePos.y;
        float z = candidatePos.z;

        bool collision = false;
        for (int j = 0; j < i; j++) {
            if (glm::distance(candidatePos, glm::vec3(objects[j].position[0], objects[j].position[1], objects[j].position[2])) < (radius + objects[j].scale[0])) {
                collision = true;
                break;
            }
        }

        if (!collision) {
            Material mat(
                {dist(gen), dist(gen), dist(gen)},  // Random albedo
                {dist(gen), dist(gen), dist(gen)},  // Random specular
                {0.0F, 0.0F, 0.0F},
                0.0F,
                dist(gen),                          // Random roughness
                0.0F,
                0.0F
            );

            objects.push_back(Object(2, {x, y, z}, {radius, radius, radius}, mat));
        }

        else {
            // Decrement to retry this index with a new position
            --i;
        }
    }

    // Adding a central light source or a reference sphere at the center
    objects.push_back(Object(1, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, Material({1.0f, 1.0f, 1.0f})));
    lights.push_back(PointLight({0.0f, 5.0f, 0.0f}, 0.5f, {1.0f, 1.0f, 1.0f}, 1.0f, 100.0f));

    // Flag for turning on/off the floor
    planeVisible = false;
}

void placeMirrorSpheres() {
    // Places a grid of reflective sphere in the scene with the same materials
	for (int i = -4; i <= 3; i++) {
		for (int j = -4; j <= 3; j++) {
			objects.push_back(  Object(1, {(float)i, 1.0F, (float)j},
                                {0.5F, 0.5F, 0.5F},
                                Material({0.0F, 0.0F, 0.0F},
                                {1.0F, 1.0F, 1.0F},
                                {0.0F, 0.0F, 0.0F},
                                0.0F,
                                0.2F,
                                0.0F,
                                0.0F
            )));
		}
	}

	planeMaterial = Material(
        { 1.0F, 1.0F, 1.0F },
        { 0.75F, 0.75F, 0.75F },
        { 0.0F, 0.0F, 0.0F },
        0.0F,
        0.0F,
        0.0F,
        0.0F
    );

	lights.push_back(PointLight({ 0.0F, 5.0F, 0.0F }, 0.5F, { 1.0F, 1.0F, 1.0F }, 1.0F, 100.0F));

    // Flag for turning on/off the floor
	planeVisible = true;
}

void placeBasicScene() {
    // Places one sphere at the center of an infintely long plane for the floor with a skybox
	objects.push_back(Object(1, { 0.0F, 0.5F, 0.0F }, { 0.5F, 0.5F, 0.5F }, Material({ 1.0F, 1.0F, 1.0F }, { 0.0F, 0.0F, 0.0F }, { 0.0F, 0.0F, 0.0F }, 0.0F, 1.0F, 0.0F, 0.0F)));

	planeMaterial = Material(
        { 1.0F, 1.0F, 1.0F },
        { 0.75F, 0.75F, 0.75F },
        { 0.0F, 0.0F, 0.0F },
        0.0F,
        0.0F,
        0.0F,
        0.0F
    );

	lights.push_back(PointLight({ 0.0F, 5.0F, 0.0F }, 0.5F, { 1.0F, 1.0F, 1.0F }, 1.0F, 100.0F));

    // Flag for turning on/off the floor
	planeVisible = true;
}

void placeCornellBoxScene() {
    // Define materials
    // The material properties are as following:
    // Albedo Color (Array), Specular Highlight Color (Array), Emission Color (Array)
    // Emission Strength, Roughness, Specular Highlight, Specular Exponent
    Material white(
        {1.0F, 1.0F, 1.0F},
        {0.0F, 0.0F, 0.0F},
        {0.0F, 0.0F, 0.0F},
        0.0F,
        1.0F,
        0.0F,
        0.0F
    );

    Material grey (
        {0.5F, 0.5F, 0.5F},   // Albedo
        {0.1F, 0.1F, 0.1F},   // Specular Color
        {0.0F, 0.0F, 0.0F},   // Emission Color
        0.0F,                 // Emission strength
        1.0F,                 // Roughness
        0.0F,                 // Specular highlight
        1.0F                  // Specular exponent
    );

    Material red(
        {1.0F, 0.0F, 0.0F},
        {0.0F, 0.0F, 0.0F},
        {0.0F, 0.0F, 0.0F},
        0.0F,
        1.0F,
        0.0F,
        0.0F
    );

    Material green(
        {0.0F, 1.0F, 0.0F},
        {0.0F, 0.0F, 0.0F},
        {0.0F, 0.0F, 0.0F},
        0.0F,
        1.0F,
        0.0F,
        0.0F
    );

    Material reflective(
        {1.0F, 1.0F, 1.0F},
        {1.0F, 1.0F, 1.0F},
        {0.0F, 0.0F, 0.0F},
        0.0F,
        1.0F,
        0.0F,
        0.0F
    );

    // Define objects
    // For objects the properties are as following:
    // Type of Object, Position, Scale
    // Back wall
    objects.push_back(Object(2, {0.0F, 0.5F, -1.0F}, {2.0F, 1.0F, 0.1F}, white));

    // Left wall (green)
    objects.push_back(Object(2, {-1.0F, 0.5F, 0.0F}, {0.1F, 1.0F, 2.0F}, green));

    // Right wall (red)
    objects.push_back(Object(2, {1.0F, 0.5F, 0.0F}, {0.1F, 1.0F, 2.0F}, red));

    // Floor
    objects.push_back(Object(2, {0.0F, 0.0F, 0.0F}, {2.0F, 0.1F, 2.0F}, white));

    // Ceiling
    objects.push_back(Object(2, {0.0F, 1.0F, 0.0F}, {2.0F, 0.1F, 2.0F}, white));

    // Sphere in the Cornell Box
    // For a sphere, only the X component is considered for scaling, as the radius
    objects.push_back(Object(1, {0.3F, 0.3F, 0.2F}, {0.25F, 0.4F, 0.4F}, reflective));

    // Another cube in the Cornell Box
    objects.push_back(Object(2, {-0.3F, 0.35F, -0.5F}, {0.3F, 0.6F, 0.3F}, white));

    // Light source as a small cube or rectangle on the ceiling
    // The parameters for light are as follows:
    // Position, Radius of the point light, Color, Power and Reach (Effective area)
    lights.push_back(PointLight({0.0F, 0.9F, 0.0F}, 0.1F, {1.0F, 1.0F, 1.0F}, 1.0F, 50.0F));

    // Flag for turning on/off the floor
	planeVisible = false;
}
