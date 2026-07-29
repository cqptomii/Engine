/**
 * @file event_stats_listener.hpp
 * @author FRAISSE Tom
 * @brief Event stats listener class for the engine debug system
 * @details This class is responsible for the tracking of the event stats for the engine debug system
 * @brief 
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_EVENT_STATS_LISTENER_HPP
#define ENGINE_DEBUG_EVENT_STATS_LISTENER_HPP

#include <cstdint>
#include <iostream>
#include <unordered_map>

#include "engine/core/event/event_listener.hpp"
#include "engine/core/event/event/ievent.hpp"
#include "engine/core/event/event_types.hpp"

/**
 * @brief Method to convert an event type to a string
 * 
 * @param type The event type
 * @return const char* The string representation of the event type
 */
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

/**
 * @brief Event stats listener class for the engine debug system
 * 
 * @details This class is responsible for the tracking of the event stats for the engine debug system
 */
class EventStatsListener : public EventListener {
    // The interval event counts
    std::unordered_map<EventType, uint32_t> interval_event_counts;
    // The frame event counts
    std::unordered_map<EventType, uint32_t> frame_event_counts;
    // The interval total events
    uint32_t interval_total_events = 0;
    // The frame total events
    uint32_t frame_total_events = 0;

public:

    /**
     * @brief Method to handle the event
     * 
     * @param event The event
     * @return void
     */
    void on_event(const IEvent& event) override {
        // Get the event type
        const EventType type = event.get_type();
        
        // Increment the event count for the interval and frame
        this->interval_event_counts[type]++;
        this->frame_event_counts[type]++;
        // Increment the total event count for the interval and frame
        this->interval_total_events++;
        this->frame_total_events++;
    }
    
    /**
     * @brief Method to reset the frame counts
     * 
     * @return void
     */
    void reset_frame_counts() {
        this->frame_event_counts.clear();
        this->frame_total_events = 0;
    }

    /**
     * @brief Method to print the event stats and reset the counts
     * 
     * @return void
     */
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

    /**
     * @brief Method to get the frame total events
     * 
     * @return uint32_t The frame total events
     */
    [[nodiscard]] uint32_t get_frame_total_events() const {
        return this->frame_total_events;
    }

    /**
     * @brief Method to get the total events
     * 
     * @return uint32_t The total events
     */
    [[nodiscard]] uint32_t get_total_events() const {
        return this->interval_total_events;
    }
};

#endif
