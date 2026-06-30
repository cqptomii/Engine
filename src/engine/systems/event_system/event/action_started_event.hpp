/**
 * @file action_started_event.hpp
 * @author Tom FRAISSE
 * @brief ActionStartedEvent that represent the start of an action
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ACTION_STARTED_EVENT_HPP
#define ACTION_STARTED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_category.hpp"
#include <string>

class ActionStartedEvent : public IEvent
{
private:
    // Name of the ActionStartedEvent 
    std::string action_name;
public:
    /**
     * @brief Ovverride Constructor to construct a new ActionStartedEvent based on a str : name
     * 
     * @param action_name : Name of the ActionStartedEvent
     */
    ActionStartedEvent(const std::string& action_name) : action_name(action_name) {}
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~ActionStartedEvent() override = default;

    /**
     * @brief Get the event action name
     * 
     * @return const std::string& : Name of the event action
     */
    const std::string& get_action_name() const { return action_name; }

    
    /**
     * @brief  Macro to define the Event Class / Category / Typ
     * 
     */
    DEFINE_EVENT(
        ActionStartedEvent, 
        ActionStarted, 
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTION_STARTED_EVENT_HPP