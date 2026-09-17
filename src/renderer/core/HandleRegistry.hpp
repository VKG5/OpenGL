#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <utility>      // std::exchange std::forward

#include "renderer/diagnostics/Log.hpp"
#include "renderer/core/Handle.hpp"

namespace yumi::rhi {
    // One registry per (tag, resource) pair. The registry OWNS the RHI
    // resource objects; handles are the only sanctioned way to reference
    // them. This resolves the ownership question from Device.hpp:
    // "Replace unique_ptr with generational handles" means the Device
    // will eventually create through a registry and return a handle.
    template<typename Tag, typename Resource>
    class HandleRegistry {
        struct Slot {
            // unique_ptr: null means "slot has never been used OR was
            // destroyed". The object itself is owned exclusively by the
            // slot; destroy() releases it.
            std::unique_ptr<Resource> resource;
            
            // Bumped on every destroy. A stored handle must match this to
            // be considered live.
            uint32_t generation = 0;
            
            // Mirrors (resource != nullptr). Keeping a bool makes intent
            // explicit and avoids repeating the null check everywhere.
            bool alive = false;
        };

    public:
        // Creates a resource in a slot and returns its handle.
        //
        // Slot selection policy: prefer the free-list (reuse), else append.
        // Reuse keeps the slot array compact instead of growing forever
        // with churning resources (per-frame transient buffers WILL churn).
        //
        // Forwarding args: the registry doesn't know (or care) how a
        // Resource is constructed — it is pure bookkeeping.
        template<typename... Args>
        Handle<Tag> create(Args&&... args) {
            uint32_t index;
            if(!m_freeList.empty()) {
                // LIFO reuse: the most recently freed slot is hottest in
                // cache and its Slot struct is already "warmed".
                index = m_freeList.back();
                m_freeList.pop_back();
            }

            else {
                index = static_cast<uint32_t>(m_slots.size());
                m_slots.emplace_back();
            }

            Slot& slot = m_slots[index];
            slot.resource = std::make_unique<Resource>(std::forward<Args>(args)...);
            slot.alive = true;

            // NOTE: generation is NOT bumped on first use of a fresh slot —
            // it starts at 0, and a create() returns (index, generation=0).
            // It only bumps on recycle, below.

            YUMI_LOG_DEBUG("Resource", "created (slot %u, gen %u, live %zu)",
                           index, slot.generation, live_count());

            return Handle<Tag>(index, slot.generation);
        }
        
        // Destroys the resource referenced by the handle.
        //
        // This is THE validation path — every one of the four checks is a
        // different real-world failure mode:
        //   out of range index   → corrupt/hand-crafted handle
        //   !alive               → double destroy (classic GL use-after-free)
        //   generation mismatch  → stale handle to a recycled slot (the bug
        //                          this whole system exists to catch)
        //   null resource        → invariant violation, should never happen
        //                          while alive — assert, it means a bug in
        //                          the REGISTRY, not the caller
        void destroy(Handle<Tag> handle) {
            if(!handle.is_valid()) {
                YUMI_LOG_WARNING("Resource", "destroy: invalid handle");
                return;
            }

            const uint32_t index = handle.index();
            if(index >= m_slots.size()) {
                YUMI_LOG_WARNING("Resource", "destroy: handle index out of range");
                return;
            }

            Slot& slot = m_slots[index];
            if(!slot.alive) {
                YUMI_LOG_WARNING("Resource", "destroy: double destroy (slot %u)",
                                 static_cast<unsigned int>(index));
                return;
            }

            if(slot.generation != handle.generation()) {
                YUMI_LOG_WARNING("Resource",
                                 "destroy: stale handle (gen %u, slot gen %u)",
                                 static_cast<unsigned int>(handle.generation()),
                                 static_cast<unsigned int>(slot.generation));
                return;
            }

            // Logical destruction happens NOW; physical GPU destruction is
            // exactly what this release() call is — Phase 0.6 will insert a
            // deferral point here (slot.resource moves to a "pending
            // destruction" list until the GPU is done with it).
            slot.resource.reset();
            slot.alive = false;

            // Recycle bookkeeping: bump generation so every outstanding
            // handle to this slot is now detectably stale, and offer the
            // slot for reuse.
            ++slot.generation;
            m_freeList.push_back(index);

            YUMI_LOG_DEBUG("Resource", "destroyed (slot %u, gen now %u, live %zu)",
                            index, slot.generation, live_count());
        }

        // Optional accessor: returns nullptr for invalid/stale handles.
        // Use when "the resource might have gone away" is an expected,
        // handled situation (e.g. freeing per-frame data).
        Resource* get(Handle<Tag> handle) {
            if(!validate(handle)) return nullptr;
            return m_slots[handle.index()].resource.get();
        }

        // Checked accessor: for hot paths where the resource MUST exist
        // (e.g. binding a buffer for a draw that was validated earlier).
        // Returns a reference; a stale handle is a programming error here.
        Resource& get_checked(Handle<Tag> handle) {
            if(!validate(handle)) {
                // Crash loudly in debug builds; in release this is UB, but
                // by then validation layers + tests must have caught it.
                // Alternative: throw. Decide based on your error policy.
                YUMI_LOG_ERROR("Resource", "get_checked: stale/invalid handle");
                    
                assert(validate(handle) && "get_checked: stale/invalid handle");
                return *m_slots[handle.index()].resource;
            }
        }

        bool is_valid(Handle<Tag> handle) const {
            return validate(handle);
        }

        // Introspection — useful for resource-leak detection later
        // (Phase 0.8 wants "debug logging for resource create/destroy";
        // a count of live resources at shutdown is how you detect leaks).
        size_t live_count() const {
            return m_slots.size() - m_freeList.size();
        }

    private:
        bool validate(Handle<Tag> handle) const {
            if(!handle.is_valid()) return false;

            const uint32_t index = handle.index();
            if(index >= m_slots.size()) return false;

            const Slot& slot = m_slots[index];
            return slot.alive && slot.generation == handle.generation();
        }

        std::vector<Slot> m_slots;              // dense, never shrinks
        std::vector<uint32_t> m_freeList;       // LIFO of reusable indices
    };
} // namespace yumi::rhi