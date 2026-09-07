#pragma once

// A semaphore is a synchronization primitive that lets GPU work wait on other GPU work - 
// for example, a render pass waits for a previous pass to finish writing a texture before 
// reading it. The CPU never waits on a semaphore directly; it only sets up the dependency 
// and lets the GPU resolve it. There is no OpenGL equivalent - this is entirely new.
// The class exists only as a handle type that `CommandQueue::submit` can reference.

namespace yumi::rhi {
    class Semaphore {
    public:
        virtual ~Semaphore() = default;

        // What it does: create the semaphore.
        // Why: the renderer needs semaphores to synchronize presentation with
        // rendering (don't draw into an image the display is still showing).
        // In Vulkan: vkCreateSemaphore.
        //
        // Currently empty beyond the destructor — the actual wait/signal semantics
        // are handled by the queue submission API (CommandQueue::submit takes
        // semaphores to wait on and signal). This class exists as an opaque
        // handle so the renderer can pass semaphores through its APIs without
        // knowing the backend implementation.
        virtual void create() = 0;
    };
} // namespace yumi::rhi