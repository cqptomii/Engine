#ifndef ENGINE_DEBUG_CONSOLE_REPORTER_HPP
#define ENGINE_DEBUG_CONSOLE_REPORTER_HPP

#include <cstdint>
#include <iostream>

#include "engine/core/debug/debug_name_registry.hpp"
#include "engine/core/debug/memory/memory_tracker.hpp"
#include "engine/core/debug/profile/profiler.hpp"
#include "engine/core/debug/stats/counter_registry.hpp"
#include "engine/core/debug/stats/event_stats_listener.hpp"
#include "engine/core/debug/stats/frame_stats.hpp"

class ConsoleReporter {
    float fps_timer = 0.0f;
    uint32_t frame_count = 0;
    float displayed_fps = 0.0f;
    float dump_interval = 1.0f;

public:
    void on_frame(
        const float delta_time,
        const DebugNameRegistry& names,
        const MemoryTracker& memory,
        const FrameStats& frame_stats,
        const Profiler& profiler,
        const CounterRegistry& counters,
        EventStatsListener& event_stats)
    {
        this->fps_timer += delta_time;
        this->frame_count++;

        if (this->fps_timer < this->dump_interval) {
            return;
        }

        this->displayed_fps = static_cast<float>(this->frame_count) / this->fps_timer;
        this->dump(names, memory, frame_stats, profiler, counters, event_stats);

        this->fps_timer = 0.0f;
        this->frame_count = 0;
    }

    void dump(
        const DebugNameRegistry& names,
        const MemoryTracker& memory,
        const FrameStats& frame_stats,
        const Profiler& profiler,
        const CounterRegistry& counters,
        EventStatsListener& event_stats) const
    {
        std::cout << "FPS: " << this->displayed_fps
                  << " | frame: " << frame_stats.get_frame_ms() << " ms" << std::endl;
        profiler.print();
        counters.print(DebugId::system());
        event_stats.print_and_reset();
        std::cout << "Registered subjects: " << names.count() << std::endl;
        names.print_names();
        memory.print();
    }

    float get_fps() const {
        return this->displayed_fps;
    }

    void set_dump_interval(const float seconds) {
        this->dump_interval = seconds;
    }
};

#endif
