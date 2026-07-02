/**
 * @file key_released_event.hpp
 * @author Tom FRAISSE
 * @brief  KeyReleasedEvent class which show when a key is released
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef KEYRELEASE_EVENT_HPP
#define KEYRELEASE_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"

class KeyReleasedEvent : public IEvent
{
private:
    // KeyCode of the key released
    int key_code;
public:

    /**
     * @brief Construct a new Key Released Event object
     * 
     * @param key_code : KeyCode of the key released
     */
    KeyReleasedEvent(int key_code) : key_code(key_code) {}
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~KeyReleasedEvent() override = default;

    /**
     * @brief Get the key code of the event object
     *
     * @return int : KeyCode associated with the key
     */
    int get_key_code() const { 
        return key_code;
     }

    /**
     * @brief Macro which defined the event Class / Type and categories
     * 
     */
    DEFINE_EVENT(
        KeyReleasedEvent,
        KeyReleased, 
        static_cast<int>(EventCategory::Keyboard) | static_cast<int>(EventCategory::Input)
    )
};


#endif //KEYRELEASE_EVENT_HPP