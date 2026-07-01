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

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
        
        // Checks if there is no inputs
        if( inputs.empty()) return false;
         
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

    /**
     * @brief Check if the subset is a strict subset of the superset
     * 
     * @param subset_id : Subset ID
     * @param superset_id : Superset ID
     * @return true : If the subset is a strict subset of the superset
     * @return false : If the subset is not a strict subset of the superset
     */
    bool is_strict_input_subset(uint32_t subset_id, uint32_t superset_id) const
    {
        const auto& subset_inputs = this->input_mapping_context->get_input_mapping().at(subset_id);
        const auto& superset_inputs = this->input_mapping_context->get_input_mapping().at(superset_id);

        if (subset_inputs.size() >= superset_inputs.size())
        {
            return false;
        }

        for (const auto& subset_input : subset_inputs)
        {
            const bool found = std::any_of(
                superset_inputs.begin(),
                superset_inputs.end(),
                [&](const Input& superset_input) { return subset_input == superset_input; }
            );

            if (!found)
            {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief True when a more specific active action makes @p action_id redundant.
     * 
     * @param action_id : Action ID
     * @param active_actions : Active actions
     * @return true : If the action is suppressed
     * @return false : If the action is not suppressed
     */
    bool is_action_suppressed(uint32_t action_id, const std::vector<uint32_t>& active_actions) const
    {
        for (const uint32_t other_action_id : active_actions)
        {
            if (other_action_id == action_id)
            {
                continue;
            }

            if (this->is_strict_input_subset(action_id, other_action_id))
            {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Collect the active actions
     * 
     * @param previous : If true, collect the previous active actions
     * @return std::vector<uint32_t> : Active actions
     */
    std::vector<uint32_t> collect_active_actions(bool previous = false) const
    {
        std::vector<uint32_t> active_actions;

        for (const auto& [action_id, inputs] : this->input_mapping_context->get_input_mapping())
        {
            if (inputs.empty())
            {
                continue;
            }

            if (this->is_action_active(action_id, previous))
            {
                active_actions.push_back(action_id);
            }
        }

        return active_actions;
    }

    /**
     * @brief Check if the action is effectively active
     * 
     * @param action_id : Action ID
     * @param raw_active : Raw active actions
     * @return true : If the action is effectively active
     * @return false : If the action is not effectively active
     */
    bool is_effectively_active(uint32_t action_id, const std::vector<uint32_t>& raw_active) const
    {
        return this->is_action_active(action_id)
            && !this->is_action_suppressed(action_id, raw_active);
    }

    /**
     * @brief Check if the action was effectively active
     * 
     * @param action_id : Action ID
     * @param raw_active_previous : Raw active actions previous
     * @return true : If the action was effectively active in the previous frame
     * @return false : If the action was not effectively active in the previous frame
     */
    bool was_effectively_active(uint32_t action_id, const std::vector<uint32_t>& raw_active_previous) const
    {
        return this->is_action_active(action_id, true)
            && !this->is_action_suppressed(action_id, raw_active_previous);
    }

    /**
     * @brief Publish the mouse delta event if the offset is not zero
     * 
     * @param x_offset : X offset
     * @param y_offset : Y offset
     */
    void publish_mouse_delta_event(const float x_offset, const float y_offset) const{
        if (x_offset != 0.f || y_offset != 0.f)
        {
            MouseDeltaEvent mouse_delta_event(x_offset, y_offset);
            this->event_bus.publish_event(mouse_delta_event);
        }
    }

    /**
     * @brief Publish the action events if the action is effectively active or not
     * 
     */
    void publish_action_event() const{
        const std::vector<uint32_t> raw_active = this->collect_active_actions();
        const std::vector<uint32_t> raw_active_previous = this->collect_active_actions(true);

        for (const auto& [action_id, inputs] : this->input_mapping_context->get_input_mapping())
        {
            if (inputs.empty())
            {
                continue;
            }

            const bool effectively_active = this->is_effectively_active(action_id, raw_active);
            const bool effectively_active_previous = this->was_effectively_active(action_id, raw_active_previous);

            if (effectively_active && !effectively_active_previous)
            {
                this->event_bus.publish_event(ActionStartedEvent(get_action_name(action_id)));
            }

            if (effectively_active)
            {
                this->event_bus.publish_event(ActionPerformedEvent(get_action_name(action_id)));
            }

            if (!effectively_active && effectively_active_previous)
            {
                this->event_bus.publish_event(ActionEndedEvent(get_action_name(action_id)));
            }
        }
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
     * @brief Update the input manager to handle the input events
     * 
     * This function is called every frame to update the input manager and handle the input events
     */
    void update()
    {
        // Publish action event
        this->publish_action_event();

        // Publish mouse delta event
        this->publish_mouse_delta_event(x_offset, y_offset);

        for (auto& [key, state] : input_state)
        {
            state.previous = state.current;
        }

        x_offset = 0.f;
        y_offset = 0.f;
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

    /**
     * @brief Check if the key is held
     * 
     * @param key : Key code
     * @return true : If the key is held
     * @return false : If the key is not held
     */
    bool is_held(int key) const
    {
        auto it = input_state.find(key);
        return it != input_state.end() && it->second.current;
    }

    /**
     * @brief Check if the key is pressed
     * 
     * @param key : Key code
     * @return true : If the key is pressed
     * @return false : If the key is not pressed (e.g. the key was released last frame)
     */
    bool is_pressed(int key) const
    {
        auto it = input_state.find(key);
        return it != input_state.end() && it->second.current && !it->second.previous;
    }

    /**
     * @brief Check if the key is released
     * 
     * @param key : Key code
     * @return true : If the key is released
     * @return false : If the key is not released (e.g. the key was pressed this frame)
     */
    bool is_released(int key) const
    {
        auto it = input_state.find(key);
        return it != input_state.end() && !it->second.current && it->second.previous;
    }
};




#endif //ENGINE_INPUT_MANAGER_HPP