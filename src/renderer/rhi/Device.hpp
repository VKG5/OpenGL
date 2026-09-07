#pragma once

#include <memory>
#include <cstdint>

// Device is the factory for every RHI resource. Its signatures use enums and
// description structs BY VALUE or BY CONST REFERENCE, so the headers that
// define them must be included here — forward declarations are not enough
// for by-value parameters (the compiler needs the complete type to lay out
// the call frame).
//
// The remaining classes appear only as unique_ptr RETURN types, which work
// with incomplete types in declarations: destruction happens in the caller's
// translation unit, where the type is complete.
#include "renderer/rhi/Buffer.hpp"      // BufferUsage
#include "renderer/rhi/Texture.hpp"     // TextureFormat
#include "renderer/rhi/Sampler.hpp"     // SamplerDesc
#include "renderer/rhi/Shader.hpp"      // ShaderStage
#include "renderer/rhi/Pipeline.hpp"    // PipelineDesc

namespace yumi::rhi {
    // Forward Declarations — Device creates these, doesn't own them after return
    class TextureView;
    class RenderTarget;
    class CommandBuffer;
    class CommandQueue;
    class Fence;
    class Semaphore;

    class Device{
    public:
        virtual ~Device() = default;

        // Resource creation -> each returns a unique_ptr documenting ownership:
        // the caller owns the resource; the device does not hold it after creation.
        // TODO : Replace unique_ptr with generational handles.
        virtual std::unique_ptr<Buffer> createBuffer(size_t bytes,
                                                     BufferUsage usage) = 0;

        virtual std::unique_ptr<Texture> createTexture(uint32_t width,
                                                       uint32_t height,
                                                       TextureFormat format,
                                                       const void* data) = 0;

        virtual std::unique_ptr<Sampler> createSampler(const SamplerDesc& desc) = 0;

        virtual std::unique_ptr<TextureView> createTextureView(const Texture& texture) = 0;

        virtual std::unique_ptr<Shader> createShader(const ShaderStage* stages,
                                                     const void** bytecodes,
                                                     const size_t* sizes,
                                                     uint32_t count) = 0;

        virtual std::unique_ptr<Pipeline> createPipeline(const PipelineDesc& desc) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(uint32_t width,
                                                                 uint32_t height) = 0;

        virtual std::unique_ptr<CommandBuffer> createCommandBuffer() = 0;
        virtual std::unique_ptr<CommandQueue> createCommandQueue() = 0;
        virtual std::unique_ptr<Fence> createFence(bool startSignaled = false) = 0;
        virtual std::unique_ptr<Semaphore> createSemaphore() = 0;
    };
} // namespace yumi::rhi