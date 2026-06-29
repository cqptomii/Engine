#ifndef WINDOWRESIZE_EVENT_HPP
#define WINDOWRESIZE_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class WindowResizeEvent : public IEvent
{
private:
    int width;
    int height;
public:
    WindowResizeEvent(int width, int height) : width(width), height(height) {}
    ~WindowResizeEvent() override = default;

    int get_width() const { return width; }
    int get_height() const { return height; }

    DEFINE_EVENT_TYPE(WindowResized, 
        static_cast<int>(EventCategory::Window) | static_cast<int>(EventCategory::Application)
    )
};

#endif //WINDOWRESIZE_EVENT_HPP