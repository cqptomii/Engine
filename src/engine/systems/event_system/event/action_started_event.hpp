#ifndef ACTION_STARTED_EVENT_HPP
#define ACTION_STARTED_EVENT_HPP

#include "engine/systems/event_system/event/IEvent.hpp"

class ActionStartedEvent : public IEvent
{
private:
    std::string action_name;
public:
    ActionStartedEvent(const std::string& action_name) : action_name(action_name) {}
    ~ActionStartedEvent() override = default;

    const std::string& get_action_name() const { return action_name; }

    DEFINE_EVENT_TYPE(ActionStartedEvent, 
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTION_STARTED_EVENT_HPP