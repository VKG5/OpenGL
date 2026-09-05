#include "renderer/diagnostics/Log.hpp"

#include <cstdarg>
#include <cstdio>

// Implementation for the logger header
// 
// Implements message(), formats the level name, the tag, the user's
// format string into one line on `stderr`
// Why `stderr` : `stdout` is often line-buffered or block-buffered (hidden)
// behind std::cout << flush. `stderr` is unbuffered, meaning the message
// appears immediately when the program crashes or aborts


namespace yumi::log {
    // Anonymous namespace
    // Anything inside an anonymous namespace has internal linkage: 
    // -> Only accessible from this .cpp / translation unit. 
    // -> Good for implementation-only helper functions, constants, types. 
    // -> Prevents exposing internal symbols to other source files. 
    // 
    // Think: 
    // "Private implementation details for this .cpp file."
    namespace {
        const char* level_name(Level level) {
            switch(level) {
                case Level::Trace:      return "TRACE";
                case Level::Debug:      return "DEBUG";
                case Level::Info:       return "INFO";
                case Level::Warning:    return "WARNING";
                case Level::Error:      return "ERROR";
            }

            return "?????";
        }
    } // namespace
    
    // C-Style Variadic arguments
    // `...` means the function accepts additional arguments.
    // IMPORTANT <cstdarg> TYPES/FUNCTIONS: 
    // 
    // va_list 
    // Represents the state/cursor used to access the arguments. 
    // 
    // va_start(args, format) 
    // Initializes `args`. 
    // `format` must be the last named parameter. 
    // 
    // va_arg(args, Type) 
    // Retrieves the next argument as `Type`. 
    // 
    // va_copy(copy, args) 
    // Creates an independent copy of a va_list. 
    // 
    // va_end(args) // Finishes/cleans up the va_list.
    void message(Level level, const char* tag, const char* format, ...) {
        // Print the prefix: [LEVEL] [tag]
        std::fprintf(stderr, "[%s] [%s] ", level_name(level), tag);

        // Froward the variadic args to vfprintf
        va_list args;
        va_start(args, format);

        // `vfprintf()` consumes the variadic arguments using the printf-style 
        // `format` string.
        std::vfprintf(stderr, format, args);
        va_end(args);

        // Newline
        std::fputc('\n', stderr);
    }
} // namespace yumi::log