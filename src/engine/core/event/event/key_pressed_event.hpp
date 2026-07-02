/**
 * @file key_pressed_event.hpp
 * @author Tom FRAISSE
 * @brief KeyPressedEvent class which represent that a Keyboard Key is pressed
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef KEYPRESS_EVENT_HPP
#define KEYPRESS_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"

class KeyPressedEvent : public IEvent
{
private:
    // KeyCode of the Key
    int key_code;

    // Handle if the key is pressed more than once
    bool is_repeat;
public:
    /**
     * @brief Override default Constructor for the class
     * 
     * @param key_code : Keycode of the key
     * @param is_repeat : 
     */
    KeyPressedEvent(int key_code, bool is_repeat) : key_code(key_code), is_repeat(is_repeat) {}
    
    /**
     * @brief Destroy the Key Pressed Event object
     * 
     */
    ~KeyPressedEvent() override = default;

    /**
     * @brief Get the KeyPressed Code 
     * 
     * @return int : Key Code of the event
     */
    int get_key_code() const { 
        return key_code; 
    }

    /**
     * @brief Get the repeated status of the keyPressedEvent
     * 
     * @return true  : The Key is pressed more than once
     * @return false : The key is pressed once
     */
    bool get_is_repeat() const { 
        return is_repeat; 
    }


    /**
     * @brief Macro which defined the Event Class / Type and Categories
     * 
     */
    DEFINE_EVENT(
        KeyPressedEvent,
        KeyPressed, 
        static_cast<int>(EventCategory::Keyboard) | static_cast<int>(EventCategory::Input)
    )
};

#endif //KEYPRESS_EVENT_HPP