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
        src/engine/core/engine.hpp
        src/engine/core/window.hpp
        src/engine/ecs/registry.hpp
        src/engine/ecs/entity.hpp
        src/engine/ecs/component_storage.hpp
        src/engine/components/mesh_component.hpp
        src/engine/components/transform_component.hpp
        src/engine/components/light_component.hpp
        src/engine/components/camera_component.hpp
        src/engine/editor/editor_camera.hpp
        src/engine/editor/editor_viewport.hpp
        src/engine/editor/runtime_viewport.hpp
        src/engine/core/input/input_manager.hpp
        src/engine/core/input/input_mapping_context.hpp
        src/engine/core/input/context/editor_mapping_context.hpp
        src/engine/core/input/context/runtime_mapping_context.hpp
        src/engine/core/input/input.hpp
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