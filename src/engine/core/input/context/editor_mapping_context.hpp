//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_EDITOR_MAPPING_CONTEXT_HPP
#define ENGINE_EDITOR_MAPPING_CONTEXT_HPP

#include "../input_mapping_context.hpp"
#include <GLFW/glfw3.h>

class EditorMappingContext : public InputMappingContext
{
    EditorMappingContext()
    {
        // Camera movement
        this->register_input_mapping("CameraTranslate", {Input(GLFW_KEY_LEFT_SHIFT, KEYBOARD)});
        this->register_input_mapping("CameraRotate", {Input(GLFW_MOUSE_BUTTON_MIDDLE, MOUSE)});

        //  Editor Shortcut
    }
};

#endif //ENGINE_EDITOR_MAPPING_CONTEXT_HPP