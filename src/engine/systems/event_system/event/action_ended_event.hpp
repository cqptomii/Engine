#ifndef ACTION_ENDED_EVENT_HPP
#define ACTION_ENDED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include <string>

class ActionEndedEvent : public IEvent
{
private:
    std::string action_name;
public:
    ActionEndedEvent(const std::string& action_name) : action_name(action_name) {}
    ~ActionEndedEvent() override = default;

    const std::string& get_action_name() const { return action_name; }

    DEFINE_EVENT_TYPE(ActionEnded, 
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTION_ENDED_EVENT_HPP