#ifndef MOUSEBUTTONPRESSED_EVENT_HPP
#define MOUSEBUTTONPRESSED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class MouseButtonPressedEvent : public IEvent
{
private:
    int button;
public:

    MouseButtonPressedEvent(int button) : button(button) {}
    ~MouseButtonPressedEvent() override = default;

    int get_button() const { return button; }

    DEFINE_EVENT_TYPE(MouseButtonPressed, 
        static_cast<int>(EventCategory::MouseButton) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEBUTTONPRESSED_EVENT_HPP