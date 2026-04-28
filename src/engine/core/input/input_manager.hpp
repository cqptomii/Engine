//
// Created by tomfr on 06/03/2026.
//

#ifndef ENGINE_INPUT_MANAGER_HPP
#define ENGINE_INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <unordered_set>
#include <string>
#include <glm/glm.hpp>
#include "engine/core/input/context/input_mapping_context.hpp"
#include "engine/core/input/context/editor_mapping_context.hpp"
#include "engine/core/input/context/runtime_mapping_context.hpp"
#include "engine/systems/event_system/event_bus.hpp"
#include "engine/systems/event_system/event_listener.hpp"
#include "engine/systems/event_system/event/mouse_delta_event.hpp"
#include "engine/systems/event_system/event/action_started_event.hpp
#include "engine/systems/event_system/event/action_performed_event.hpp
#include "engine/systems/event_system/event/action_ended_event.hpp"
#include "engine/systems/event_system/event_category.hpp"

// Key state struct to keep track of the current and previous state of a key
struct KeyState
{
    bool current = false;
    bool previous = false;
}

class InputManager : public EventListener
{
    EventBus event_bus;
    std::unique_ptr<InputMappingContext> input_mapping_context;

    std::unordered_map<int, KeyState> key_state;


    // mouse data
    float last_x = 0.0f;
    float last_y = 0.0f;

    float x_offset = 0.0f;
    float y_offset = 0.0f;
    bool first_movement = true;


    bool is_action_active(uint32_t action_id, bool previous = false) const
    {
        // Get the inputs for the action from the mapping context
        const auto& inputs = this->input_mapping_context->get_input_mapping().at(action_id);

        // Check if all the inputs are active
        for (const auto& input : inputs)
        {
            const auto key = input.get_input_key();

            // Find the key state in the key_state map
            auto it = key_state.find(key);
            if (it == key_state.end())
            {
                return false;
            }

            bool state = previous ? it->second.previous : it->second.current;
            if (!state)
            {
                return false;
            }
        }

        return true;
    }


public:

    InputManager() : event_bus(nullptr)
    {
        // By default, we are in the Editor Mode
        this->input_mapping_context = std::make_unique<InputMappingContext>(EditorMappingContext());
        this->input_mapping_context->debug_mapping();
    }
    InputManager(EventBus& bus) : event_bus(bus)
    {
        // By default, we are in the Editor Mode
        this->input_mapping_context = std::make_unique<InputMappingContext>(EditorMappingContext());
        this->input_mapping_context->debug_mapping();

        // Subscribe to the event bus
        this->event_bus.add_listener(this,
            static_cast<int>(EventCategory::MouseButton) | 
            static_cast<int>(EventCategory::Mouse) | 
            static_cast<int>(EventCategory::Keyboard) | 
            static_cast<int>(EventCategory::Input)
        );
    }
    explicit InputManager(std::unique_ptr<InputMappingContext> input_mapping_context, EventBus& bus) : event_bus(bus)
    {
        this->input_mapping_context = std::move(input_mapping_context);
    }
    ~InputManager() = default;

    InputManager(const InputManager& other) = delete;
    InputManager& operator=(const InputManager& other) = delete;

    // EventListener implementation
    void on_event(const IEvent& event) override{
        // Check if the event is a KeyPressEvent
        if (event.get_type() == EventType::KEY_PRESS)
        {
            const auto& key_press_event = static_cast<const KeyPressEvent&>(event);
            const int key = key_press_event.get_key();
            const bool is_repeat = key_press_event.is_repeat();

            // Update the key state
            key_state[key].current = true;
        }
        else if (event.get_type() == EventType::KEY_RELEASE)
        {
            const auto& key_release_event = static_cast<const KeyReleaseEvent&>(event);
            const int key = key_release_event.get_key();

            // Update the key state
            key_state[key].current = false;
        }

        // Check if the event is a MouseButtonPressEvent
        else if (event.get_type() == EventType::MOUSE_BUTTON_PRESS)
        {
            const auto& mouse_button_press_event = static_cast<const MouseButtonPressEvent&>(event);
            const int button = mouse_button_press_event.get_button();

            // Update the key state
            key_state[button].current = true;
        }
        else if (event.get_type() == EventType::MOUSE_BUTTON_RELEASE)
        {
            const auto& mouse_button_release_event = static_cast<const MouseButtonReleaseEvent&>(event);
            const int button = mouse_button_release_event.get_button();

            // Update the key state
            key_state[button].current = false;
        }

        // Check if the event is a MouseMotionEvent
        else if (event.get_type() == EventType::MOUSE_MOTION)
        {
            const auto& mouse_motion_event = static_cast<const MouseMotionEvent&>(event);
            const float x_pos = mouse_motion_event.get_x_pos();
            const float y_pos = mouse_motion_event.get_y_pos();

            // Update the current mouse position
            current_x_pos = x_pos;
            current_y_pos = y_pos;

            // Handle mouse offset event
            if (this->first_movement)
            {
                last_x = current_x_pos;
                last_y = current_y_pos;
                this->first_movement = false;
            }

            x_offset = current_x_pos - last_x;
            y_offset = current_y_pos - last_y;

            last_x = current_x_pos;
            last_y = current_y_pos;

            // Publish offset event
            MouseOffsetEvent mouse_offset_event(x_offset, y_offset);
            this->event_bus.publish_event(mouse_offset_event);
        }

    }

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
        // Update the state of the keys
        for (auto& [key, state] : key_state)
        {
            state.previous = state.current;
        }


        // Process each action in the mapping context
        for (auto& [action_id, inputs] : this->input_mapping_context->get_input_mapping())
        {
            bool currently_active = this->is_action_active(action_id);
            bool previously_active = this->is_action_active(action_id, true);

            // ActionStarted
            if (currently_active && !previously_active)
            {
                event_bus.publish_event(ActionStartedEvent(action_id));
            }

            // ActionPerformed
            if (currently_active)
            {
                event_bus.publish_event(ActionPerformedEvent(action_id));
            }

            // ActionEnded
            if (!currently_active && previously_active)
            {
                event_bus.publish_event(ActionEndedEvent(action_id));
            }

        }
    }


    // Actions
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
};




#endif //ENGINE_INPUT_MANAGER_HPP