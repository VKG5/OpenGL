// This file is responsible for platform and compiler detection
#pragma once

// Platform detection
//
// Rule: platform branching outside backend code is usually a design smell.
// These macros exist so the few places that genuinely need them (debug
// intrinsics, OS paths, console color codes) have a single, authoritative
// source.
#if defined(_WIN32) || defined(_WIN64)
    #define YUMI_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #include <TargetConditionals.h>     // Apple SDK Headers
    #if TARGET_OS_MAC
        #define YUMI_PLATFORM_MACOS 1
    #else
        #error "Unsupported Apple Platform"
    #endif
#elif defined(__linux__) || defined(__unix__)
    #define YUMI_PLATFORM_LINUX 1
#else
    #error "Unknown platform - not supported"
#endif

// Compiler detection
//
// Why: GCC and Clang accept the same attributes but MSVC uses different
// ones (e.g. __declspec vs __attribute__). When the renderer needs
// platform-specific annotations later, these macros gate them.
#if defined(_MSC_VER)
    #define YUMI_COMPILER_MSVC 1
#elif defined(__clang__)
    #define YUMI_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define YUMI_COMPILER_GCC 1
#else
    #error "Unknown compiler - not supported"
#endif