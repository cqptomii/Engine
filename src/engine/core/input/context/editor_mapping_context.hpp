//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_EDITOR_MAPPING_CONTEXT_HPP
#define ENGINE_EDITOR_MAPPING_CONTEXT_HPP

#include "engine/core/input/context/input_mapping_context.hpp"
#include "engine/core/input/input.hpp"
#include <GLFW/glfw3.h>

class EditorMappingContext : public InputMappingContext
{
public:
    EditorMappingContext()
    {
        // Camera Translation mappings with mouse
        this->register_input_mapping("camera_vector_move", {
            Input(GLFW_KEY_LEFT_SHIFT, KEYBOARD), 
            Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)
        });

        // Camera Translation mappings with keyboard
        this->register_input_mapping("camera_move_left", {
            Input(GLFW_KEY_LEFT, KEYBOARD)
        });
        this->register_input_mapping("camera_move_right", {
            Input(GLFW_KEY_RIGHT, KEYBOARD)
        });
        this->register_input_mapping("camera_move_top", {
            Input(GLFW_KEY_UP, KEYBOARD)
        });
        this->register_input_mapping("camera_move_bottom", {
            Input(GLFW_KEY_DOWN, KEYBOARD)
        });
        
        // Camera rotation mapping
        this->register_input_mapping("rotate_camera", {
            Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)
        });

        // Camera reset mapping
        this->register_input_mapping("reset_camera", {
            Input(GLFW_KEY_LEFT_ALT, KEYBOARD),
            Input(GLFW_KEY_R, KEYBOARD)
        });

        // Object Picking
        this->register_input_mapping("pick_one_object", {
            Input(GLFW_MOUSE_BUTTON_LEFT, MOUSE)
        });
        this->register_input_mapping("pick_multiple_objects", {
            Input(GLFW_MOUSE_BUTTON_LEFT, MOUSE),
            Input(GLFW_KEY_LEFT_SHIFT, KEYBOARD)
        });
        this->register_input_mapping("pick_all_objects", {
            Input(GLFW_KEY_A, KEYBOARD)
        });

        // Object Manipulation
        this->register_input_mapping("manipulation_mode_translate", {
            Input(GLFW_KEY_G, KEYBOARD)
        });
        this->register_input_mapping("manipulation_mode_rotate", {
            Input(GLFW_KEY_R, KEYBOARD)
        });
        this->register_input_mapping("manipulation_mode_scale", {
            Input(GLFW_KEY_S, KEYBOARD)
        });

        this->register_input_mapping("manipulation_mode_none", {
            Input(GLFW_KEY_ESCAPE, KEYBOARD)
        });

        //  Editor Shortcut
    }
};

#endif //ENGINE_EDITOR_MAPPING_CONTEXT_HPP