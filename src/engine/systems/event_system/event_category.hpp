#ifndef EVENTCATEGORY_HPP
#define EVENTCATEGORY_HPP

enum class EventCategory
{
    None = 0,
    Application = 1 << 0,
    Input = 1 << 1,
    Keyboard = 1 << 2,
    Mouse = 1 << 3,
    MouseButton = 1 << 4
    Window = 1 << 5
};


#endif //EVENTCATEGORY_HPP