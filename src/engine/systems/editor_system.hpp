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

#include <vector>

#include <entt/entt.hpp>

#include "engine/editor/editor_viewport.hpp"
#include "engine/editor/runtime_viewport.hpp"
#include "engine/editor/iviewport.hpp"
#include "engine/scene/Scene.hpp"
#include "engine/core/event/event_bus.hpp"
#include "engine/core/input/input_manager.hpp"

#include "engine/editor/ui/viewport_layout.hpp"

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
    void prepare_viewport_input(const ViewportClientBounds& bounds, const bool input_enabled)
    {
        if (this->is_editor_mode && this->editor_viewport)
        {
            this->editor_viewport->set_viewport_input_context(bounds, input_enabled);
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
};

#endif //ENGINE_EDITOR_SYSTEM_HPP