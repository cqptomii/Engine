/**
 * @file window_resize_event.hpp
 * @author Tom FRAISSE
 * @brief WindowResizeEvent Class which represent the window resize event
 * @version 0.1
 * @date 2026-06-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef WINDOWRESIZE_EVENT_HPP
#define WINDOWRESIZE_EVENT_HPP

#include "engine/systems/event_system/event/ievent.hpp"
#include "engine/systems/event_system/event_category.hpp"

class WindowResizeEvent : public IEvent
{
private:

    // Resized size of the window
    int width;
    int height;
public:

    /**
    * @brief Override Constructor of the class
    * 
    * @param width : Width of the resized window
    * @param height : Height of the resized window
    */
    WindowResizeEvent(int width, int height) : width(width), height(height) {}
    
    /**
     * @brief Default Destructor of the class
     * 
     */
    ~WindowResizeEvent() override = default;

    /**
     * @brief Get the width of the window resized
     * 
     * @return int : width
     */
    int get_width() const { 
        return width;
    }
    
    /**
     * @brief Get the height of the window resized
     * 
     * @return int : height
     */
    int get_height() const { 
        return height; 
    }

    DEFINE_EVENT(
        WindowResizeEvent,
        WindowResized,
        static_cast<int>(EventCategory::Window) | static_cast<int>(EventCategory::Application)
    )
};

#endif //WINDOWRESIZE_EVENT_HPP