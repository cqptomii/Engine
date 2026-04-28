#ifndef IEVENT_HPP
#define IEVENT_HPP

#include "engine/systems/event_system/event_category.hpp"

class IEvent
{
public:
    virtual ~IEvent() = default;

    virtual void Getype() const = 0;
    virtual void Clone() const = 0;

    // Return the categories that belong to the event
    virtual int get_category_flags() const = 0;
};

// Macro to help define event types
#define DEFINE_EVENT_TYPE(type, categories) \
    static const char* GetStaticType() { return #type; } \
    virtual const char* GetType() const override { return GetStaticType(); } \
    virtual Event* Clone() const override { return new type(*this); } \
    virtual int get_category_flags() const override { return categories; } \
#endif //IEVENT_HPP