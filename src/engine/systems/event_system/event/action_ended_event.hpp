/**
 * @file action_ended_event.hpp
 * @author Tom FRAISSE
 * @brief ActionEndedEvent Class that represent that an action is ended
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ACTION_ENDED_EVENT_HPP
#define ACTION_ENDED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_category.hpp"
#include <string>

class ActionEndedEvent : public IEvent
{
private:
    // Name of the event ended
    std::string action_name;

public:
    
    /**
     * @brief Ovverride Constructor to construct a new ActionEndedEvent based on a str : name
     * 
     * @param action_name : Name of the ActionEndedEvent
     */
    ActionEndedEvent(const std::string& action_name) : action_name(action_name) {}

    /**
     * @brief Default Destructor of the class
     */
    ~ActionEndedEvent() override = default;

    /**
     * @brief Get the name of the ActionEndedEvent
     * 
     * @return const std::string&  : Name of the ActionEndedEvent
     */
    const std::string& get_action_name() const { 
        return action_name; 
    }

    /**
     * @brief  Macro to define the event Class / Category and Type
     */
    DEFINE_EVENT(ActionEndedEvent, ActionEnded,
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTION_ENDED_EVENT_HPP