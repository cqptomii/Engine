#ifndef ENGINE_DEBUG_EVENT_STATS_LISTENER_HPP
#define ENGINE_DEBUG_EVENT_STATS_LISTENER_HPP

#include <cstdint>
#include <iostream>
#include <unordered_map>

#include "engine/core/event/event_listener.hpp"
#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_types.hpp"

inline const char* event_type_to_string(const EventType type) {
    switch (type) {
        case EventType::KeyPressed: return "KeyPressed";
        case EventType::KeyReleased: return "KeyReleased";
        case EventType::KeyTyped: return "KeyTyped";
        case EventType::KeyRepeated: return "KeyRepeated";
        case EventType::MouseMoved: return "MouseMoved";
        case EventType::MouseScrolled: return "MouseScrolled";
        case EventType::MouseButtonPressed: return "MouseButtonPressed";
        case EventType::MouseButtonReleased: return "MouseButtonReleased";
        case EventType::MouseDelta: return "MouseDelta";
        case EventType::ActionStarted: return "ActionStarted";
        case EventType::ActionPerformed: return "ActionPerformed";
        case EventType::ActionEnded: return "ActionEnded";
        case EventType::WindowClosed: return "WindowClosed";
        case EventType::WindowResized: return "WindowResized";
        case EventType::WindowFocused: return "WindowFocused";
        case EventType::WindowBlurred: return "WindowBlurred";
        case EventType::WindowIconified: return "WindowIconified";
        case EventType::WindowDeiconified: return "WindowDeiconified";
        case EventType::WindowMoved: return "WindowMoved";
        default: return "Unknown";
    }
}

class EventStatsListener : public EventListener {
    std::unordered_map<EventType, uint32_t> event_counts;
    uint32_t total_events = 0;

public:
    void on_event(const IEvent& event) override {
        this->event_counts[event.get_type()]++;
        this->total_events++;
    }

    void print_and_reset() {
        std::cout << "Events/s: " << this->total_events << std::endl;
        if (this->event_counts.empty()) {
            return;
        }

        for (const auto& [type, count] : this->event_counts) {
            std::cout << "  - " << event_type_to_string(type) << ": " << count << std::endl;
        }

        this->event_counts.clear();
        this->total_events = 0;
    }

    uint32_t get_total_events() const {
        return this->total_events;
    }
};

#endif
