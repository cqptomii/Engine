/**
 * @file event_types.hpp
 * @author Tom FRAISSE
 * @brief EventType is a enum that represents the types of events
 * @version 0.1
 * @date 2026-06-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef EVENTTYPES_HPP
#define EVENTTYPES_HPP

enum class EventType
{
    None = 0,
    
    // Keyboard events
    KeyPressed = 1,
    KeyReleased = 2,
    KeyTyped = 3,
    KeyRepeated = 4,
    
    // Mouse events
    MouseMoved = 5,
    MouseScrolled = 6,
    MouseButtonPressed = 7,
    MouseButtonReleased = 8,
    MouseDelta = 9,

    // Action events
    ActionStarted = 10,
    ActionPerformed = 11,
    ActionEnded = 12,

    // Window events
    WindowClosed = 13,
    WindowResized = 14,
    WindowFocused = 15,
    WindowBlurred = 16,
    WindowIconified = 17,
    WindowDeiconified = 18,
    WindowMoved = 19,
};


#endif // EVENTTYPES_HPP