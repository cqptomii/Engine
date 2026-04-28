#ifndef MOUSEMOTION_EVENT_HPP
#define MOUSEMOTION_EVENT_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class MouseMotionEvent : public IEvent
{
private:
    int x;
    int y;
public:
    MouseMotionEvent(int x, int y) : x(x), y(y) {}
    ~MouseMotionEvent() override = default;

    int get_x() const { return x; }
    int get_y() const { return y; }

    DEFINE_EVENT_TYPE(MouseMotionEvent, 
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEMOTION_EVENT_HPP