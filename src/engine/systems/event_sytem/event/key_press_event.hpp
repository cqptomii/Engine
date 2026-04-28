#ifndef KEYPRESS_EVENT_HPP
#define KEYPRESS_EVENT_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class KeyPressEvent : public IEvent
{
private:
    int key_code;
    bool is_repeat;
public:
    KeyPressEvent(int key_code, bool is_repeat) : key_code(key_code), is_repeat(is_repeat) {}
    ~KeyPressEvent() override = default;

    int get_key_code() const { return key_code; }
    bool get_is_repeat() const { return is_repeat; }

    DEFINE_EVENT_TYPE(KeyPressEvent)
};

#endif //KEYPRESS_EVENT_HPP