#ifndef MOUSECLICK_EVENT_HPP
#define MOUSECLICK_EVENT_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class MouseClickEvent : public IEvent
{
private:
    int button;
    int x;
    int y;
public:

    MouseClickEvent(int button, int x, int y) : button(button), x(x), y(y) {}
    ~MouseClickEvent() override = default;

    int get_button() const { return button; }

    int get_x() const { return x; }
    int get_y() const { return y; }

    DEFINE_EVENT_TYPE(MouseClickEvent)
};

#endif //MOUSECLICK_EVENT_HPP