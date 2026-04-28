#ifndef MOUSECLICK_EVENT_HPP
#define MOUSECLICK_EVENT_HPP

#include "engine/systems/event_system/event/IEvent.hpp"

class MouseButtonEvent : public IEvent
{
private:
    int button;
public:

    MouseButtonEvent(int button) : button(button) {}
    ~MouseButtonEvent() override = default;

    int get_button() const { return button; }

    DEFINE_EVENT_TYPE(MouseButtonEvent, 
        static_cast<int>(EventCategory::MouseButton) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSECLICK_EVENT_HPP