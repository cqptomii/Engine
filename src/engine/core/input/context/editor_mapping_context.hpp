//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_EDITOR_MAPPING_CONTEXT_HPP
#define ENGINE_EDITOR_MAPPING_CONTEXT_HPP

#include "../input_mapping_context.hpp"
#include <GLFW/glfw3.h>

class EditorMappingContext : public InputMappingContext
{
public:
    EditorMappingContext()
    {
        // Camera movement
        this->register_input_mapping("move_camera", {Input(GLFW_KEY_LEFT_SHIFT, KEYBOARD), Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)});
        this->register_input_mapping("rotate_camera", {Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)});
        this->register_input_mapping("camera_forward", {Input(GLFW_KEY_S, KEYBOARD)});
        this->register_input_mapping("camera_backward", {Input(GLFW_KEY_W, KEYBOARD)});
        //  Editor Shortcut
    }
};

#endif //ENGINE_EDITOR_MAPPING_CONTEXT_HPP