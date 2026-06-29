/**
 * @file ievent.hpp
 * @author Tom FRAISSE
 * @brief IEvent is an interface that represents an event
 * @version 0.1
 * @date 2026-06-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef IEVENT_HPP
#define IEVENT_HPP

#include "engine/systems/event_system/event_category.hpp"
#include "engine/systems/event_system/event_types.hpp"

class IEvent
{
public:
    /**
     * @brief Destructor
     * @details The destructor is used to destroy an event
     */
    virtual ~IEvent() = default;
    
    /**
     * @brief Get the type of the event
     * @details The type is used to identify the type of an event
     * @return The type of the event
     */
    
    virtual EventType get_type() const = 0;

    // Return the categories that belong to the event
    virtual int get_category_flags() const = 0;

};

// Macro to help define event types
#define DEFINE_EVENT_TYPE(event_type, categories) \
    EventType get_type() const override { return EventType::event_type; } \
    int get_category_flags() const override { return categories; }


#endif // IEVENT_HPP