#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP

#include "engine/systems/event_sytem/event/IEvent.hpp"
#include "engine/systems/event_sytem/event_listener.hpp"
#include "engine/systems/event_sytem/event_dispatcher.hpp"
#include <vector>
#include <mutex>
#include <queue>
#include <memory>

class EventBus
{
private:

    struct EventListenerInfo
    {
        EventListener* listener;
        int filter;
    };


    // List of event listeners
    std::vector<EventListenerInfo> listeners_info;
    std::queue<std::unique_ptr<IEvent> event_queue;
    std::mutex mutex;
    bool immediate = true;
public:
    EventBus() = default;
    ~EventBus() = default;

    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;


    bool set_immediate(bool value) {
        std::lock_guard<std::mutex> lock(mutex);
        immediate = value;
        return immediate;
    }

    void add_listener(EventListener* listener, int filter = -1){
        listeners_info.push_back(
            EventListenerInfo{listener, filter}
        );
    }
    void remove_listener(EventListener* listener){
        // Find the listener int the list if it exist
        auto it = std::find(listeners_info.begin(), listeners_info.end(), [listener](const EventListenerInfo& info) {
            return info.listener == listener;
        });
        if (it != listeners.end()) {
            listeners.erase(it);
        }
    }

    void publish_event(const IEvent& event){
        if (immediate) {
            for (auto& info : listeners_info) {
                if (info.filter == -1 || (event.get_category_flags() & info.filter)) {
                    info.listener->on_event(event);
                }
            }
        } else {
            std::lock_guard<std::mutex> lock(mutex);
            event_queue.push(std::make_unique<IEvent>(event));
        }
    }
    void process_events(){
        if (immediate) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex);
        while (!event_queue.empty()) {
            // Get the event at the front of the queue
            auto& event = event_queue.front();

            // Dispatch event to listeners
            for (auto& info : listeners_info) {
                if( info.filter == -1 || (event->get_category_flags() & info.filter)) {
                    info.listener->on_event(*event);
                }
            }
            event_queue.pop();
        }


    }
};


#endif //EVENTBUS_HPP