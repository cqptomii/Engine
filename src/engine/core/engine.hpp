//
// Created by tomfr on 03/03/2026.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Window includes
#include "engine/core/window.hpp"

// Input includes
#include "engine/core/input/input_manager.hpp"
#include "engine/core/input/input_system.hpp"

// Event includes
#include "engine/core/event/event_bus.hpp"

// Resource includes
#include "engine/resources/cpu/primitives/MeshPrimitive3D.hpp"

// Systems includes
#include "engine/systems/editor_system.hpp"
#include "engine/systems/render_system.hpp"

// Components includes
#include "engine/ecs/components/mesh_component.hpp"
#include "engine/ecs/components/material_component.hpp"
#include "engine/ecs/components/transform_component.hpp"
#include "engine/ecs/components/debug_name_component.hpp"

// Debug includes
#include "engine/core/debug/debug_config.hpp"
#include "engine/core/debug/debug_registration.hpp"
#include "engine/core/debug/instrumentation.hpp"

// Utils includes
#include "engine/core/utils.hpp"

// Editor UI includes
#include "engine/editor/ui/imgui_layer.hpp"
#include "engine/editor/ui/editor_ui.hpp"
#include "engine/editor/ui/editor_context.hpp"
#include "engine/editor/ui/viewport_layout.hpp"
#include "engine/editor/ui/viewport_framebuffer.hpp"
#include "engine/scene/scene_hierarchy.hpp"

class Engine
{
    // Pointer to the main OpenGL Window
    std::unique_ptr<Window> window_ptr;

    // Main Event Bus object
    EventBus event_bus;

    // Input systems objects
    InputManager input_manager;
    InputSystem input_system;

    // Main Systems objects
    EditorSystem editor_system;
    RenderSystem render_system;
    CpuResourceManager resource_manager;

    // Editor UI
    ImGuiLayer imgui_layer;
    EditorUI editor_ui;

    // Current Scene displayed
    Scene current_scene;


    float last_frame = 0.0f;
    float delta_time = 0.0f;

    /**
     * @brief GLFW error callback
     * Print the error code and the description of the error
     */
    static void glfw_error_callback(const int error_code, const char* description)
    {
        std::cerr << "GLFW Error [" << error_code << "]: "
                  << (description ? description : "unknown") << std::endl;
    }


    /**
     * @brief Initialize the GLFW library and the OpenGL context
     * Set the error callback
     * Initialize the GLFW library
     * Set the window hints
     * Initialize the OpenGL context
     * Initialize the OpenGL context
     */
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

    /**
     * @brief Cleanup the window object
     * Release the window object
     */
    void cleanup()
    {
        this->window_ptr.release();
    }

    /**
     * @brief Initialize the default scene
     * Create a new cube mesh
     * Create a new default material
     * Create a new default material instance
     * Add the cube entity to the scene
     * Add the transform component to the cube entity
     * Add the mesh component to the cube entity
     * Add the material component to the cube entity
     */
    void initialize_default_scene()
    {
        const auto cube_mesh = MeshPrimitive3D::CreateCube(this->resource_manager, "primitive/cube/default");
        const auto default_material = this->resource_manager.load_material_resource(
            "material/default",
            "assets/shaders/base.vs",
            "assets/shaders/base.fs",
            {}
        );
        const auto default_material_instance = this->resource_manager.create_material_instance(default_material);

        const entt::entity scene_root = scene_hierarchy::ensure_root(this->current_scene);

        const entt::entity cube_entity = this->current_scene.add_object();
        const DebugNameComponent cube_name{"Cube"};
        this->current_scene.add_component(cube_entity, TransformComponent{glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f)});
        this->current_scene.add_component(cube_entity, cube_name);
        this->current_scene.add_component(cube_entity, MeshComponent{cube_mesh});
        this->current_scene.add_component(cube_entity, MaterialComponent{default_material_instance});
        scene_hierarchy::set_parent(this->current_scene, cube_entity, scene_root);
        debug_register_name(cube_name.get_id(), cube_name.get_name());

        const auto sphere_mesh = MeshPrimitive3D::CreateUVSphere(this->resource_manager, "primitive/sphere/default");
        const entt::entity sphere_entity = this->current_scene.add_object();
        const DebugNameComponent sphere_name{"Sphere"};
        this->current_scene.add_component(sphere_entity, TransformComponent{glm::vec3(2.5f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f)});
        this->current_scene.add_component(sphere_entity, sphere_name);
        this->current_scene.add_component(sphere_entity, MeshComponent{sphere_mesh});
        this->current_scene.add_component(sphere_entity, MaterialComponent{default_material_instance});
        scene_hierarchy::set_parent(this->current_scene, sphere_entity, scene_root);
        debug_register_name(sphere_name.get_id(), sphere_name.get_name());
    }

