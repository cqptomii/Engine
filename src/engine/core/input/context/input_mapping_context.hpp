//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_INPUT_MAPPING_CONTEXT_HPP
#define ENGINE_INPUT_MAPPING_CONTEXT_HPP

#include <unordered_map>
#include <vector>
#include <iostream>
#include <ostream>
#include <string>
#include <cstdint>
#include "engine/core/input/input.hpp"

enum MappingContextType
{
    EDITOR,
    RUNTIME
};

class InputMappingContext
{
    std::unordered_map<std::string, uint32_t> action_mapping;
    std::vector<std::string> action_names;
    std::unordered_map<uint32_t, std::vector<Input>> input_mapping;

    // Action mapping
    uint32_t register_action(const std::string& action_name)
    {
        if (!action_mapping.contains(action_name))
        {
            const uint32_t id = action_names.size();
            action_mapping[action_name] = id;
            action_names.push_back(action_name);
            return id;
        }

        return action_mapping[action_name];
    }

public:
    InputMappingContext() = default;
    InputMappingContext(const InputMappingContext& other) = default;
    ~InputMappingContext() = default;

    // Input mapping
    void register_input(const std::string& action_name, const Input& input)
    {
        const uint32_t action_id = this->register_action(action_name);
        auto& mapping = this->input_mapping[action_id];

        if (std::ranges::find(mapping, input) == mapping.end())
        {
            mapping.push_back(input);
        }
    }
    void register_input_mapping(const std::string& action_name, const std::vector<Input>& inputs)
    {
        const uint32_t action_id = this->register_action(action_name);
        auto& mapping = this->input_mapping[action_id];

        for (const auto& input : inputs)
        {
            if (std::ranges::find(mapping, input) == mapping.end())
            {
                mapping.push_back(input);
            }
        }
    }

    void unregister_input_mapping(const std::string& action_name, const std::vector<Input>& inputs)
    {
        const uint32_t action_id = this->register_action(action_name);
        auto& mapping = this->input_mapping[action_id];
        for (const auto& input : inputs)
        {
            mapping.erase(std::ranges::find(mapping, input));
        }
    }
    void unregister_input(const std::string& action_name, const Input& input)
    {
        const uint32_t action_id = this->register_action(action_name);
        auto& mapping = this->input_mapping[action_id];
        mapping.erase(std::ranges::find(mapping, input));
    }

    uint32_t get_action_id(const std::string& action_name) const
    {
        return action_mapping.at(action_name);
    }
    std::string get_action_name(const uint32_t action_id)
    {
        return this->action_names.at(action_id);
    }
    std::unordered_map<std::string, uint32_t>& get_action_mapping()
    {
        return this->action_mapping;
    }
    std::unordered_map<uint32_t, std::vector<Input>>& get_input_mapping()
    {
        return this->input_mapping;
    }
    std::vector<Input> get_inputs(const std::string& action_name)
    {
        return this->input_mapping.at(this->get_action_id(action_name));
    }

    void debug_mapping()
    {
        for (auto& [action_id, inputs] : this->get_input_mapping())
        {
            std::cout << "Action ID: " << action_id << std::endl;
            for (const auto& input : inputs)
            {
                std::cout << "Input: " << input.get_input_key() << std::endl;
            }
        }
    }
};

#endif //ENGINE_INPUT_MAPPING_CONTEXT_HPP