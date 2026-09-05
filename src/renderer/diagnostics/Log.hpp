#pragma once

// Minimal leveled logging for the renderer.
//
// Design:
//   - One function, one call path.  No logging library dependency.
//   - Macros as the only sanctioned call-site surface.
//   - stderr as destination (decoupled from stdout; can change to a file,
//     GPU marker bridge, or ring buffer later without touching call sites).

namespace yumi::log {
    enum class Level : int {
        Trace,          // high-frequency per-frame noise
        Debug,          // development diagnostics
        Info,           // normal operational messages (resource created, pass completed)
        Warning,        // non-fatal issues (fallback path taken, feature unsupported)
        Error           // fatal for the current operation (shader failed to compile)
    };

    // Core entry point. `tag` identifies the subsystem, keep it short and stable
    // (e.g. "renderer", "rhi", "opengl", "vulkan")
    void message(Level level, const char* tag, const char* format, ...);

    // Compiler check, MSVC doesn't support attribute style formatting
    #if defined(YUMI_COMPILER_GCC) || defined(YUMI_COMPILER_CLANG)
        void message(Level level, const char* tag, const char* format, ...)
        __attribute__((format(printf, 3, 4)));
    #endif
} // namespace yumi::log


// Macros
// They wrap the function so call sites look uniform and don't repeat
// the level enum path.  They also make it trivial to add a source-location
// macro (__FILE__, __LINE__) later without changing every call site.
#define YUMI_LOG_TRACE(tag, ...) \
    ::yumi::log::message(::yumi::log::Level::Trace,     (tag), __VA_ARGS__)

#define YUMI_LOG_DEBUG(tag, ...) \
    ::yumi::log::message(::yumi::log::Level::Debug,     (tag), __VA_ARGS__)

#define YUMI_LOG_INFO(tag, ...) \
    ::yumi::log::message(::yumi::log::Level::Info,      (tag), __VA_ARGS__)

#define YUMI_LOG_WARNING(tag, ...) \
    ::yumi::log::message(::yumi::log::Level::Warning,   (tag), __VA_ARGS__)

#define YUMI_LOG_ERROR(tag, ...) \
    ::yumi::log::message(::yumi::log::Level::Error,     (tag), __VA_ARGS__)