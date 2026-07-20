cmake_minimum_required(VERSION 3.15)

project(TheDeepSea)

set(CMAKE_CXX_STANDARD 17)

# OpenGL
find_package(OpenGL REQUIRED)

# Executable
add_executable(TheDeepSea
    src/main.cpp
    src/Camera.cpp
    src/STLLoader.cpp
    src/Shader.cpp
    src/Texture.cpp
    extern/glad/src/glad.c
    src/graphics/Material.h
    src/graphics/Material.cpp
    src/graphics/Light.h
    src/graphics/Light.cpp
    src/graphics/ShadowMap.cpp
    src/postprocess/PostProcess.cpp
    src/physics/Collision.cpp
)

# Include directories
target_include_directories(TheDeepSea PRIVATE
    extern/glad/include
    extern/glfw/include
    extern/glm
    extern/stb
)

# GLFW static library
target_link_libraries(TheDeepSea PRIVATE
    OpenGL::GL
    ${CMAKE_SOURCE_DIR}/extern/glfw/lib-mingw-w64/libglfw3.a
)

# Windows system libs
target_link_libraries(TheDeepSea PRIVATE
    gdi32
)

# Copy runtime assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})