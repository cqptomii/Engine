
/**
 * @file mouse_button_pressed_event.hpp
 * @author Tom FRAISSE
 * @brief MouseButtonPressedEvent Class which represent the state where one button from the mouse is pressed
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef MOUSEBUTTONPRESSED_EVENT_HPP
#define MOUSEBUTTONPRESSED_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_category.hpp"

class MouseButtonPressedEvent : public IEvent
{
private:
    // Button Code
    int button;
public:

    /**
    * @brief Override default Constructor of the class
    * 
    * @param button : Code of the button pressed
    */
    MouseButtonPressedEvent(int button) : button(button) {}

    /**
     * @brief Default Destructor of the class
     * 
     */
    ~MouseButtonPressedEvent() override = default;

    /**
     * @brief Get the button code of the Event
     * 
     * @return int 
     */
    int get_button() const { 
        return button; 
    }

    /**
     * @brief  Macro which determine the event CLASS / Types and categories
     * 
     */
    DEFINE_EVENT(
        MouseButtonPressedEvent,
        MouseButtonPressed,
        static_cast<int>(EventCategory::MouseButton) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEBUTTONPRESSED_EVENT_HPP