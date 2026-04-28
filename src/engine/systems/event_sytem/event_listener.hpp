#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

class EventListener
{
    public:
        virtual ~EventListener() = default;
        virtual void on_event(const IEvent& event) = 0;
};

#endif //EVENTLISTENER_HPP