public:

    /**
     * @brief Default Constructor
     * Initialize the GLFW library
     * Create a new window object
     * Set the user pointer for the input system
     * Initialize the default scene
     */
    Engine() : current_scene(this->resource_manager), event_bus(),
    editor_system(event_bus, input_manager),
    input_manager(event_bus),
    input_system(event_bus),
    resource_manager()
    {
        init();
        Instrumentation::init();
        Instrumentation::attach_event_bus(this->event_bus);

        this->window_ptr = std::make_unique<Window>(800, 600, "Engine");
        this->window_ptr->setUserPointer(&this->input_system);
        this->imgui_layer.init(this->window_ptr->get_window_ptr());
        this->initialize_default_scene();
    }

    /**
     * @brief Constructor with a window object in parameter
     * 
     * @param window : The window object
     * Initialize the GLFW library
     * Move the window object
     * Set the user pointer for the input system
     * Initialize the default scene
     */
    explicit Engine(std::unique_ptr<Window> window) : current_scene(this->resource_manager), 
    event_bus(), 
    editor_system(event_bus, input_manager),
    input_manager(event_bus),
    input_system(event_bus),
    resource_manager()
    {
        init();
        Instrumentation::init();
        Instrumentation::attach_event_bus(this->event_bus);

        this->window_ptr = std::move(window);
        this->window_ptr->setUserPointer(&this->input_system);
        this->imgui_layer.init(this->window_ptr->get_window_ptr());
        this->initialize_default_scene();
    }

    /**
     * @brief Default Destructor
     * Terminate the GLFW library and cleanup the window
     */
    ~Engine()
    {
        this->imgui_layer.shutdown();
        Instrumentation::shutdown();
        glfwTerminate();
        this->cleanup();
    }

    /**
     * @brief Main function
     * Run the engine
     * Update the input system
     * Clear the color and depth buffer
     * Set the OpenGL state for rendering
     * Update the viewports
     * Render the scene onto the screen
     * Update the window buffer
     * Show the frame rate
     */
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

            Instrumentation::begin_frame();

            {
                ENGINE_PROFILE_SCOPE("poll_events");
                this->window_ptr->poll_events();
            }

            {
                ENGINE_PROFILE_SCOPE("imgui_begin");
                this->imgui_layer.begin_frame();
            }

            int framebuffer_width = 0;
            int framebuffer_height = 0;
            glfwGetFramebufferSize(this->window_ptr->get_window_ptr(), &framebuffer_width, &framebuffer_height);

            this->editor_ui.begin_frame_ui(
                this->window_ptr->get_window_ptr(),
                EditorUIContext{
                    this->current_scene,
                    this->editor_system.get_selected_objects(),
                    [this](const entt::entity entity) {
                        this->editor_system.select_entity(entity);
                    },
                    [this]() {
                        this->editor_system.clear_selection();
                    },
                    [this](const PrimitiveType primitive_type) {
                        return this->editor_system.spawn_primitive_in_front_of_camera(
                            this->current_scene,
                            this->resource_manager,
                            primitive_type
                        );
                    },
                    [this](const entt::entity parent) {
                        return this->editor_system.create_empty_node(this->current_scene, parent);
                    },
                    [this](const entt::entity entity) {
                        this->editor_system.delete_entity(this->current_scene, entity);
                    },
                    [this](const entt::entity child, const entt::entity new_parent) {
                        this->editor_system.reparent_entity(this->current_scene, child, new_parent);
                    }
                }
            );

            ViewportLayout viewport_layout{};
            this->editor_ui.begin_viewport_panel(viewport_layout);

            this->editor_system.prepare_viewport_input(
                this->editor_ui.get_viewport_client_bounds(),
                this->editor_ui.allows_viewport_mouse_input(),
                this->editor_ui.allows_viewport_keyboard_input()
            );

            {
                ENGINE_PROFILE_SCOPE("input");
                this->input_manager.update();
            }

            this->editor_ui.end_viewport_panel([&](const ViewportLayout& layout) {
                    if (!layout.visible || layout.render_width <= 0 || layout.render_height <= 0) {
                        return;
                    }

                    ViewportFramebuffer& viewport_fbo = this->editor_ui.get_viewport_framebuffer();
                    viewport_fbo.bind();

                    float clear_red = 0.0f;
                    float clear_green = 0.0f;
                    float clear_blue = 0.0f;
                    float clear_alpha = 1.0f;
                    convert_hex_to_rgba(0x383c42, clear_red, clear_green, clear_blue, clear_alpha);
                    glClearColor(clear_red, clear_green, clear_blue, clear_alpha);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    this->window_ptr->set_depth_test(true);
                    this->window_ptr->disable_blending();

                    {
                        ENGINE_PROFILE_SCOPE("editor");
                        this->editor_system.update(
                            this->current_scene,
                            layout.render_width,
                            layout.render_height
                        );
                    }

                    const auto editor_camera = this->editor_system.get_main_camera().get_camera_data(
                        layout.render_width,
                        layout.render_height
                    );

                    {
                        ENGINE_PROFILE_SCOPE("render");
                        this->render_system.update(
                            this->current_scene,
                            editor_camera,
                            this->editor_system.get_is_editor_mode(),
                            this->editor_system.get_selected_objects(),
                            this->editor_system.get_manipulation_mode()
                        );
                    }

                    viewport_fbo.unbind(framebuffer_width, framebuffer_height);
                });

            glClearColor(red, green, blue, alpha);
            glClear(GL_COLOR_BUFFER_BIT);

            {
                ENGINE_PROFILE_SCOPE("imgui_end");
                this->imgui_layer.end_frame();
            }

            {
                ENGINE_PROFILE_SCOPE("swap");
                this->window_ptr->swap_buffers();
            }

            ENGINE_REFRESH_MEMORY(this->resource_manager, this->render_system.get_gpu_resource_manager());
            Instrumentation::end_frame(this->delta_time);
        }
    }
};

#endif //ENGINE_HPP
