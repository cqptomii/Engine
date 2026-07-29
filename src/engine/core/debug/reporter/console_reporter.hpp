/**
 * @file console_reporter.hpp
 * @author FRAISSE Tom
 * @brief Console reporter class for the engine debug system
 * @details This class is responsible for the reporting of the engine debug system to the console
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
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
    // The FPS timer
    float fps_timer = 0.0f;

    // The frame count
    uint32_t frame_count = 0;
    float displayed_fps = 0.0f;

    // The dump interval
    float dump_interval = 1.0f;

public:

    /**
     * @brief Method to handle the frame
     * 
     * @param delta_time The delta time
     * @param names The debug name registry
     * @param memory The memory tracker
     * @param frame_stats The frame stats
     * @param profiler The profiler
     * @param counters The counter registry
     * @param event_stats The event stats listener
     * @return void
     */
    void on_frame(const float delta_time, const DebugNameRegistry& names, const MemoryTracker& memory, const FrameStats& frame_stats, const Profiler& profiler, const CounterRegistry& counters, EventStatsListener& event_stats)
    {
        // Update the FPS timer
        this->fps_timer += delta_time;
        // Update the frame count
        this->frame_count++;

        // If the FPS timer is less than the dump interval, return
        if (this->fps_timer < this->dump_interval) {
            return;
        }

        this->displayed_fps = static_cast<float>(this->frame_count) / this->fps_timer;
        // Dump the data
        this->dump(names, memory, frame_stats, profiler, counters, event_stats);

        // Reset the FPS timer and the frame count
        this->fps_timer = 0.0f;
        this->frame_count = 0;
    }

    /**
     * @brief Method to dump the data to the console
     * 
     * @param names The debug name registry
     * @param memory The memory tracker
     * @param frame_stats The frame stats
     * @param profiler The profiler
     * @param counters The counter registry
     * @param event_stats The event stats listener
     * @return void
     */
    void dump(const DebugNameRegistry& names, const MemoryTracker& memory, const FrameStats& frame_stats, const Profiler& profiler, const CounterRegistry& counters, EventStatsListener& event_stats) const
    {
        // Print the FPS
        std::cout << "FPS: " << this->displayed_fps
                  << " | frame: " << frame_stats.get_frame_ms() << " ms" << std::endl;
        
        // Print the profiler
        profiler.print();

        // Print the counters
        counters.print(DebugId::system());
        
        // Print the event stats
        event_stats.print_and_reset();
        
        // Print the registered subjects
        std::cout << "Registered subjects: " << names.count() << std::endl;
        names.print_names();
        
        // Print the memory
        memory.print();
    }

    /**
     * @brief Method to get the FPS
     * 
     * @return float The FPS
     */
    float get_fps() const {
        return this->displayed_fps;
    }

    /**
     * @brief Set the dump interval object
     * 
     * @param seconds The dump interval in seconds
     * @return void
     */
    void set_dump_interval(const float seconds) {
        this->dump_interval = seconds;
    }
};

#endif
