#pragma once

// Always include GLFW after GLAD/GLEW - Core Libraries
#include <GL/glew.h>

// GLM Files - Math Library
#include <glm/gtc/matrix_transform.hpp>

// Basic C++ Libraries for various operations
#include <initializer_list>
#include <algorithm>
#include <vector>

namespace Scene {
	struct Material {
		float albedo[3];
		float specular[3];
		float emission[3];
		float emissionStrength;
		float roughness;
		float specularHighlight;
		float specularExponent;

		Material(const std::initializer_list<float>& color);
		Material(const std::initializer_list<float>& albedo, const std::initializer_list<float>& specular, const std::initializer_list<float>& emission, float emissionStrength, float roughness, float specularHighlight, float specularExponent);
		Material();
	};

	struct Object {
        // Currently there are 3 types of objects that can be rendered
        // Type 0 - None (invisible)
        // Type 1 - Sphere
        // Type 2 - Box
		unsigned int type;

        // XYZ positions for the objects in the world
		float position[3];

        // For Sphere, we only used the X value
		float scale[3];

        // Courrently supports - Roughness, Emission, Specular Intensity and Color
		Material material;

		Object(unsigned int type, const std::initializer_list<float>& position, const std::initializer_list<float>& scale, Material material);
		Object();
	};

	struct PointLight {
		float position[3];
		float radius;
		float color[3];
		float power;

        // Points within the reach of the light will be affected by it
		float reach;

		PointLight(const std::initializer_list<float>& position, float radius, const std::initializer_list<float>& color, float power, float reach);
		PointLight();
	};

    // Camera Settings
	extern glm::vec3 cameraPosition;
	extern float cameraYaw, cameraPitch;

    // Shader ID for the linked program
	extern GLuint shaderID;

    // List of objects in the scene for selection
	extern std::vector<Object> objects;

    // List of lights in the scene
	extern std::vector<PointLight> lights;

    // This is the material of the plane/floor that is present by default in the scene
	extern Material planeMaterial;
	extern int shadowResolution;
	extern int lightBounces;
	extern int framePasses;
	extern float blur;
	extern float bloomRadius;
	extern float bloomIntensity;
	extern float skyboxStrength;
	extern float skyboxGamma;
	extern float skyboxCeiling;
	extern int selectedObjectIndex;
	extern GLuint skyboxTexture;
	extern bool planeVisible;
    extern bool isRayTracing;

	void bind(GLuint shaderProgram);
	void unbind();
	void selectHovered(float mouseX, float mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPosition, glm::mat4 rotationMatrix);
	void mousePlace(float mouseX, float mouseY, int screenWidth, int screenHeight, glm::vec3 cameraPosition, glm::mat4 rotationMatrix);
}


