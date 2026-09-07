#pragma once

#include <cstdint>

namespace yumi::rhi {
    enum class FilterMode : uint8_t {
        Nearest,                    // GL_NEAREST
        Linear,                     // GL_LINEAR
        NearestMipmapNearest,       // GL_NEAREST_MIPMAP_NEAREST
        LinearMipmapNearest,        // GL_LINEAR_MIPMAP_NEAREST
        LinearMipmapLinear          // GL_LINEAR_MIPMAP_LINEAR (trilinear — used as GL default)
    };

    enum class WrapMode : uint8_t {
        Repeat,             // GL_REPEAT (the GL default for this codebase)
        ClampToEdge,        // GL_CLAMP_TO_EDGE
        ClampToBorder       // GL_CLAMP_TO_BORDER
    };

    // Structs map naturally to Vulkan's `VkSamplerCreateInfo`
    // Also consolidated presentation of information
    struct SamplerDesc {
        FilterMode minFilter = FilterMode::LinearMipmapLinear;
        FilterMode magFilter = FilterMode::Linear;
        WrapMode wrapU       = WrapMode::Repeat;
        WrapMode wrapV       = WrapMode::Repeat;
    };

    class Sampler {
    public:
        virtual ~Sampler() = default;
        
        // What it does: create the GPU sampler object from a description.
        // Why: glTexParameteri calls are scattered inside Texture::loadTexture —
        // pulling them out into a structured description means the renderer can
        // create samplers once and reuse them across textures. The struct is
        // passed by const-ref so the backend copies what it needs.
        virtual void create(const SamplerDesc& desc) = 0;
    };
} // namespace yumi::rhi