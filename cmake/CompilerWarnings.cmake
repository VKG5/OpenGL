add_library(yumi_warnings INTERFACE)

if(MSVC)
    target_compile_options(yumi_warnings INTERFACE /W4 /permissive- /utf-8)
else()
    target_compile_options(yumi_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)
endif()

if(YUMI_WARNINGS_AS_ERRORS)
    target_compile_options(yumi_warnings INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
    )
endif()