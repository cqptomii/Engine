//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_EDITOR_MAPPING_CONTEXT_HPP
#define ENGINE_EDITOR_MAPPING_CONTEXT_HPP

#include "engine/core/input/context/input_mapping_context.hpp"
#include <GLFW/glfw3.h>

class EditorMappingContext : public InputMappingContext
{
public:
    EditorMappingContext()
    {
        // Camera movement
        this->register_input_mapping("move_camera", {
            Input(GLFW_KEY_LEFT_SHIFT, KEYBOARD), 
            Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)
        });
        this->register_input_mapping("rotate_camera", {
            Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)
        });
        this->register_input_mapping("reset_camera", {
            Input(GLFW_KEY_R, KEYBOARD)
        });

        // Object Picking
        this->register_input_mapping("pick_object", {
            Input(GLFW_MOUSE_BUTTON_LEFT, MOUSE)
        });
        // Object Manipulation
        this->register_input_mapping("translate_object", {
            Input(GLFW_KEY_G, KEYBOARD)
        });
        this->register_input_mapping("rotate_object", {
            Input(GLFW_KEY_R, KEYBOARD)
        });
        this->register_input_mapping("scale_object", {
            Input(GLFW_KEY_S, KEYBOARD)
        });

        //  Editor Shortcut
    }
};

#endif //ENGINE_EDITOR_MAPPING_CONTEXT_HPP