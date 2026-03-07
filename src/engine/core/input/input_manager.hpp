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
    glm::vec2 mouse_position = glm::vec2(0.0f);
    glm::vec2 mouse_delta = glm::vec2(0.0f);
    float scroll_delta = 0.0f;
    bool first_movement = true;

public:

    InputManager()
    {
        // By default, we are in the Editor Mode
        this->input_mapping_context = std::make_unique<InputMappingContext>();
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
            for (Input& input : inputs)
            {
                const int input_key = input.get_input_key();

                if (!key_state.at(input_key))
                {
                    is_active = false;
                }
            }

            // Add the action only if all the inputs are triggered
            if (is_active)
            {
                this->active_actions.insert(action_id);
                std::cout << "Action " << this->input_mapping_context->get_action_name(action_id) << " triggered" << std::endl;
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
        key_state[button] = action;
    }
    void on_mouse_scroll(double x_offset, double y_offset)
    {
        this->scroll_delta = static_cast<float>(y_offset);
    }
    void on_mouse_move(const double x_pos, const double y_pos)
    {
        const auto current_x_pos = static_cast<float>(x_pos);
        const auto current_y_pos = static_cast<float>(y_pos);

        if (this->first_movement)
        {
            this->mouse_position = glm::vec2(current_x_pos, current_y_pos);
            this->first_movement = false;
        }


        this->mouse_delta = glm::vec2(current_x_pos - this->mouse_position.x, current_y_pos - this->mouse_position.y);
    }

    float get_scroll_delta()
    {
        const float delta = this->scroll_delta;
        this->scroll_delta = 0.0f;
        return delta;
    }
    glm::vec2 get_mouse_delta()
    {
        auto delta = glm::vec2(0.0f);
        this->mouse_delta = glm::vec2(0.0f);
        return delta;
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