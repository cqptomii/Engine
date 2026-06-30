/**
 * @file input_manager.hpp
 * @author Tom FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-06-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_INPUT_MANAGER_HPP
#define ENGINE_INPUT_MANAGER_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <utility>
#include <memory>
#include <unordered_map>

#include "engine/systems/event_system/event_listener.hpp"
#include "engine/core/input/context/editor_mapping_context.hpp"
#include "engine/core/input/context/input_mapping_context.hpp"
#include "engine/core/input/context/runtime_mapping_context.hpp"
#include "engine/systems/event_system/event_bus.hpp"

// Events Inludes
#include "engine/systems/event_system/event/key_pressed_event.hpp"
#include "engine/systems/event_system/event/key_released_event.hpp"
#include "engine/systems/event_system/event/mouse_motion_event.hpp"
#include "engine/systems/event_system/event/mouse_button_pressed_event.hpp"
#include "engine/systems/event_system/event/mouse_button_released_event.hpp"
#include "engine/systems/event_system/event/mouse_delta_event.hpp"
#include "engine/systems/event_system/event/mouse_scroll_event.hpp"
#include "engine/systems/event_system/event/window_resize_event.hpp"
#include "engine/systems/event_system/event/action_ended_event.hpp"
#include "engine/systems/event_system/event/action_started_event.hpp"
#include "engine/systems/event_system/event/action_performed_event.hpp"
#include "engine/systems/event_system/event_types.hpp"

/**
 * @brief Tracks the previous and current state of a keyboard key or mouse button.
 */
struct InputState
{
    bool current = false;
    bool previous = false;
};


/**
 * @brief Input Manager class to handle the input of the application
 */
class InputManager : public EventListener
{
    EventBus& event_bus;
    std::unique_ptr<InputMappingContext> input_mapping_context;

    std::unordered_map<int, InputState> input_state;

    // Mouse last position on the screen
    float last_x = 0.0f;
    float last_y = 0.0f;

    // Mouse deplacement offset between the two last positions
    float x_offset = 0.0f;
    float y_offset = 0.0f;
    bool first_movement = true;

