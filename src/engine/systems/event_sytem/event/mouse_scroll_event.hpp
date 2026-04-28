#ifndef MOUSESCROLL_EVENT_HPP
#define MOUSESCROLL_EVENT_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class MouseScrollEvent : public IEvent
{
private:
    double x_offset;
    double y_offset;
public:
    MouseScrollEvent(double x_offset, double y_offset) : x_offset(x_offset), y_offset(y_offset) {}
    ~MouseScrollEvent() override = default;

    double get_x_offset() const { return x_offset; }
    double get_y_offset() const { return y_offset; }

    DEFINE_EVENT_TYPE(MouseScrollEvent)
};

#endif //MOUSESCROLL_EVENT_HPP