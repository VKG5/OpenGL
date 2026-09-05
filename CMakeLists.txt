## Steps to follow - Generate the project
# Create source and CMakeLists
# 1) mkdir build
# 2) cd build
# 3) cmake .. -> Generating the build files/Configuring the project
# 4) cmake --build . -> Command to build all targets
# 5) ./Executable

## Steps to follow - Update project
# 1) cd build
# 2) cmake .

## Setting the generator in Terminal to a specific version
# cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_STANDARD=17 -S path/to/source -B path/to/build
# cmake -G "Visual Studio 17 2022"
cmake_minimum_required(VERSION 3.6)

# Solve cross-directory linking issue
cmake_policy(SET CMP0079 NEW)

project(
    Yumi
    VERSION 0.1.0
    LANGUAGES C CXX
)

# Setting the language version
set(CMAKE_CXX_STANDARD 20)

# Making it so the language is enforced
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For external libraries
set(CMAKE_CXX_EXTENSIONS OFF)

# Feeds clang-tidy/clanged later
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setting the Executable and Library Name
set(EXTERN_LIBRARY_NAME Library_External)
set(EXECUTABLE_NAME Executable)
set(LIBRARY_NAME Library)

# Renderer Name
set(RENDERER_LIBRARY Renderer)

# Add external libraries manually
# Setting the Cmake modules path to our custom path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

## Adding extra modules
# include(AddGitSubmodules) - Optional custom module
include(FetchContent)
include(CompilerWarnings)

# Platform report
message(STATUS "Platform : ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_PROCESSOR})")
message(STATUS "Compiler : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Linux check
if(UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    if(NOT X11_FOUND)
        message(FATAL_ERROR
                " X11 development files not found (required by GLFW on Linux).\n"
                " Debian/Ubuntu: sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev\n"
                " Fedora    : sudo dnf install libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel\n"
                " Arch      : sudo pacman -S libx11 libxrandr libxinerama libxcursor libxi" )
    endif()
endif()

# Optional params for users
option(YUMI_BUILD_APPS "Build the demo applications" ON)
option(YUMI_WARNINGS_AS_ERRORS "Treat first-party warnings as errors" OFF)

# Adding git libraries with inbuilt function
FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY "https://github.com/KhronosGroup/Vulkan-Headers"
    GIT_TAG v1.4.362
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Vulkan-Headers)

FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)

# Dependency trimming
set(GLM_TEST_ENABLE     OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw"
    GIT_TAG 3.5.1
    GIT_SHALLOW TRUE
)

# Dependency trimming
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY "https://github.com/assimp/assimp"
    GIT_TAG v6.0.5
    GIT_SHALLOW TRUE
)

# Dependency trimming
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT          ON  CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# Calling the function
#add_git_submodules(extern/glew)

# Since src (Application) is dependent on include (Custom Libraries)
# We include them in a reverse order, otherwise
# The application will throw an error while building
add_subdirectory(extern)
add_subdirectory(include)
add_subdirectory(src)
