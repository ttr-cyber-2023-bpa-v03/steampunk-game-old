# CMake build script for steampunk-game

# I hate cmake with a passion. Their scripting language is awful.
# Stuff breaks when you run once, but the 2nd time it works with
# ZERO CHANGES. Great to know I wasted 3 hours of my life on this
# garbage. I hope there's a better alternative to CMake soon.

cmake_minimum_required(VERSION 3.5)

# Force CMake to make options respect 'set' or else this will break
# on the first build. I'm just slowly deteriorating at this point.
cmake_policy(SET CMP0077 NEW)

# Init project
project(steampunk-game LANGUAGES C CXX)

# EXPORT COMPILE COMMANDS ALREADY GOD DAMN IT
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Enable projects in folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

# Add dependencies
add_subdirectory(dependencies/glm)
set_property(TARGET glm PROPERTY FOLDER "dependencies/glm")

set(TBB_TEST OFF)
set(TBB_INSTALL OFF)
add_subdirectory(dependencies/oneTBB)
set_property(TARGET tbb tbbmalloc tbbmalloc_proxy PROPERTY FOLDER "dependencies/oneTBB")

set(SDL_TEST OFF)
set(SDL2_DISABLE_INSTALL OFF)
add_subdirectory(dependencies/SDL EXCLUDE_FROM_ALL)
set_property(TARGET SDL2 SDL2main sdl_headers_copy PROPERTY FOLDER "dependencies/SDL")

set(SDL2IMAGE_INSTALL OFF)
add_subdirectory(dependencies/SDL_image)
set_property(TARGET SDL2_image PROPERTY FOLDER "dependencies/SDL_image")

set(SDL2TTF_INSTALL OFF)
add_subdirectory(dependencies/SDL_ttf)
set_property(TARGET SDL2_ttf glfont showfont PROPERTY FOLDER "dependencies/SDL_ttf")

# Add source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
    #src/*.hpp
)

# Create the main executable
add_executable(steampunk-game ${SOURCES})

# Include directories
target_include_directories(steampunk-game PRIVATE src)

# Libraries
if(TARGET SDL2::SDL2main)
    target_link_libraries(steampunk-game PRIVATE SDL2::SDL2main)
endif()

# Link dependencies
target_link_libraries(steampunk-game PRIVATE
    glm
    TBB::tbb
    SDL2::SDL2
    SDL2_image
    SDL2_ttf
)

# Additional settings

# Specify C++ standard
target_compile_features(steampunk-game PRIVATE cxx_std_20)

# Copy SDL and TBB DLLs to the executable directory
add_custom_command(TARGET steampunk-game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:TBB::tbb>
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE:SDL2_image>
        $<TARGET_FILE:SDL2_ttf>
        $<TARGET_FILE_DIR:steampunk-game>
)