#pragma once
#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap {
private:

public:
    // Constructor
    OmniShadowMap();

    bool init(unsigned int width, unsigned int height);

    void write();

    void read(GLenum textureUnit);

    // Destructor
    ~OmniShadowMap();
};
