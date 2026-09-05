set(
    LIBRARY_SOURCES
    "diagnostics/Log.cpp"
)

set(
    LIBRARY_HEADERS
    "core/Defines.hpp"
    "diagnostics/Log.hpp"
)

add_library(
    ${RENDERER_LIBRARY} STATIC
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS}
)

add_library(yumi::renderer ALIAS ${RENDERER_LIBRARY})
target_include_directories(${RENDERER_LIBRARY} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${RENDERER_LIBRARY} PUBLIC yumi_warnings)
