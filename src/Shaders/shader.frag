#version 460 core

in vec4 col;
in vec2 texCoord;
in vec3 Normal;
in vec3 fragPos;
in vec4 directionalLightSpacePos;

out vec4 colour;

// Should be same as Utilities.h header file
const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
    vec3 colour;
    float ambientIntensity;
    float diffuseIntensity;
};

struct DirectionalLight {
    Light base;
    vec3 direction;
};

struct PointLight {
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
};

struct SpotLight {
    PointLight base;
    vec3 direction;
    float edge;
};

struct OmniShadowMap {
    samplerCube shadowMap;
    float farPlane;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

// Lights
uniform DirectionalLight directionalLight;
uniform PointLight pointLight[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

// Textures
uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap;
// Includes both points lights and spot lights
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

// Materials
uniform Material material;

// Eye Position
uniform vec3 eyePosition;

float calcDirectionalShadowFactor(DirectionalLight light) {
    vec3 projectionCoords = directionalLightSpacePos.xyz / directionalLightSpacePos.w;

    // Normalizing coordinates
    projectionCoords = (projectionCoords * 0.5) + 0.5;

    float current = projectionCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(directionalLight.direction);

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);

    // Getting a 3x3 frame around the pixel and averaging
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <=1; ++y) {
            float pcfDepth = texture(directionalShadowMap, projectionCoords.xy + vec2(x, y) * texelSize).r;
            shadow += current - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if(projectionCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

float calcOmniShadowFactor(PointLight light, int shadowIndex) {
    vec3 fragToLight = fragPos - light.position;

    // Getting the first value for closest object
    float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight).r;

    // Converting the normalized distance to normal
    closest *= omniShadowMaps[shadowIndex].farPlane;

    float current = length(fragToLight);

    float bias = 0.05;
    float shadow = current - bias > closest ? 1.0 : 0.0;

    return shadow;
}

vec4 calcLightByDirection(Light light, vec3 direction, float shadowFactor) {
    // Ambient Light
    vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;

    // Diffuse Light
    // Getting the cosine of angle between two vectors
    float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);
    vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);

    // Specular Light
    vec4 specularColour = vec4(0, 0, 0, 0);

    // If no diffuse, then no specular
    if(diffuseFactor > 0.0f) {
        vec3 fragToEye = normalize(eyePosition - fragPos);
        vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));

        float specularFactor = dot(fragToEye, reflectedVertex);

        if(specularFactor > 0.0f) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour));
}

vec4 calcDirectionalLight() {
    float shadowFactor = calcDirectionalShadowFactor(directionalLight);
    return calcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 calcPointLightsBase(PointLight pLight, int shadowIndex) {
    // Calculating Direction of our Point Lights
    // Getting direction from light to fragment
    vec3 direction = fragPos - pLight.position;
    float distance = length(direction);

    direction = normalize(direction);

    float shadowFactor = calcOmniShadowFactor(pLight, shadowIndex);

    vec4 colour = calcLightByDirection(pLight.base, direction, shadowFactor);

    float attenuation = pLight.exponent * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;

    return (colour / attenuation);
}

vec4 calcSpotLightsBase(SpotLight sLight, int shadowIndex) {
    // Getting Direction
    vec3 rayDirection = normalize(fragPos - sLight.base.position);
    float slFactor = dot(rayDirection, sLight.direction);
    vec4 colour = vec4(0, 0, 0, 0);

    // If we are withing range
    if(slFactor > sLight.edge) {
        colour = calcPointLightsBase(sLight.base, shadowIndex);
        if(colour != vec4(0, 0, 0, 0)) {
            // colour *= (1.0f - (1.0f - slFactor)*(1.0f/(1.0f - sLight.edge)));
            colour = vec4(1, 1, 1, 1) * (1.0f - (1.0f - slFactor) * (1.0f/(1.0f - sLight.edge)));
        }
    }

    return colour;
}

vec4 calcPointLights() {
    vec4 totalColour = vec4(0, 0, 0, 0);

    for(int i = 0; i < pointLightCount; i++) {
        totalColour += calcPointLightsBase(pointLight[i], i);
    }

    return totalColour;
}

vec4 calcSpotLights() {
    vec4 totalColour = vec4(0, 0, 0, 0);

    for(int i = 0; i < spotLightCount; i++) {
        // Hacky way for spotlights
        totalColour += calcSpotLightsBase(spotLight[i], i + pointLightCount);
    }

    return totalColour;
}

void main() {
    vec4 finalColour = calcDirectionalLight();
    finalColour += calcPointLights();
    finalColour += calcSpotLights();

    colour = texture(theTexture, texCoord) * finalColour;
}
