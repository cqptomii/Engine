/**
 * @file event_listener.hpp 
 * @author FRAISSE Tom
 * @brief 
 * @version 0.1
 * @date 2026-06-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

#include "engine/core/event/event/ievent.hpp"

class EventListener
{
    public:
        virtual ~EventListener() = default;
        virtual void on_event(const IEvent& event) = 0;
};

#endif //EVENTLISTENER_HPP