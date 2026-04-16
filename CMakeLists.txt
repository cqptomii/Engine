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
        GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.4
)
FetchContent_MakeAvailable(assimp)

FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt.git
        GIT_TAG v3.15.0
)
FetchContent_MakeAvailable(entt)

# Ajouter le code source
add_executable(Engine
        src/main.cpp
        src/stb_image.h
    src/stb_image_impl.cpp
        src/glad.c
        src/engine/utils.hpp
        src/engine/core/engine.hpp
        src/engine/core/window.hpp
        src/engine/ecs/registry.hpp
        src/engine/ecs/components/transform_component.hpp
        src/engine/ecs/components/light_component.hpp
        src/engine/ecs/components/camera_component.hpp
        src/engine/editor/editor_camera.hpp
        src/engine/editor/editor_viewport.hpp
        src/engine/editor/runtime_viewport.hpp
        src/engine/core/input/input_manager.hpp
        src/engine/core/input/input_mapping_context.hpp
        src/engine/core/input/context/editor_mapping_context.hpp
        src/engine/core/input/context/runtime_mapping_context.hpp
        src/engine/core/input/input.hpp
        src/engine/rendering/shader.hpp
        src/engine/rendering/Texture.hpp
        src/engine/rendering/mesh.hpp
        src/engine/rendering/renderer.hpp
        src/engine/rendering/render_queue.hpp
        src/engine/resources/resource_manager.hpp
        src/engine/resources/shader_resource.hpp
        src/engine/resources/mesh_resource.hpp
        src/engine/resources/material/material_instance.hpp
        src/engine/resources/material/material_resource.hpp
        src/engine/resources/texture_resource.hpp
        src/engine/resources/resource_handle.hpp
        src/engine/resources/model/model_resource.hpp
        src/engine/resources/model/model_resource.cpp
        src/engine/systems/render_system.hpp
        src/engine/ecs/components/model_component.hpp
        src/engine/systems/system.hpp
        src/engine/resources/light_resource.hpp
        src/engine/rendering/light.hpp
        src/engine/scene/Scene.hpp
        src/engine/systems/editor_system.hpp
        src/engine/editor/iviewport.hpp
        src/engine/rendering/camera_data.hpp
        src/engine/ecs/components/material_component.hpp
        src/engine/scene/SceneManager.hpp
        src/engine/resources/material/UniformBuffer.hpp
)

target_include_directories(Engine PRIVATE ${CMAKE_SOURCE_DIR}/src ${glm_SOURCE_DIR})

# Lier GLFW et OpenGL
if (WIN32)
    target_link_libraries(Engine glfw3 opengl32 assimp EnTT::EnTT)
    add_custom_command(TARGET Engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:assimp>
            $<TARGET_FILE_DIR:Engine>
    )
elseif(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    find_library(ASSIMP Assimp)
    target_link_libraries(Engine glfw ${COCOA} ${IOKIT} ${COREVIDEO} ${ASSIMP} EnTT::EnTT)
else()
    target_link_libraries(Engine glfw GL dl pthread X11 Xrandr Xi EnTT::EnTT)
endif()