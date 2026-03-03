cmake_minimum_required(VERSION 3.31)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

# Inclure les headers
include_directories(libs)
include_directories(${CMAKE_SOURCE_DIR}/libs/glfw-3.4/include)
link_directories(${CMAKE_SOURCE_DIR}/libs/glfw-3.4/lib-vc2022)

include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)

FetchContent_MakeAvailable(glm)

# Ajouter le code source
add_executable(Engine
        src/main.cpp
        src/glad.c
        src/engine/utils.hpp
)

# Lier GLFW et OpenGL
if (WIN32)
    target_link_libraries(Engine glfw3 opengl32 glm::glm)
elseif(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    target_link_libraries(Engine glfw ${COCOA} ${IOKIT} ${COREVIDEO})
else()
    target_link_libraries(Engine glfw GL dl pthread X11 Xrandr Xi)
endif()