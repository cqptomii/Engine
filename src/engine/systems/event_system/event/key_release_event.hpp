#ifndef KEYRELEASE_EVENT_HPP
#define KEYRELEASE_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class KeyReleaseEvent : public IEvent
{
private:
    int key_code;
public:

    KeyReleaseEvent(int key_code) : key_code(key_code) {}
    ~KeyReleaseEvent() override = default;

    int get_key_code() const { return key_code; }

    DEFINE_EVENT_TYPE(KeyReleased, 
        static_cast<int>(EventCategory::Keyboard) | static_cast<int>(EventCategory::Input)
    )
};


#endif //KEYRELEASE_EVENT_HPP