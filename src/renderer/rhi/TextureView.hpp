#pragma once

namespace yumi::rhi {
    // Forward Declaration - TextureView references a texture, doesn't own it
    class Texture;

    class TextureView {
    public:
        virtual ~TextureView() = default;

        // What it does: create a view into an existing texture.
        // Why: cubemap faces (skybox), individual mip levels, and render-target
        // attachments all need "a way to look at part of a texture." The backend
        // maps this to VkImageView; in GL it's a conceptual no-op (the texture
        // itself serves as its own view), but the interface must exist so renderer
        // code doesn't assume whole-texture access.
        virtual void create(const Texture& texture) = 0;
    };    
} // namespace yumi::rhi