//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_INPUT_MANAGER_HPP
#define ENGINE_INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <unordered_set>
#include <string>
#include "input_mapping_context.hpp"

class InputManager
{
    InputMappingContext input_mapping_context;
    std::unordered_set<std::string> active_actions;
public:

    InputManager() = default;
    InputManager(const InputMappingContext& input_mapping_context) : input_mapping_context(input_mapping_context) {}
    InputManager(const InputManager& other) = default;
    ~InputManager() = default;

    void set_input_context(const InputMappingContext& context)
    {
        this->input_mapping_context = context;
    }
    bool is_action_active(const std::string& action_name) const
    {
        return active_actions.contains(action_name);
    }

    void process_input(GLFWwindow* window)
    {
        for (auto& [action_id, inputs] : this->input_mapping_context.get_input_mapping())
        {
            for (Input& input : inputs)
            {
                const int input_key = input.get_input_key();
                DeviceType device_type = input.get_input_type();

                if (device_type == KEYBOARD && glfwGetKey(window, input_key) == GLFW_PRESS)
                {
                    this->active_actions.insert(this->input_mapping_context.get_action_name(action_id));
                }else if(device_type == MOUSE && glfwGetMouseButton(window, input_key) == GLFW_PRESS)
                {
                    this->active_actions.insert(this->input_mapping_context.get_action_name(action_id));
                }
            }
        }
    }


};



#endif //ENGINE_INPUT_MANAGER_HPP