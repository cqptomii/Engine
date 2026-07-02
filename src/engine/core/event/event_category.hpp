/**
 * @file event_category.hpp
 * @author Tom FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-06-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef EVENTCATEGORY_HPP
#define EVENTCATEGORY_HPP

/**
 * @brief EventCategory is a enum that represents the category of an event
 * @details The category is used to filter events
 */
enum class EventCategory
{
    None = 0,

    // Application category
    Application = 1 << 0,

    // Input category
    Input = 1 << 1,
    Keyboard = 1 << 2,
    Mouse = 1 << 3,
    MouseButton = 1 << 4,

    // Window category
    Window = 1 << 5
};


#endif //EVENTCATEGORY_HPP