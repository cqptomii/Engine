/**
 * @file event_dispatcher.hpp
 * @author Tom FRAISSE
 * @brief 
 * @version 0.1
 * @date 2026-06-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef EVENTDISPATCHER_HPP
#define EVENTDISPATCHER_HPP

#include "engine/systems/event_system/event/ievent.hpp"

class EventDispatcher
{
private:
    const IEvent& event;

public:
    /**
     * @brief Recopied Constructor
     * 
     * @param event The event to dispatch
     */
    explicit EventDispatcher(const IEvent& event) : event(event) {
    }

    /**
     * @brief Dispatch the event to the handler if the type matches
     * @details The dispatch is used to dispatch the event to the handler if the type matches
     *
     * @tparam T The type of the event to dispatch
     * @tparam F The type of the function to handle the event
     * @param handler The function to handle the event
     * @return True if the event was handled, false otherwise
     */
    template<typename T, typename F>
    bool dispatch(const F& handler) const noexcept
    {
        if (event.get_type() == T::get_static_type())
        {
            handler(static_cast<const T&>(event)); // Call the handler with the event
            return true;
        }
        return false; // Return false if the event was not handled
    }
};

#endif //EVENTDISPATCHER_HPP