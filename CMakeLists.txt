cmake_minimum_required(VERSION 3.31)

if (POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(Engine)

set(CMAKE_CXX_STANDARD 20)
# Export the clangd Compile_command.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if (MSVC)
    # Enforce a single MSVC runtime across Engine and fetched dependencies.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/glfw-3.4 EXCLUDE_FROM_ALL)

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

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)
target_compile_definitions(imgui PUBLIC GLFW_INCLUDE_NONE)
target_link_libraries(imgui PUBLIC glfw)

# Ajouter le code source
add_executable(Engine
        src/main.cpp
        external/stb/stb_image.h
        external/stb/stb_image_impl.cpp
        external/glad/glad.c

        ## Core
        src/engine/core/engine.hpp
        src/engine/core/window.hpp
        src/engine/core/utils.hpp

        ## Math
        src/engine/math/aabb.hpp
        src/engine/math/ray.hpp

        ## Debug
        src/engine/core/debug/debug_config.hpp
        src/engine/core/debug/debug_id.hpp
        src/engine/core/debug/debug_name_registry.hpp
        src/engine/core/debug/debug_hooks.hpp
        src/engine/core/debug/debug_registration.hpp
        src/engine/core/debug/instrumentation.hpp
        src/engine/core/debug/log/log_types.hpp
        src/engine/core/debug/log/log_buffer.hpp
        src/engine/core/debug/log/logger.hpp
        src/engine/core/debug/profile/profiler.hpp
        src/engine/core/debug/stats/counter_registry.hpp
        src/engine/core/debug/stats/frame_stats.hpp
        src/engine/core/debug/stats/event_stats_listener.hpp
        src/engine/core/debug/memory/memory_tracker.hpp
        src/engine/core/debug/reporter/console_reporter.hpp

        ## Core Camera
        src/engine/core/camera_movement.hpp

        ## Input system
        src/engine/core/input/input.hpp
        src/engine/core/input/input_manager.hpp
        src/engine/core/input/input_system.hpp
        src/engine/core/input/context/input_mapping_context.hpp
        src/engine/core/input/context/editor_mapping_context.hpp
        src/engine/core/input/context/runtime_mapping_context.hpp

        ## Event system
        src/engine/core/event/event_types.hpp
        src/engine/core/event/event_category.hpp
        src/engine/core/event/event_listener.hpp
        src/engine/core/event/event_dispatcher.hpp
        src/engine/core/event/event_bus.hpp
        src/engine/core/event/event/ievent.hpp
        src/engine/core/event/event/key_pressed_event.hpp
        src/engine/core/event/event/key_released_event.hpp
        src/engine/core/event/event/mouse_motion_event.hpp
        src/engine/core/event/event/mouse_scroll_event.hpp
        src/engine/core/event/event/mouse_delta_event.hpp
        src/engine/core/event/event/mouse_button_pressed_event.hpp
        src/engine/core/event/event/mouse_button_released_event.hpp
        src/engine/core/event/event/action_started_event.hpp
        src/engine/core/event/event/action_performed_event.hpp
        src/engine/core/event/event/action_ended_event.hpp
        src/engine/core/event/event/window_resize_event.hpp

        ## GPU wrappers
        src/engine/resources/gpu/wrappers/Buffer.hpp
        src/engine/resources/gpu/wrappers/VAO.hpp
        src/engine/resources/gpu/wrappers/TextureBuffer.hpp

        ## Primitives
        src/engine/resources/cpu/primitives/MeshPrimitive2D.hpp
        src/engine/resources/cpu/primitives/MeshPrimitive3D.hpp
        src/engine/resources/cpu/vertex.hpp

        ## ECS
        src/engine/ecs/registry.hpp
        src/engine/ecs/components/transform_component.hpp
        src/engine/ecs/components/light_component.hpp
        src/engine/ecs/components/camera_component.hpp
        src/engine/ecs/components/model_component.hpp
        src/engine/ecs/components/mesh_component.hpp
        src/engine/ecs/components/material_component.hpp
        src/engine/ecs/components/debug_name_component.hpp
        src/engine/ecs/components/hierarchy_component.hpp
        src/engine/ecs/components/scene_root_component.hpp

        ## Scene
        src/engine/scene/Scene.hpp
        src/engine/scene/scene_hierarchy.hpp
        src/engine/scene/scene_serializer.hpp
        src/engine/scene/SceneManager.hpp

        ## Editor
        src/engine/editor/editor_camera.hpp
        src/engine/editor/editor_viewport.hpp
        src/engine/editor/runtime_viewport.hpp
        src/engine/editor/iviewport.hpp
        src/engine/editor/picking/picking.hpp
        src/engine/editor/picking/picking.cpp
        src/engine/editor/picking/manipulation_mode.hpp
        src/engine/editor/picking/object_manipulation.hpp
        src/engine/editor/picking/object_manipulation.cpp
        src/engine/editor/picking/selection_context.hpp
        src/engine/editor/picking/selection_context.cpp
        src/engine/editor/gizmo/gizmo_axis.hpp
        src/engine/editor/gizmo/gizmo_picker.hpp
        src/engine/editor/gizmo/gizmo_picker.cpp
        src/engine/editor/gizmo/gizmo_manipulator.hpp
        src/engine/editor/gizmo/gizmo_manipulator.cpp
        src/engine/editor/ui/imgui_layer.hpp
        src/engine/editor/ui/editor_ui.hpp
        src/engine/editor/ui/editor_context.hpp
        src/engine/editor/ui/viewport_framebuffer.hpp
        src/engine/editor/ui/viewport_layout.hpp
        src/engine/editor/ui/panels/debug_panel.hpp
        src/engine/editor/ui/panels/log_panel.hpp
        src/engine/editor/ui/panels/hierarchy_panel.hpp
        src/engine/editor/ui/panels/properties_panel.hpp
        src/engine/editor/editor_scene_factory.hpp

        ## Systems
        src/engine/systems/isystem.hpp
        src/engine/systems/render_system.hpp
        src/engine/systems/editor_system.hpp

        ## Rendering
        src/engine/rendering/utils/camera_data.hpp
        src/engine/rendering/utils/transform_data.hpp
        src/engine/rendering/utils/ubo_types.hpp
        src/engine/rendering/renderer.hpp
        src/engine/rendering/render_queue.hpp
        src/engine/rendering/gizmo/gizmo_renderer.hpp
        src/engine/rendering/gizmo/transform_gizmo_renderer.hpp
        src/engine/rendering/grid_renderer.hpp
        src/engine/rendering/selection_overlay_renderer.hpp

        ## Resources CPU
        src/engine/resources/cpu/resource_handle.hpp
        src/engine/resources/cpu/cpu_resource_manager.hpp
        src/engine/resources/cpu/shader_resource.hpp
        src/engine/resources/cpu/mesh_resource.hpp
        src/engine/resources/cpu/texture_resource.hpp
        src/engine/resources/cpu/light_resource.hpp
        src/engine/resources/cpu/material/material_resource.hpp
        src/engine/resources/cpu/material/material_instance.hpp
        src/engine/resources/cpu/model/model_resource.hpp
        src/engine/resources/cpu/model/model_resource.cpp

        ## Resources GPU
        src/engine/resources/gpu/light.hpp
        src/engine/resources/gpu/shader.hpp
        src/engine/resources/gpu/texture.hpp
        src/engine/resources/gpu/mesh.hpp
        src/engine/resources/gpu/gpu_resource_manager.hpp
)

target_include_directories(Engine
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/libs
        ${CMAKE_SOURCE_DIR}/external/stb
        ${glm_SOURCE_DIR}
)

target_compile_definitions(Engine PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG>
)

# Lier GLFW et OpenGL
if (WIN32)
    target_link_libraries(Engine PRIVATE imgui glfw opengl32 assimp EnTT::EnTT)
    add_custom_command(TARGET Engine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:assimp>
            $<TARGET_FILE_DIR:Engine>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:Engine>
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:Engine>/assets
    )
elseif(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    find_library(ASSIMP Assimp)
    target_link_libraries(Engine imgui glfw ${COCOA} ${IOKIT} ${COREVIDEO} ${ASSIMP} EnTT::EnTT)
else()
    target_link_libraries(Engine imgui glfw GL dl pthread X11 Xrandr Xi EnTT::EnTT)
endif()
 