#ifndef MOUSESCROLL_EVENT_HPP
#define MOUSESCROLL_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class MouseScrollEvent : public IEvent
{
private:
    double y_offset;
public:
    MouseScrollEvent(double y_offset) : y_offset(y_offset) {}
    ~MouseScrollEvent() override = default;

    double get_y_offset() const { return y_offset; }

    DEFINE_EVENT_TYPE(MouseScrolled, 
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSESCROLL_EVENT_HPP