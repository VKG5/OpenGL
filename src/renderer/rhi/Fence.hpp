#pragma once

// A fence is a synchronization primitive that lets the CPU wait for the GPU to finish 
// executing submitted work. OpenGL's implicit synchronization hides this entirely - 
// `glDrawElements` returns immediately, and the driver ensures the GPU catches up 
// before the next frame. In Vulkan, the CPU must explicitly track which frames the GPU 
// is still working on (frames in flight) to avoid overwriting resources the GPU is using.

namespace yumi::rhi {
    class Fence {
    public:
        virtual ~Fence() = default;

        // What it does: create a fence, optionally in the "signaled" state.
        // Why: the first frame has no prior GPU work, so the fence starts
        // signaled (no need to wait). In Vulkan: VkFenceCreateFlagBits.
        virtual void create(bool startSignaled = false) = 0;

        // What it does: block the calling thread until the GPU signals this fence.
        // Why: the renderer must not overwrite a buffer that the GPU is still
        // reading. Calling wait() before reusing a resource guarantees the GPU
        // is done. In Vulkan: vkWaitForFences. In GL: glFinish (heavy-handed equivalent).
        virtual void wait() = 0;

        // What it does: reset the fence back to the unsignaled state.
        // Why: after wait() returns, the fence must be reset before the next
        // submit() can signal it again. Vulkan fences are not auto-reset.
        virtual void reset() = 0;

        // What it does: check whether the GPU has signaled this fence, without blocking.
        // Why: allows the renderer to check "is this frame's GPU work done?" without
        // stalling. Used for non-blocking resource recycling.
        virtual bool isSignaled() const = 0;
    };
} // namespace yumi::rhi