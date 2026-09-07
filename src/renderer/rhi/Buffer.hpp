#pragma once

#include <cstddef>
#include <cstdint>

// Pure Virtual Interface
// - Strictly contains only pure virtual functions (= 0). No implementation code allowed.
// - Should not contain data members.
// - Mandatory for the derived class to override every single function to be instantiated.
// - Almost never defines a constructor (only a virtual destructor).
namespace yumi::rhi {
    enum class BufferUsage : uint8_t {
        Vertex,         // Vertex positions, normals, UVs (was VBO in Mesh::createMesh)
        Index,          // Triangle index data (was IBO in Mesh::createMesh)
        Uniform,        // Constant data read by shaders (was glUniform* uploads)
        Staging         // CPU-visible buffer used to transfer data to GPU-local memory
    };

    class Buffer {
    public:
        virtual ~Buffer() = default;

        // What it does: allocate GPU memory for this buffer.
        // Why: Mesh::createMesh calls glBufferData - this is the equivalent.
        // The renderer must know how large a buffer is and what it's for
        // before it can put data into it.
        virtual void create(size_t bytes, BufferUsage usage) = 0;

        // What it does: copy data from CPU memory into this buffer.
        // Why: Mesh::createMesh uploads vertex/index data; uniform buffers
        // are updated per-frame. The offset parameter supports sub-allocation
        // (writing part of a larger buffer) - needed later for dynamic uniform
        // buffers without creating a new VkBuffer per frame.
        virtual void upload(const void* data, size_t bytes, size_t offset = 0) = 0;

        // What it does: report how many bytes this buffer holds.
        // Why: validation (drawing with more indices than the buffer contains
        // is a GPU crash). The renderer needs this without querying the backend.
        virtual size_t size() const = 0;

        // What it does: report the usage intent.
        // Why: the renderer may want to assert that a vertex buffer isn't
        // accidentally bound as a uniform buffer.
        virtual BufferUsage usage() const = 0;
    };
} // namespace yumi::rhi