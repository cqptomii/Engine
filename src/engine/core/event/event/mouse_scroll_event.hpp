/**
 * @file mouse_scroll_event.hpp
 * @author Tom FRAISSE
 * @brief MouseScollEvent Class that represents a mouse scroll event
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef MOUSESCROLL_EVENT_HPP
#define MOUSESCROLL_EVENT_HPP

#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_category.hpp"

class MouseScrollEvent : public IEvent
{
private:
    // Scroll offset on the Y axis
    double y_offset;
public:
    /**
     * @brief Constructor of the MouseScrollEvent class
     * 
     * @param y_offset Scroll offset on the Y axis
     */
    MouseScrollEvent(double y_offset) : y_offset(y_offset) {}
    
    /**
     * @brief Destructor of the MouseScrollEvent class
     */
    ~MouseScrollEvent() override = default;

    /**
     * @brief Get the scroll offset on the Y axis
     * 
     * @return double Scroll offset on the Y axis
     */
    double get_y_offset() const { 
        return y_offset; 
    }

    /**
     * @brief Define the MouseScrollEvent event
     * 
     * @return IEvent* Pointer to the MouseScrollEvent event
     */
    DEFINE_EVENT(
        MouseScrollEvent,
        MouseScrolled, 
        static_cast<int>(EventCategory::Mouse) | static_cast<int>(EventCategory::Input)
    )
};

#endif //MOUSESCROLL_EVENT_HPP