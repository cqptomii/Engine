#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include <unordered_map>
#include "engine/systems/event_system/event/key_press_event.hpp"
#include "engine/systems/event_system/event/key_release_event.hpp"
#include "engine/systems/event_system/event/mouse_button_press_event.hpp"
#include "engine/systems/event_system/event/mouse_button_release_event.hpp"
#include "engine/systems/event_system/event/mouse_motion_event.hpp"
#include "engine/systems/event_system/event/mouse_scroll_event.hpp"
#include "engine/systems/event_system/event_bus.hpp"


class InputSystem
{
private:
    EventBus event_bus = nullptr;

public:
    InputSystem(EventBus& bus) : event_bus(bus){};
    InputSystem() = default;
    ~InputSystem() = default;
    
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    // GLFW callbacks functions
    void on_key_event(int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS)
        {
            KeyPressEvent event(key, false);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_RELEASE)
        {
            KeyReleaseEvent event(key);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_REPEAT)
        {
            KeyPressEvent event(key, true);
            this->event_bus.publish_event(event);
        }
    }
    void on_mouse_button_event(int button, int action, int mods)
    {
        
        if (action == GLFW_PRESS)
        {
            MouseButtonPressEvent event(button);
            this->event_bus.publish_event(event);
        }
        else if (action == GLFW_RELEASE)
        {
            MouseButtonReleaseEvent event(button);
            this->event_bus.publish_event(event);
        }
    }
    void on_mouse_scroll(double xoffset, double yoffset)
    {
        MouseScrollEvent event(yoffset);
        this->event_bus.publish_event(event);
    }
    void on_mouse_move(const double x_pos, const double y_pos)
    {
        const auto current_x_pos = static_cast<float>(x_pos);
        const auto current_y_pos = static_cast<float>(y_pos);

        MouseMotionEvent event(current_x_pos, current_y_pos);
        this->event_bus.publish_event(event);
    }
};


#endif //INPUT_SYSTEM_HPP