    /**
     * @brief Check if the action is active
     * 
     * @param action_id : Action ID
     * @param previous : If true, check the previous state of the action
     * @return bool : True if the action is active, false otherwise
     */
     bool is_action_active(uint32_t action_id, bool previous = false) const
     {
         // Get the inputs for the action from the mapping context
         const auto& inputs = this->input_mapping_context->get_input_mapping().at(action_id);
 
         // Check if all the inputs are active
         for (const auto& input : inputs)
         {
             const auto key = input.get_input_key();
             
             // Find the key state in the input_state map
             auto it = input_state.find(key);
             if (it == input_state.end())
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
    /**
     * @brief Delete the default constructor to avoid duplicated bus
     */
    InputManager() = delete;

    /**
     * @brief Construct a new Input Manager object
     * 
     * @param bus : Event bus to subscribe to
     */
    InputManager(EventBus& bus) : event_bus(bus)
    {
        // By default, we are in the Editor Mode
        this->input_mapping_context = std::make_unique<InputMappingContext>(EditorMappingContext());

        // Subscribe to the event bus
        this->event_bus.add_listener(this,
            static_cast<int>(EventCategory::MouseButton) | 
            static_cast<int>(EventCategory::Mouse) | 
            static_cast<int>(EventCategory::Keyboard) | 
            static_cast<int>(EventCategory::Input)
        );
    }

    /**
     * @brief Construct a new Input Manager object
     * 
     * @param input_mapping_context : Input mapping context
     * @param bus : Event bus to subscribe to
     */
    explicit InputManager(std::unique_ptr<InputMappingContext> input_mapping_context, EventBus& bus) : event_bus(bus)
    {
        this->input_mapping_context = std::move(input_mapping_context);

        // Subscribe to the event bus
        this->event_bus.add_listener(this,
            static_cast<int>(EventCategory::MouseButton) | 
            static_cast<int>(EventCategory::Mouse) | 
            static_cast<int>(EventCategory::Keyboard) | 
            static_cast<int>(EventCategory::Input)
        );
    }

    /**
     * @brief Default Destructor of the class
     */
    ~InputManager(){
        // Remove  the input manager from the bus
        event_bus.remove_listener(this);
    };

    /**
     * @brief Delete the copy constructor to avoid duplicated bus
     */
    InputManager(const InputManager& other) = delete;
    
    /**
     * @brief Delete the assignment operator to avoid duplicated bus
     * 
     * @param other : Other input manager to assign from
     * @return InputManager& : Reference to the assigned input manager
     */
    InputManager& operator=(const InputManager& other) = delete;

    /**
     * @brief EventListener implementation
     * 
     * @param event : Event to handle
     */
    void on_event(const IEvent& event) override{
        // Check if the event is a KeyPressEvent
        if (event.get_type() == EventType::KeyPressed)
        {
            const auto& key_press_event = static_cast<const KeyPressedEvent&>(event);
            const int key = key_press_event.get_key_code();

            // Update the input state (repeat keeps the key held)
            input_state[key].current = true;
        }
        else if (event.get_type() == EventType::KeyReleased)
        {
            const auto& key_release_event = static_cast<const KeyReleasedEvent&>(event);
            const int key = key_release_event.get_key_code();

            // Update the key state
            input_state[key].current = false;
        }

        // Check if the event is a MouseButtonPressEvent
        else if (event.get_type() == EventType::MouseButtonPressed)
        {
            const auto& mouse_button_press_event = static_cast<const MouseButtonPressedEvent&>(event);
            const int button = mouse_button_press_event.get_button();

            // Update the key state
            input_state[button].current = true;
        }
        else if (event.get_type() == EventType::MouseButtonReleased)
        {
            const auto& mouse_button_release_event = static_cast<const MouseButtonReleasedEvent&>(event);
            const int button = mouse_button_release_event.get_button();

            // Update the key state
            input_state[button].current = false;
        }

        // Check if the event is a MouseMotionEvent
        else if (event.get_type() == EventType::MouseMoved)
        {
            const auto& mouse_motion_event = static_cast<const MouseMotionEvent&>(event);
            const float x_pos = mouse_motion_event.get_x();
            const float y_pos = mouse_motion_event.get_y();

            // Update the current mouse position
            float current_x_pos = x_pos;
            float current_y_pos = y_pos;

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
            MouseDeltaEvent mouse_delta_event(x_offset, y_offset);
            this->event_bus.publish_event(mouse_delta_event);
        }
    }

    /**
     * @brief Set the input context
     * 
     * @param input_context_type : Input context type
     */
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
    
    /**
     * @brief Update the input manager
     */
    void update()
    {
        // Process each action in the mapping context
        for (auto& [action_id, inputs] : this->input_mapping_context->get_input_mapping())
        {
            bool currently_active = this->is_action_active(action_id);
            bool previously_active = this->is_action_active(action_id, true);

            // ActionStarted
            if (currently_active && !previously_active)
            {
                event_bus.publish_event(ActionStartedEvent(get_action_name(action_id)));
            }

            // ActionPerformed
            if (currently_active)
            {
                event_bus.publish_event(ActionPerformedEvent(get_action_name(action_id)));
            }

            // ActionEnded
            if (!currently_active && previously_active)
            {
                event_bus.publish_event(ActionEndedEvent(get_action_name(action_id)));
            }

        }

        // Update the state of the keys
        for (auto& [key, state] : input_state)
        {
            state.previous = state.current;
        }
    }

    /**
     * @brief Get the action mapping
     * 
     * @return std::unordered_map<std::string, uint32_t> : Action mapping
     */
    std::unordered_map<std::string, uint32_t> get_action_mapping() const
    {
        return this->input_mapping_context->get_action_mapping();
    }

    /**
     * @brief Get the action name
     * 
     * @param action_id : Action ID
     * @return std::string : Action name
     */
    std::string get_action_name(const uint32_t action_id) const
    {
        return this->input_mapping_context->get_action_name(action_id);
    }
    
    /**
     * @brief Get the action ID
     * 
     * @param action_name : Action name
     * @return uint32_t : Action ID
     */
    uint32_t get_action_id(const std::string& action_name) const
    {
        return this->input_mapping_context->get_action_id(action_name);
    }
};




#endif //ENGINE_INPUT_MANAGER_HPP