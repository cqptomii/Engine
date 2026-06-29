#ifndef MOUSEMOTION_EVENT_HPP
#define MOUSEMOTION_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class MouseMotionEvent : public IEvent
{
private:
    float x;
    float y;
public:
    MouseMotionEvent(float x, float y) : x(x), y(y) {}
    ~MouseMotionEvent() override = default;

    float get_x() const { return x; }
    float get_y() const { return y; }

    DEFINE_EVENT_TYPE(MouseMoved, 
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEMOTION_EVENT_HPP