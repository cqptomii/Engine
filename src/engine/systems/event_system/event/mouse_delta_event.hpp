#ifndef MOUSE_DELTA_EVENT_HPP
#define MOUSE_DELTA_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class MouseDeltaEvent : public IEvent
{
private:
    float x_offset;
    float y_offset;
public:
    MouseDeltaEvent(float x_offset, float y_offset) : x_offset(x_offset), y_offset(y_offset) {}

    float get_x_offset() const { 
        return x_offset; 
    }
    float get_y_offset() const { 
        return y_offset; 
    }

    DEFINE_EVENT_TYPE(MouseDelta,
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSE_DELTA_EVENT_HPP