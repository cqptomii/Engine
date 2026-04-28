#ifndef KEYRELEASE_EVENT_HPP
#define KEYRELEASE_EVENT_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class KeyReleaseEvent : public IEvent
{
private:
    int key_code;
    bool is_repeat;
public:

    KeyReleaseEvent(int key_code, bool is_repeat) : key_code(key_code), is_repeat(is_repeat) {}
    ~KeyReleaseEvent() override = default;

    int get_key_code() const { return key_code; }
    bool get_is_repeat() const { return is_repeat; }


    DEFINE_EVENT_TYPE(KeyReleaseEvent, 
        static_cast<int>(EventCategory::Keyboard) | static_cast<int>(EventCategory::Input)
    )
};


#endif //KEYRELEASE_EVENT_HPP