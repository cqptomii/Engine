//
// Created by tomfr on 03/03/2026.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <chrono>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "engine/editor/editor_viewport.hpp"
#include "engine/core/input/input_manager.hpp"
#include "engine/rendering/renderer.hpp"
#include "engine/systems/editor_system.hpp"
#include "engine/systems/render_system.hpp"
#include "engine/core/primitives/MeshPrimitive3D.hpp"
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/ecs/components/material_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/utils.hpp"

class Engine
{
    std::unique_ptr<Window> window_ptr;
    InputManager input_manager = InputManager();

    EditorSystem editor_system;
    RenderSystem render_system;
    CpuResourceManager resource_manager;

    // Current Scene displayed
    Scene current_scene;


    float last_frame = 0.0f;
    float delta_time = 0.0f;
    float fps_timer = 0.0f;
    uint32_t fps_frame_count = 0;
    float displayed_fps = 0.0f;

    static void glfw_error_callback(const int error_code, const char* description)
    {
        std::cerr << "GLFW Error [" << error_code << "]: "
                  << (description ? description : "unknown") << std::endl;
    }

    static void init()
    {
        glfwSetErrorCallback(glfw_error_callback);

        if (!glfwInit())
        {
            const char* error_desc = nullptr;
            const int error_code = glfwGetError(&error_desc);
            std::cerr << "Failed to initialize GLFW"
                      << " (error " << error_code << ": "
                      << (error_desc ? error_desc : "unknown") << ")"
                      << std::endl;
            exit(EXIT_FAILURE);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    }
    void cleanup()
    {
        this->window_ptr.release();
    }

    void initialize_default_scene()
    {
        const auto cube_mesh = MeshPrimitive3D::CreateCube(this->resource_manager, "primitive/cube/default");
        const auto default_material = this->resource_manager.load_material_resource(
            "material/default",
            "sources/shader/base.vs",
            "sources/shader/base.fs",
            {}
        );
        const auto default_material_instance = this->resource_manager.create_material_instance(default_material);

        const entt::entity cube_entity = this->current_scene.add_object();
        this->current_scene.add_component(cube_entity, TransformComponent{glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(), glm::vec3(1.0f)});
        this->current_scene.add_component(cube_entity, MeshComponent{cube_mesh});
        this->current_scene.add_component(cube_entity, MaterialComponent{default_material_instance});
    }

public:
    Engine() : current_scene(this->resource_manager)
    {
        init();

        this->window_ptr = std::make_unique<Window>(800, 600, "Engine");
        this->window_ptr->setUserPointer(&this->input_manager);
        this->initialize_default_scene();
    }

    explicit Engine(std::unique_ptr<Window> window) : current_scene(this->resource_manager)
    {
        init();
        this->window_ptr = std::move(window);
        this->initialize_default_scene();
    }

    ~Engine()
    {
        glfwTerminate();
        this->cleanup();
    }

    // Main function
    void run()
    {
        float red = 0.0f, green = 0.0f, blue = 0.0f, alpha = 1.0f;
        convert_hex_to_rgba(0x383c42, red, green, blue, alpha);

        while (this->window_ptr->isOpen())
        {
            const float current_frame = static_cast<float>(glfwGetTime());
            this->delta_time = current_frame - this->last_frame;
            this->last_frame = current_frame;

            if (this->delta_time < 0.0f)
            {
                this->delta_time = 0.0f;
            }


            // process inputs
            this->input_manager.update();

            glClearColor(red, green, blue, alpha);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Set the OpenGL state for rendering
            this->window_ptr->set_depth_test(true);
            this->window_ptr->disable_blending();

            // Update viewports
            this->editor_system.update(this->current_scene, input_manager);

            int framebuffer_width = 0;
            int framebuffer_height = 0;
            glfwGetFramebufferSize(this->window_ptr->get_window_ptr(), &framebuffer_width, &framebuffer_height);

            // Get the editorCamera data
            auto editor_camera = this->editor_system.get_main_camera().get_camera_data(framebuffer_width, framebuffer_height);

            // Render the scene onto the screen
            this->render_system.update(
                this->current_scene,
                editor_camera,
                this->editor_system.get_editor_mode()
            );

            // Window buffer Update
            this->window_ptr->update();
            // Show frame per second
            this->show_frame_rate(this->delta_time);
        }
    }

    void show_frame_rate(const float delta_time)
    {
        this->fps_timer += delta_time;
        this->fps_frame_count++;

        if (this->fps_timer >= 1.0f)
        {
            this->displayed_fps = static_cast<float>(this->fps_frame_count) / this->fps_timer;
            std::cout << "FPS: " << this->displayed_fps << std::endl;

            this->fps_timer = 0.0f;
            this->fps_frame_count = 0;
        }
    }
};

#endif //ENGINE_HPP
