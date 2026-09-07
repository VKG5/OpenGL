#pragma once

#include <cstdint>

namespace yumi::rhi {
    // Forward Declaration - Pipeline references a Shader, doesn't own it
    class Shader;

    enum class CompareOp : uint8_t {
        Never,
        Less,           // GL_LESS — the standard depth test (Base + Imgui apps)
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always          // GL_ALWAYS — used when depth test is disabled conceptually
    };

    enum class BlendMode : uint8_t {
        None,           // Opaque rendering (the dominant path in both apps)
        AlphaBlend,     // Standard src-alpha / one-minus-src-alpha
        Additive        // src-one / dst-one
    };

    enum class CullMode : uint8_t {
        None,           // No face culling
        Back,           // Back-face culling (GL default)
        Front
    };

    enum class FrontFace : uint8_t {
        CounterClockwise,   // GL default
        Clockwise
    };

    // Structs map sensible defaults, making them explicit and visible.
    // For example, `glEnable(GL_DEPTH_TEST)` and `glCullFace(GL_BACK)`
    // This struct captures all states that were previously mutable-global in OpenGL.
    struct PipelineDesc {
        const Shader* shader        = nullptr;
        bool depthTestEnable        = true;
        bool depthWriteEnable       = true;
        CompareOp depthCompareOp    = CompareOp::Less;
        BlendMode blendMode         = BlendMode::None;
        CullMode cullMode           = CullMode::Back;
        FrontFace frontFace         = FrontFace::CounterClockwise;
    };

    class Pipeline{
    public:
        virtual ~Pipeline() = default;

        // What it does: bake a complete pipeline configuration.
        // Why: this replaces ~6 separate GL state toggles (depth test,
        // depth func, blend mode, cull mode, front face, polygon mode)
        // with a single validated object. In GL, PipelineDesc values would
        // be set via glEnable/glDisable/glBlendFunc; in Vulkan, they're
        // baked into VkGraphicsPipelineCreateInfo at creation time.
        virtual void create(const PipelineDesc& desc) = 0;
    };
} // namespace yumi::rhi