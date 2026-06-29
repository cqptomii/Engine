#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class EventDispatcher
{
private:
    // Event to dispatch to listners
    IEvent& event;

public:
    /**
     * @brief Recopied Constructor
     * 
     * @param event 
     */
    explicit EventDispatcher(IEvent& event) : event(event) {}

    // Dispatche event to handler if type match
    template<typename T, typename F>
    bool dispatch(const F& handler )
    {
        if (event.get_type() == T::GetStaticType())
        {
            handler(static_cast<const T&>(event));
            return true;
        }
        return false;
    }
};

#endif //EVENTDISPATCHER_HPP