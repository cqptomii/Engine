/**
 * @file action_performed_event.hpp
 * @author Tom FRAISSE
 * @brief ActionPerformedEvent that represent that an action is performed
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ACTIONPERFORMED_EVENT_HPP
#define ACTIONPERFORMED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_category.hpp"
#include <string>

class ActionPerformedEvent : public IEvent
{
private:
    // Name of the action performed
    std::string action_name;

public:
    
    /**
     * @brief Ovverride Constructor to construct a new ActionPerformedEvent based on a str : name
     * 
     * @param action_name : Name of the ActionPerformedEvent
     */
    ActionPerformedEvent(const std::string& action_name) : action_name(action_name) {}
    
    ActionPerformedEvent(const ActionPerformedEvent&) = default;
    ActionPerformedEvent& operator=(const ActionPerformedEvent&) = default;
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~ActionPerformedEvent() override = default;

    /**
     * @brief Get the event action name
     * 
     * @return const std::string& : Name of the action event
     */
    const std::string& get_action_name() const { return action_name; }


    /**
     * @brief  Macro to define the event Class / Category and Type
     */
    DEFINE_EVENT(ActionPerformedEvent, ActionPerformed, 
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTIONPERFORMED_EVENT_HPP