/**
 * @file editor_system.hpp
 * @author Tom FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-03-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_EDITOR_SYSTEM_HPP
#define ENGINE_EDITOR_SYSTEM_HPP

#include <algorithm>
#include <vector>

#include <entt/entt.hpp>

#include "engine/editor/editor_viewport.hpp"
#include "engine/editor/runtime_viewport.hpp"
#include "engine/editor/iviewport.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/core/event/event_bus.hpp"
#include "engine/core/input/input_manager.hpp"

#include "engine/editor/ui/viewport_layout.hpp"
#include "engine/editor/ui/editor_context.hpp"
#include "engine/editor/editor_scene_factory.hpp"
#include "engine/scene/scene_hierarchy.hpp"
#include "engine/scene/scene_serializer.hpp"

#include <glm/glm.hpp>

class EditorSystem
{
    // Current viewport
    IViewport* current_viewport = nullptr;

    // Editor&Runtime viewport
    std::unique_ptr<EditorViewport> editor_viewport;
    std::unique_ptr<RuntimeViewport> runtime_viewport;

    // EventBus object
    EventBus& event_bus;

    // Input Manager Object
    InputManager& input_manager;


    bool is_editor_mode = true;
public:
    
    /**
     * @brief Delete the default constructor to avoid eventBus duplication
     * 
     */
    EditorSystem() = delete;

    /**
     * @brief Construct a new Editor System object
     * 
     * @param bus (EventBus&) : EventBus reference
     * @param input_manager (InputManager&) : InputManager reference
     */
    EditorSystem(EventBus& bus, InputManager& input_manager) : event_bus(bus), input_manager(input_manager)
    {
        this->is_editor_mode = true;
        this->editor_viewport = std::make_unique<EditorViewport>(bus, input_manager);
        this->runtime_viewport = std::make_unique<RuntimeViewport>();
        this->current_viewport = this->editor_viewport.get();
    }

    /**
     * @brief Configure editor viewport input using the previous UI frame state.
     */
    void prepare_viewport_input(
        const ViewportClientBounds& bounds,
        const bool mouse_input_enabled,
        const bool keyboard_input_enabled)
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->set_viewport_input_context(bounds, mouse_input_enabled, keyboard_input_enabled);
        }
    }

    /**
     * @brief Update the editor system
     * 
     * @param scene (Scene&) : Scene reference to update
     * @param viewport_width Render target width of the viewport panel
     * @param viewport_height Render target height of the viewport panel
     */
    void update(Scene& scene, const int viewport_width, const int viewport_height)
    {
        if (this->is_editor_mode)
        {
            this->editor_viewport->set_viewport_render_size(viewport_width, viewport_height);
        }

        this->current_viewport->update(scene);
    }

    /**
     * @brief Method to toggle / un toggle the editor mode
     * 
     */
    void toggle_editor_mode()
    {
        this->is_editor_mode = !this->is_editor_mode;

        // Set the current viewport to Editor
        if (this->is_editor_mode)
        {
            this->current_viewport = this->editor_viewport.get();
        }
        else
        {
            this->current_viewport = this->runtime_viewport.get();
        }
    }

    /**
     * @brief Get the status of the editor system
     * 
     * @return true : We are in editor mode
     * @return false : We aren't in editor mode
     */
    bool get_is_editor_mode() const
    {
        return this->is_editor_mode;
    }

    /**
     * @brief Get the main camera object
     * 
     * @return EditorCamera& : Editor Camera object
     */
    EditorCamera& get_main_camera()
    {
        return this->current_viewport->get_main_camera();
    }

    /**
     * @brief Get entities currently selected in the editor viewport.
     */
    std::vector<entt::entity> get_selected_objects() const
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            return this->editor_viewport->get_selected_objects();
        }

        return {};
    }

    /**
     * @brief Get the active manipulation mode of the editor viewport.
     *
     * Used by the renderer to decide which transform gizmo to draw.
     */
    ManipulationMode get_manipulation_mode() const
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            return this->editor_viewport->get_manipulation_mode();
        }

        return ManipulationMode::NONE;
    }

    void set_manipulation_mode(const ManipulationMode mode)
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->set_manipulation_mode(mode);
        }
    }

    void select_entity(const entt::entity entity)
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->select_entity(entity);
        }
    }

    void clear_selection()
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->clear_selection();
        }
    }

    entt::entity spawn_primitive_in_front_of_camera(
        Scene& scene,
        CpuResourceManager& resource_manager,
        const PrimitiveType primitive_type)
    {
        if (!this->is_editor_mode || !this->editor_viewport)
        {
            return entt::null;
        }

        const entt::entity parent = scene_hierarchy::resolve_spawn_parent(
            scene,
            this->editor_viewport->get_selected_objects()
        );

        EditorCamera& camera = this->editor_viewport->get_main_camera();
        const glm::vec3 world_spawn_position = camera.get_position() + camera.get_direction() * 4.0f;
        const glm::vec3 local_spawn_position = scene_hierarchy::world_to_parent_local(
            scene,
            parent,
            world_spawn_position
        );

        const entt::entity spawned_entity = EditorSceneFactory::spawn_primitive(
            scene,
            resource_manager,
            primitive_type,
            local_spawn_position,
            parent
        );

        if (spawned_entity != entt::null)
        {
            this->editor_viewport->select_entity(spawned_entity);
        }

        return spawned_entity;
    }

    entt::entity create_empty_node(Scene& scene, const entt::entity parent)
    {
        const entt::entity resolved_parent = parent != entt::null
            ? parent
            : scene_hierarchy::resolve_spawn_parent(scene, this->get_selected_objects());

        const entt::entity created_entity = scene_hierarchy::create_empty_node(scene, resolved_parent);

        if (this->is_editor_mode && this->editor_viewport && created_entity != entt::null)
        {
            this->editor_viewport->select_entity(created_entity);
        }

        return created_entity;
    }

    void delete_entity(Scene& scene, const entt::entity entity)
    {
        if (!this->is_editor_mode || !this->editor_viewport)
        {
            return;
        }

        const auto& selected = this->editor_viewport->get_selected_objects();
        if (std::find(selected.begin(), selected.end(), entity) != selected.end())
        {
            this->editor_viewport->clear_selection();
        }

        scene_hierarchy::delete_entity(scene, entity);
    }

    void reparent_entity(Scene& scene, const entt::entity child, const entt::entity new_parent)
    {
        scene_hierarchy::set_parent(scene, child, new_parent);
    }

    bool rename_entity(Scene& scene, const entt::entity entity, const std::string& new_name)
    {
        return scene_hierarchy::rename_entity(scene, entity, new_name);
    }

    void new_scene(Scene& scene)
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->clear_selection();
        }

        scene_hierarchy::clear_scene(scene);
        (void)scene_hierarchy::ensure_root(scene);
    }

    bool save_scene(Scene& scene, CpuResourceManager& resource_manager, const std::string& file_path)
    {
        return scene_serializer::save_scene(scene, resource_manager, file_path);
    }

    bool load_scene(Scene& scene, CpuResourceManager& resource_manager, const std::string& file_path)
    {
        if (!scene_serializer::load_scene(scene, resource_manager, file_path))
        {
            return false;
        }

        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->clear_selection();
        }

        return true;
    }
};

#endif //ENGINE_EDITOR_SYSTEM_HPP