#pragma once

#include <cstddef>
#include <cstdint>

namespace yumi::rhi {
    // Forward Declaration - CommandBuffer doesn't own any of these
    class Pipeline;
    class Buffer;
    class Texture;
    class Sampler;
    class RenderTarget;

    class CommandBuffer {
    public:
        virtual ~CommandBuffer() = default;

        // What it does: begin recording commands into this buffer.
        // Why: OpenGL has no recording concept — commands execute immediately.
        // In Vulkan, vkBeginCommandBuffer must be called before any commands.
        // This method marks the start of a recording session.
        virtual void begin() = 0;

        // What it does: stop recording. The buffer is now ready for submission.
        // Why: pairs with begin(). The backend finalizes the command stream.
        virtual void end() = 0;

        // What it does: begin rendering to a specific render target.
        // Why: replaces glClear + implicit default framebuffer. In the GL code,
        // Scene::generalElements calls glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        // before drawing — this method makes "what am I drawing to?" explicit.
        // The render target's load op (clear/preserve) determines the clear behavior.
        virtual void beginRenderPass(RenderTarget& target) = 0;

        // What it does: end the current render pass.
        // Why: Vulkan render passes have explicit begin/end scope. The backend
        // resolves tile-based rendering, framebuffer transitions, etc. here.
        virtual void endRenderPass() = 0;

        // What it does: select which pipeline (shader + fixed-function state) to use.
        // Why: replaces glUseProgram + all the glEnable/glBlendFunc/glCullFace calls
        // that were scattered through the render pass. One bind → all state set.
        virtual void bindPipeline(const Pipeline& pipeline) = 0;

        // What it does: bind a buffer as the source of vertex data.
        // Why: replaces glBindBuffer(GL_ARRAY_BUFFER, VBO) + glBindVertexArray(VAO).
        // The vertex layout (stride, attribute offsets) is described by the pipeline's
        // shader reflection, not stored in a VAO.
        virtual void bindVertexBuffer(const Buffer& buffer) = 0;

        // What it does: bind a buffer as the source of index data.
        // Why: replaces glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO).
        virtual void bindIndexBuffer(const Buffer& buffer) = 0;

        // What it does: bind a texture + sampler to a slot for subsequent draw calls.
        // Why: replaces glActiveTexture(GL_TEXTURE0 + unit) + glBindTexture +
        // Shader::setTexture. The slot number corresponds to the shader's declared
        // binding point (what was previously a manual uniform location).
        virtual void bindTexture(const Texture& texture,
                                 const Sampler& sampler,
                                 uint32_t slot) = 0;
        
        // What it does: push a small block of data (e.g. a model matrix) directly
        // into the shader. Replaces glUniformMatrix4fv(uniformModel, ...) per object.
        // Why: per-object data changes every draw call; push constants are the
        // fastest way to get small amounts of data into the shader (limited to
        // ~128-256 bytes depending on the GPU, but a mat4 is only 64 bytes).
        virtual void pushConstants(uint32_t offset,
                                   const void* data,
                                   size_t size) = 0;
        
        // What it does: issue an indexed draw call.
        // Why: replaces glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0).
        // indexCount = number of indices to read (what was numOfIndices in Mesh).
        // firstIndex = offset into the index buffer (0 for most draws).
        // instanceCount = number of instances (1 for non-instanced draws;
        // Future: the PCG city loop's N buildings could become one instanced draw).
        virtual void drawIndexed(uint32_t indexCount,
                                 uint32_t firstIndex = 0,
                                 uint32_t instanceCount = 1) = 0;
        
        // What it does: set the viewport rectangle.
        // Why: replaces glViewport(0, 0, bufferWidth, bufferHeight). In the GL code,
        // viewport is set once at init; in Vulkan it's dynamic state set per-pass.
        virtual void setViewport(float x, float y,
                                 float width, float height) = 0;
    };
    
} // namespace yumi::rhi
