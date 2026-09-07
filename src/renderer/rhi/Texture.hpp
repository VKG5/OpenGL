#pragma once

#include <cstdint>

namespace yumi::rhi {
    enum class TextureFormat : uint8_t {
        RGB8,           // 3-channel 8-bit (some JPEGs, old GL code path)
        RGBA8,          // 4-channel 8-bit (most PNGs, the dominant format in the codebase)
        Depth32F        // 32-bit float depth (shadow maps, Base app FBO)
    };

    class Texture{
    public:
        virtual ~Texture() = default;

        // What it does: allocate and upload image data to the GPU.
        // Why: Texture::loadTexture calls glGenTextures + glTexImage2D —
        // this bundles both into one operation. The renderer passes CPU pixel
        // data; the backend decides where it lands in GPU memory.
        virtual void create(uint32_t width, uint32_t height,
                            TextureFormat format, const void* data) = 0;
        
        // What they do: report dimensions and format.
        // Why: validation and render-target sizing. The renderer needs to know
        // "this shadow map is 1024x1024" without asking the backend how it stored it.
        virtual uint32_t width() const = 0;
        virtual uint32_t height() const = 0;
        virtual TextureFormat format() const = 0;
    };
} // namespace yumi::rhi