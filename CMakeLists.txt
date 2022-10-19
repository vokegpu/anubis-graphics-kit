cmake_minimum_required(VERSION 3.13)
project(agk)

set(LINKED_LIBRARIES "")
set(BUILD_LOCATION "")

if(WIN32)
	set(LINKED_LIBRARIES mingw32 opengl32 glew32 SDL2main SDL2 freetype)
	set(BUILD_LOCATION "../build/win/")
else()
	set(LINKED_LIBRARIES SDL2main SDL2 GL GLEW freetype)
	set(BUILD_LOCATION "../build/linux/")
endif()

# Set the version of C++ 17 and the build location.
set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_LOCATION})
add_compile_options("-O3")

# Find for all files.
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Include the src for environment of project.
include_directories(src)

# Inject at the executable the src files and link the libraries used in the project.
add_executable(agk ${SRC_FILES})
target_link_libraries(agk ${LINKED_LIBRARIES})