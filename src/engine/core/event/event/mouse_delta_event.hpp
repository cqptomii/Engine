/**
 * @file mouse_delta_event.hpp
 * @author Tom FRAISSE
 * @brief MouseDeltaEvent class which represent the offset betwen two MouseMotionEvents 
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MOUSE_DELTA_EVENT_HPP
#define MOUSE_DELTA_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"

class MouseDeltaEvent : public IEvent
{
private:
    // Deplacement of the cursor on the screen between the two Motion events
    float x_offset;
    float y_offset;

public:

    /**
     * @brief Override default Constructor of the class
     * 
     * @param x_offset : Horizontal movement offset
     * @param y_offset : Vertical movement offset
     */
    MouseDeltaEvent(float x_offset, float y_offset) : x_offset(x_offset), y_offset(y_offset) {}
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~MouseDeltaEvent() override{}

    /**
     * @brief Get the x offset
     * 
     * @return float : horizontal offset
     */
    float get_x_offset() const { 
        return x_offset; 
    }

    /**
     * @brief Get the y offset
     * 
     * @return float : vertical offset
     */
    float get_y_offset() const { 
        return y_offset; 
    }

    /**
     * @brief Macro which defined the event Class / Type / categories
     * 
     */
    DEFINE_EVENT(
        MouseDeltaEvent,
        MouseDelta,
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSE_DELTA_EVENT_HPP