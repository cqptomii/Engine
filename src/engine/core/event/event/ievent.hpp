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

#include <memory>
#include "engine/core/event/event_types.hpp"

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

    /**
     * @brief Clone the event
     * @details The clone is used to create a new event of the same type and category
     * @return A new event of the same type and category
     */
    virtual std::unique_ptr<IEvent> clone() const = 0; 


};

// Macro to help define event types
#define DEFINE_EVENT(Class, event_type, categories) \
    static EventType get_static_type() { return EventType::event_type; } \
    EventType get_type() const override { return EventType::event_type; } \
    int get_category_flags() const override { return categories; } \
    std::unique_ptr<IEvent> clone() const override { \
        return std::make_unique<Class>(*this); \
    }


#endif // IEVENT_HPP