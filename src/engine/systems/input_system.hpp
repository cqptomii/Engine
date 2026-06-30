/**
 * @file input_system.hpp
 * @author TOM FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-06-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <memory>
#include <utility>
#include "engine/systems/event_system/event/key_pressed_event.hpp"
#include "engine/systems/event_system/event/key_released_event.hpp"
#include "engine/systems/event_system/event/mouse_button_released_event.hpp"
#include "engine/systems/event_system/event/mouse_button_pressed_event.hpp"
#include "engine/systems/event_system/event/mouse_motion_event.hpp"
#include "engine/systems/event_system/event/mouse_scroll_event.hpp"
#include "engine/systems/event_system/event/window_resize_event.hpp"
#include "engine/systems/event_system/event_bus.hpp"


class InputSystem
{
private:
    EventBus& event_bus;

public:

    /**
     * @brief Delete the default construct
     */
    InputSystem() = delete;

    /**
     * @brief Default Constructor with the EventBus object in parameter
     * 
     * @param bus : Main event Bus
     */
    InputSystem(EventBus& bus) : event_bus(bus){};

    /**
     * @brief Default Destructor
     * 
     */
    ~InputSystem() = default;
    
    /**
     * @brief Delete the recopied Constructor
     * 
     */
    InputSystem(const InputSystem&) = delete;
    
    /**
     * @brief Delete the equal operator for this class
     * 
     * @return InputSystem& 
     */
    InputSystem& operator=(const InputSystem&) = delete;

    /**
     * @brief  Method use to handle the key event
     * 
     * @param key (int) : Key code
     * @param scancode (int) : Scancode
     * @param action (int) : Action code
     * @param mods (int) : Modifier code
     */
    void on_key_event(int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            KeyPressedEvent event(key, false);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleasedEvent event(key);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_REPEAT)
        {
            KeyPressedEvent event(key, true);
            this->event_bus.publish_event(event);
        }
    }

    /**
     * @brief Method use to handle the mouse button event
     * 
     * @param button (int) : Button number
     * @param action (int) : Action code
     * @param mods (int) : Modifier code
     */
    void on_mouse_button_event(int button, int action, int mods)
    {
        
        if (action == GLFW_PRESS)
        {
            MouseButtonPressedEvent event(button);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_RELEASE)
        {
            MouseButtonReleasedEvent event(button);
            this->event_bus.publish_event(event);
        }
    }

    /**
     * @brief Method use to handle the mouse scroll
     * 
     * @param xoffset (double) : Horizontal offset of the mouse scroll  
     * @param yoffset (double) : Vertical offset of the mouse scroll
     */
    void on_mouse_scroll(double xoffset, double yoffset)
    {
        MouseScrollEvent event(yoffset);
        this->event_bus.publish_event(event);
    }

    /**
     * @brief Method use to handle the mouse movement
     * 
     * @param x_pos (double) : Horizontal position of the mouse cursor
     * @param y_pos (double) : Vertical position of the mouse cursor
     */
    void on_mouse_move(const double x_pos, const double y_pos)
    {
        const auto current_x_pos = static_cast<float>(x_pos);
        const auto current_y_pos = static_cast<float>(y_pos);

        MouseMotionEvent event(current_x_pos, current_y_pos);
        this->event_bus.publish_event(event);
    }

    /**
     * @brief  Method use to resize the screen
     * 
     * @param width (int) : Horizontal size of the screen
     * @param height (int) : Vertical size of the screen
     */
    void on_window_resize(const int width, const int height)
    {
        glViewport(0, 0, width, height);
        WindowResizeEvent event(width, height);
        this->event_bus.publish_event(event);
    }
};


#endif //INPUT_SYSTEM_HPP