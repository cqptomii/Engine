#ifndef IEVENT_HPP
#define IEVENT_HPP

class IEvent
{
public:
    virtual ~IEvent() = default;

    virtual void Getype() const = 0;
    virtual void Clone() const = 0;
};

// Macro to help define event types
#define DEFINE_EVENT_TYPE(type) \
    static const char* GetStaticType() { return #type; } \
    virtual const char* GetType() const override { return GetStaticType(); } \
    virtual Event* Clone() const override { return new type(*this); }
#endif //IEVENT_HPP