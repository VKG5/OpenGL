#pragma once

#include <cstdint>

namespace yumi::rhi {
    enum class ShaderStage : uint8_t {
        Vertex,         // Was the .vert file in src/Imgui/Shaders/
        Fragment,       // Was the .frag file
        Geometry        // Was omniShadowMap.geom in the Base app
    };

    class Shader{
    public:
        virtual ~Shader() = default;

        // What it does: compile and link shader stages into a GPU program.
        // Why: Shader::createFromFiles read GLSL from disk, compiled each stage,
        // and linked a program. This method takes pre-compiled bytecode (SPIR-V
        // for Vulkan, or GLSL strings for the GL backend) — the renderer doesn't
        // compile; it passes bytecode and the backend handles the rest.
        // This is necessary for the RHI to stay backend-independent.
        //
        // Parameters:
        //   stages    - array of ShaderStage values (e.g. {Vertex, Fragment})
        //   bytecodes - parallel array of pointers to compiled bytecode
        //   sizes     - parallel array of bytecode sizes in bytes
        //   count     - number of stages (typically 2: vertex + fragment)
        //
        // The renderer calls this once per shader variant; the backend holds the
        // result for pipeline creation.
        virtual void create(const ShaderStage* stages,
                            const void** bytecodes,
                            const size_t* sizes,
                            uint32_t count) = 0;
    };
} // namespace yumi::rhi