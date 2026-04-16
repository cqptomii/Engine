//
// Created by tomfr on 12/03/2026.
//

#ifndef ENGINE_EDITOR_SYSTEM_HPP
#define ENGINE_EDITOR_SYSTEM_HPP

#include "../src/engine/editor/editor_viewport.hpp"
#include "../src/engine/editor/runtime_viewport.hpp"
#include "../src/engine/editor/iviewport.hpp"
#include "../src/engine/scene/Scene.hpp"
#include "../src/engine/core/input/input_manager.hpp"

class EditorSystem
{
    // Current viewport
    IViewport* current_viewport = nullptr;

    // Editor&Runtime viewport
    std::unique_ptr<EditorViewport> editor_viewport;


    bool is_editor_mode = true;
public:
    EditorSystem()
    {
        this->is_editor_mode = true;
        this->editor_viewport = std::make_unique<EditorViewport>();
        this->current_viewport = this->editor_viewport.get();
    }
    void update(Scene& scene, InputManager& input_manager)
    {
        this->current_viewport->update(scene, input_manager);
    }

    void toggle_editor_mode()
    {
        this->is_editor_mode = !this->is_editor_mode;

        // Set the current viewport to Editor
        if (this->is_editor_mode)
        {
            this->current_viewport = this->editor_viewport.get();
        }
        // Set the current viewport to Runtime
    }
    bool get_editor_mode() const
    {
        return this->is_editor_mode;
    }

    EditorCamera& get_main_camera()
    {
        return this->current_viewport->get_main_camera();
    }
};

#endif //ENGINE_EDITOR_SYSTEM_HPP