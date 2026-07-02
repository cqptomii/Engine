/**
 * @file mouse_button_released_event.hpp
 * @author Tom FRAISSE
 * @brief MouseButtonReleasedEvent Class which represent the state when a mouse button is released
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MOUSEBUTTONRELEASED_EVENT_HPP
#define MOUSEBUTTONRELEASED_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"


class MouseButtonReleasedEvent : public IEvent
{
private:
    // Code of the button
    int button;
public:

    /**
     * @brief Override default constructor of the class
     * 
     * @param button : Code of the button released
     */
    MouseButtonReleasedEvent(int button) : button(button) {}

    /**
     * @brief Default Destructor of the class 
     * 
     */
    ~MouseButtonReleasedEvent() override = default;

    /**
     * @brief Get the button code of the event
     * 
     * @return int : Button code
     */
    int get_button() const { 
        return button; 
    }

    /**
     * @brief Macro which defined the event CLASS / Type and categories
     * 
     */
    DEFINE_EVENT(
        MouseButtonReleasedEvent,
        MouseButtonReleased, 
        static_cast<int>(EventCategory::MouseButton) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEBUTTONRELEASED_EVENT_HPP