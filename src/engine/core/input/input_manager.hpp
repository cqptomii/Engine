//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_INPUT_MANAGER_HPP
#define ENGINE_INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <unordered_set>
#include <string>
#include "input_mapping_context.hpp"
#include <glm/glm.hpp>
#include "context/editor_mapping_context.hpp"
#include "context/runtime_mapping_context.hpp"

class InputManager
{
    std::unique_ptr<InputMappingContext> input_mapping_context;

    std::unordered_set<uint32_t> active_actions;
    std::unordered_map<int, int> key_state;

    // mouse data
    float last_x = 0.0f;
    float last_y = 0.0f;

    float x_offset = 0.0f;
    float y_offset = 0.0f;
    bool first_movement = true;

    float scroll_delta = 0.0f;


public:

    InputManager()
    {
        // By default, we are in the Editor Mode
        this->input_mapping_context = std::make_unique<InputMappingContext>(EditorMappingContext());
        this->input_mapping_context->debug_mapping();
    }
    explicit InputManager(std::unique_ptr<InputMappingContext> input_mapping_context)
    {
        this->input_mapping_context = std::move(input_mapping_context);
    }
    ~InputManager() = default;

    // Input Context System
    void set_input_context(const MappingContextType input_context_type)
    {
        if (input_context_type == EDITOR)
        {
            this->input_mapping_context = std::make_unique<InputMappingContext>(EditorMappingContext());
        }else if(input_context_type == RUNTIME)
        {
            this->input_mapping_context = std::make_unique<InputMappingContext>(RuntimeMappingContext());
        }
    }

    void update()
    {
        // reset active actions set
        this->clear_actions();

        for (auto& [action_id, inputs] : this->input_mapping_context->get_input_mapping())
        {
            bool is_active = true;

            std::unordered_set<int> pressed_keys;
            for (const auto& [key, state] : key_state)
            {
                if (state != GLFW_RELEASE)
                {
                    pressed_keys.insert(key);
                }
            }



            for (Input& input : inputs)
            {
                if (!pressed_keys.contains(input.get_input_key()))
                {
                    is_active = false;
                    break;
                }
            }

            // Add the action only if all the inputs are triggered
            if (is_active)
            {
                if (pressed_keys.size() == inputs.size())
                {
                    this->active_actions.insert(action_id);
                }
            }

        }
    }


    // GLFW callbacks functions
    void on_key_event(int key, int scancode, int action, int mods)
    {
        std::cout << "Key " << key << " " << action << std::endl;
        key_state[key] = action;
    }
    void on_mouse_button_event(int button, int action, int mods)
    {
        std::cout << "Mouse button " << button << " " << action << std::endl;
        key_state[button] = action;
    }
    void on_mouse_scroll(double xoffset, double yoffset)
    {
        this->scroll_delta = static_cast<float>(yoffset);
    }
    void on_mouse_move(const double x_pos, const double y_pos)
    {
        const auto current_x_pos = static_cast<float>(x_pos);
        const auto current_y_pos = static_cast<float>(y_pos);

        // Update last position

        if (this->first_movement)
        {
            last_x = current_x_pos;
            last_y = current_y_pos;
            this->first_movement = false;
        }

        x_offset = current_x_pos - last_x;
        y_offset = last_y - current_y_pos;

        last_x = current_x_pos;
        last_y = current_y_pos;
    }

    float get_scroll_delta()
    {
        const float delta = this->scroll_delta;
        this->scroll_delta = 0.0f;
        return delta;
    }
    glm::vec2 get_mouse_offset()
    {
        glm::vec2 offset = {x_offset, y_offset};

        x_offset = 0;
        y_offset = 0;

        return offset;
    }


    // Actions
    std::unordered_set<uint32_t>& get_active_actions()
    {
        return active_actions;
    }
    std::unordered_map<std::string, uint32_t> get_action_mapping() const
    {
        return this->input_mapping_context->get_action_mapping();
    }
    std::string get_action_name(const uint32_t action_id) const
    {
        return this->input_mapping_context->get_action_name(action_id);
    }
    uint32_t get_action_id(const std::string& action_name) const
    {
        return this->input_mapping_context->get_action_id(action_name);
    }
    bool is_action_active(const uint32_t action_id) const
    {
        return active_actions.contains(action_id);
    }
    void clear_actions()
    {
        active_actions.clear();
    }
};



#endif //ENGINE_INPUT_MANAGER_HPP