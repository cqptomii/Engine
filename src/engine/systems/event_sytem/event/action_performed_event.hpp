#ifndef ACTIONPERFORMED_EVENT_HPP
#define ACTIONPERFORMED_EVENT_HPP

class ActionPerformedEvent : public IEvent
{
private:
    std::string action_name;
public:
    ActionPerformedEvent(const std::string& action_name) : action_name(action_name) {}
    ~ActionPerformedEvent() override = default;

    const std::string& get_action_name() const { return action_name; }

    DEFINE_EVENT_TYPE(ActionPerformedEvent, 
        static_cast<int>(EventCategory::Input)
    )
};

#endif //ACTIONPERFORMED_EVENT_HPP