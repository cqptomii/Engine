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
    std::unordered_map<EventType, uint32_t> interval_event_counts;
    std::unordered_map<EventType, uint32_t> frame_event_counts;
    uint32_t interval_total_events = 0;
    uint32_t frame_total_events = 0;

public:
    void on_event(const IEvent& event) override {
        const EventType type = event.get_type();
        this->interval_event_counts[type]++;
        this->frame_event_counts[type]++;
        this->interval_total_events++;
        this->frame_total_events++;
    }

    void reset_frame_counts() {
        this->frame_event_counts.clear();
        this->frame_total_events = 0;
    }

    void print_and_reset() {
        std::cout << "Events/s: " << this->interval_total_events << std::endl;
        if (this->interval_event_counts.empty()) {
            this->interval_total_events = 0;
            return;
        }

        for (const auto& [type, count] : this->interval_event_counts) {
            std::cout << "  - " << event_type_to_string(type) << ": " << count << std::endl;
        }

        this->interval_event_counts.clear();
        this->interval_total_events = 0;
    }

    [[nodiscard]] uint32_t get_frame_total_events() const {
        return this->frame_total_events;
    }

    [[nodiscard]] uint32_t get_total_events() const {
        return this->interval_total_events;
    }
};

#endif
