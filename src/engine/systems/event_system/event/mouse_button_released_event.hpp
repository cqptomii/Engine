#ifndef MOUSEBUTTONRELEASED_EVENT_HPP
#define MOUSEBUTTONRELEASED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class MouseButtonReleasedEvent : public IEvent
{
private:
    int button;
public:

    MouseButtonReleasedEvent(int button) : button(button) {}
    ~MouseButtonReleasedEvent() override = default;

    int get_button() const { return button; }

    DEFINE_EVENT_TYPE(MouseButtonReleased, 
        static_cast<int>(EventCategory::MouseButton) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEBUTTONRELEASED_EVENT_HPP