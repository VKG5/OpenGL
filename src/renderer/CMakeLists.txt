set(
    LIBRARY_SOURCES
    "diagnostics/Log.cpp"
)

set(
    LIBRARY_HEADERS
    "core/Defines.hpp"
    "diagnostics/Log.hpp"
    
    # Handles and Registry
    "core/Handle.hpp"
    "core/HandleRegistry.hpp"
    
    # RHI Headers
    "rhi/Buffer.hpp"
    "rhi/CommandBuffer.hpp"
    "rhi/CommandQueue.hpp"
    "rhi/Sampler.hpp"
    "rhi/Shader.hpp"
    "rhi/Texture.hpp"
    "rhi/TextureView.hpp"
    "rhi/RenderTarget.hpp"
    "rhi/Pipeline.hpp"
    "rhi/Fence.hpp"
    "rhi/Semaphore.hpp"
    "rhi/Device.hpp"
)

add_library(
    ${RENDERER_LIBRARY} STATIC
    ${LIBRARY_SOURCES}
    ${LIBRARY_HEADERS}
)

add_library(yumi::renderer ALIAS ${RENDERER_LIBRARY})
target_include_directories(${RENDERER_LIBRARY} PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${RENDERER_LIBRARY} PUBLIC yumi_warnings)

add_executable(HandleScratch tests/HandleScratch.cpp)
target_link_libraries(HandleScratch PRIVATE ${RENDERER_LIBRARY})
