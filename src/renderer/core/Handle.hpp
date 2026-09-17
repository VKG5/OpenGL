#pragma once

#include <cstdint>

namespace yumi::rhi {
    // Empty tag types — exist only so that Handle<BufferTag> and
    // Handle<TextureTag> are DIFFERENT TYPES. Compile-time safety, zero
    // runtime cost: a BufferHandle cannot be passed to code expecting a
    // TextureHandle. Tags are never defined; they are never instantiated
    struct BufferTag {};
    struct TextureTag {};
    struct SamplerTag {};
    struct ShaderTag {};
    struct PipelineTag{};

    // Generational handle — a lightweight, trivially copyable value type.
    //   index      : which slot in the owning HandleRegistry
    //   generation : the registry bumps a slot's generation every time the
    //                slot is recycled. A handle whose generation does not
    //                match the slot's is STALE and must be rejected.
    //
    // Note what this class does NOT contain:
    //   - no pointer to the resource (the registry owns that)
    //   - no reference to the registry (the caller knows which registry it
    //     came from — handles from different registries are never mixed)
    //   - no virtual functions, no destructor logic (it must stay 8 bytes)
    template<typename Tag>
    class Handle {
    public:
        static constexpr uint32_t INVALID_INDEX = ~0u;
        
        // Default constructor = invalid handle. This matters because:
        //  1. "Handle h;" must be legal (deferred acquisition pattern)
        //  2. Renderer code stores handles in structs/arrays that must
        //     default-construct before resources are loaded
        constexpr Handle() = default;

        // Production constructor — only the registry calls this.
        // Why explicit: prevents accidental Handle{3u, 0u} construction
        // from raw numbers outside the registry.
        constexpr Handle(uint32_t index, uint32_t generation) 
            : m_index(index), m_generation(generation) {}

        // Validity: an index of INVALID_INDEX can never be allocated,
        // so it unambiguously means "no resource".
        constexpr bool is_valid() const { return m_index != INVALID_INDEX; }
        constexpr explicit operator bool() const { return is_valid(); }

        // Raw index accessor. Deliberately available, deliberately raw —
        // the REGISTRY decides what the index means; callers just need it
        // for storage in flat arrays / maps keyed by index.
        constexpr uint32_t index() const { return m_index; }
        
        // Generation is NOT exposed publicly. Only the registry compares
        // generations. If renderer code starts comparing generations, you
        // have leaked the recycling mechanism into user code. (If you later
        // find a real need, add a friend declaration instead.)
        constexpr uint32_t generation() const {return m_generation; }

        // Comparison: needed for deduplication, map keys, and "is this the
        // same resource I cached last frame?" checks. Note: comparing
        // generation too means an OLD handle and the NEW handle for the
        // same slot compare unequal — which is exactly what we want.
        constexpr bool operator==(const Handle& other) const {
            return m_index == other.m_index && m_generation == other.m_generation;
        }
        constexpr bool operator!=(const Handle& other) const {
            return !(*this == other);
        }

        // Ordering: NOT strictly necessary, but enables std::set/std::map
        // keyed by handle without extra comparators. Cheap to provide.
        constexpr bool operator<(const Handle& other) const {
            return m_index != other.m_index 
                ? m_index < other.m_index 
                : m_generation < other.m_generation;
        }

    private:
        uint32_t m_index = INVALID_INDEX;
        uint32_t m_generation = 0;
    };

    using BufferHandle = Handle<BufferTag>;
    using TextureHandle = Handle<TextureTag>;
    using SamplerHandle = Handle<SamplerTag>;
    using ShaderHandle = Handle<ShaderTag>;
    using PipelineHandle = Handle<PipelineTag>;
} // namespace yumi::rhi