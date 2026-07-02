/**
 * @file mouse_motion_event.hpp
 * @author Tom FRAISSE
 * @brief MouseMotionEvent that represent the state of the mouse cursor on the screen
 *  0,0  -> Top left corner of the screen
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef MOUSEMOTION_EVENT_HPP
#define MOUSEMOTION_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"


class MouseMotionEvent : public IEvent
{
private:

    // Position of the mouse cursor in the screen
    float x;
    float y;

public:

    /**
     * @brief Override Default constructor of the class
     * 
     * @param x : horizontal position of the cursor on the screen
     * @param y : vertical position of the cursor on the screen
     */
    MouseMotionEvent(float x, float y) : x(x), y(y) {}
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~MouseMotionEvent() override = default;

    /**
     * @brief Get the horizontal position of the cursor on the screen
     * 
     * @return float : horizontal position
     */
    float get_x() const { 
        return x; 
    }
    
    /**
     * @brief Get the vertical position of the cursor on the screen
     * 
     * @return float : vertical position
     */
    float get_y() const { 
        return y; 
    }

    /**
     * @brief Macro which defined the event Class / Type and categories
     * 
     */
    DEFINE_EVENT(
        MouseMotionEvent,
        MouseMoved, 
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSEMOTION_EVENT_HPP