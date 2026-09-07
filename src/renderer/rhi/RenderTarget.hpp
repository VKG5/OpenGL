#pragma once

#include <cstdint>

namespace yumi::rhi {
    // Forward Declaration - Render Targets reference Textures as attachments
    class Texture;

    class RenderTarget {
    public:
        virtual ~RenderTarget() = default;

        // What it does: create a render target with specified dimensions.
        // Why: ShadowMap::init allocates an FBO with width/height; the default
        // framebuffer is sized to the window. The backend allocates the
        // underlying image(s) and any necessary framebuffer objects.
        virtual void create(uint32_t width, uint32_t height) = 0;

        // What they do: report dimensions.
        // Why: validation and ensuring render targets match expected sizes
        // (e.g. shadow map resolution must match the light's shadow map dimensions).
        virtual uint32_t width() const = 0;
        virtual uint32_t height() const = 0;
    };
} // namespace yumi::rhi