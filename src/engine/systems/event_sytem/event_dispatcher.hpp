#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"

class EventDispatcher
{
private:
    // Event to dispatch to listners
    IEvent& event;

public:
    explicit EventDispatcher(const IEvent& event) : event(event) {}

    // Dispatche event to handler if type match
    template<typename T, typename F>
    bool dispatch(const F& handler )
    {
        if (event.GetType() == T::GetStaticType())
        {
            handler(static_cast<const T&>(event));
            return true;
        }
        return false;
    }
};

#endif //EVENTDISPATCHER_HPP