#include "OmniShadowMap.h"

OmniShadowMap::OmniShadowMap() : ShadowMap() {
}

bool OmniShadowMap::init(unsigned int width, unsigned int height) {
    shadowWidth = width; shadowHeight = height;

    glGenFramebuffers(1, &FBO);

    glGenTextures(1, &shadowMap);

    // Generating a CUBEMAP texture for omni-directional shadows
    // 6 sub-textures
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

    // Iterating over textures of cubemap
    for(size_t i = 0; i < 6; i++) {
        // All the sub-textures are in contiguous series
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    // For zooming out - Minify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // For zooming in - Magnify
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Setting parameter values
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // For z coordinate
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

    // Don't want to read any colour data
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Frame buffer error : %s\n", status);
        return false;
    }

    return true;
}

void OmniShadowMap::write() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void OmniShadowMap::read(GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
}

OmniShadowMap::~OmniShadowMap() {
}
