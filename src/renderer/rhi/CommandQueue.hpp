#pragma once

namespace yumi::rhi {
    // Forward Declaration - CommandQueue doesn't own any of the references
    class CommandBuffer;
    class Fence;
    class Semaphore;

    class CommandQueue {
    public:
        virtual ~CommandQueue() = default;

        // What it does: submit a command buffer for GPU execution.
        // Why: in OpenGL, glfwSwapBuffers implicitly submits all pending commands.
        // In Vulkan, submission is explicit - vkQueueSubmit takes a command buffer,
        // optional fence to signal on completion, and semaphores to wait on/signal.
        // The GL backend can wrap glFlush/glFinish here.
        //
        // signalFence: if non-null, the GPU signals this fence when the command
        //   buffer finishes executing. The CPU can later call fence->wait() to
        //   block until execution is complete.
        virtual void submit(CommandBuffer& cmd,
                            Fence* signalFence = nullptr) = 0;

        // What it does: present a rendered image to the display.
        // Why: replaces glfwSwapBuffers. In Vulkan, presentation is a queue
        // operation (vkQueuePresentKHR) that takes a swapchain image and a
        // semaphore to wait on. The GL backend wraps glfwSwapBuffers here.
        virtual void present() = 0;
        
        // What it does: block until all previously submitted work on this queue
        // has finished executing on the GPU.
        // Why: replaces glFinish. Used during shutdown and resource destruction
        // to ensure the GPU isn't still reading resources being freed.
        virtual void waitIdle() = 0;
    };
} // namespace yumi::rhi