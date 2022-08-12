cmake_minimum_required(VERSION 3.13)
project(anubis_graphics_kit)

# Set the version of C++ 11 and the build location (the DLLs are copy manually).
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ../build)

# Find for all files.
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE LIB_FILES "external/*.lib")

# Include the src for environment of project.
include_directories(src)

# Inject at the executable the src files and link the libraries used in the project.
add_executable(anubis_graphics_kit ${SRC_FILES})
target_link_libraries(anubis_graphics_kit mingw32 opengl32 glew32 SDL2main SDL2)