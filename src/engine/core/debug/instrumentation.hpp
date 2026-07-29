/**
 * @file instrumentation.hpp
 * @author FRAISSE Tom
 * @brief Instrumentation class for the engine debug system 
 * @details This class is responsible for the instrumentation of the engine debug system
 * @version 0.1
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_INSTRUMENTATION_HPP
#define ENGINE_DEBUG_INSTRUMENTATION_HPP

#include "engine/core/debug/debug_hooks.hpp"
#include "engine/core/debug/log/logger.hpp"
#include "engine/core/debug/memory/memory_tracker.hpp"
#include "engine/core/debug/profile/profiler.hpp"
#include "engine/core/debug/reporter/console_reporter.hpp"
#include "engine/core/debug/stats/counter_registry.hpp"
#include "engine/core/debug/stats/event_stats_listener.hpp"
#include "engine/core/debug/stats/frame_stats.hpp"
#include "engine/core/debug/debug_name_registry.hpp"

#include "engine/core/event/event_bus.hpp"
#include "engine/resources/cpu/cpu_resource_manager.hpp"
#include "engine/resources/gpu/gpu_resource_manager.hpp"

class Instrumentation {

    inline static Logger logger_instance{};
    inline static DebugNameRegistry name_registry{};
    inline static CounterRegistry counter_registry{};
    inline static Profiler profiler_instance{};
    inline static FrameStats frame_stats_instance{};
    inline static MemoryTracker memory_tracker{};
    inline static ConsoleReporter console_reporter{};
    inline static EventStatsListener event_stats_listener{};
    inline static EventBus* attached_event_bus = nullptr;
    inline static bool initialized = false;

public:

    /**
     * @brief Method to initialize the instrumentation
     * 
     * @details This method is used to initialize the instrumentation
     * @details If the instrumentation is already initialized, it will return
     * @details If the instrumentation is not initialized, it will initialize the instrumentation
     */
    static void init() {
        if (Instrumentation::initialized) {
            return;
        }

        // Set the minimum log level to info
        Instrumentation::logger_instance.set_min_level(LogLevel::Info);

        // Register the system name
        Instrumentation::name_registry.register_name(DebugId::system(), "System");

        // Log the initialization
        Instrumentation::logger_instance.log(LogLevel::Info, "Engine", "Debug instrumentation initialized.");

        // Set the function to register the name of the resource
        debug_name_register_fn() = [](const uint32_t resource_id, const std::string& path) {
            Instrumentation::name_registry.register_name(
                DebugId{resource_id},
                resource_display_name(path));
        };

        // Set the function to add a counter
        debug_counter_add_fn() = [](const uint32_t debug_id, const char* key, const uint32_t amount) {
            Instrumentation::counter_registry.add(DebugId{debug_id}, key, amount);
        };

        // Set the function to set a gauge
        debug_counter_gauge_fn() = [](const uint32_t debug_id, const char* key, const uint32_t value) {
            Instrumentation::counter_registry.set_gauge(DebugId{debug_id}, key, value);
        };

        // Set the initialized flag to true
        Instrumentation::initialized = true;
    }

    /**
     * @brief Method to attach an event bus to the instrumentation
     * 
     * @param event_bus The event bus to attach
     * @return void
     */
    static void attach_event_bus(EventBus& event_bus) {
        // Check if the instrumentation is initialized
#ifdef ENGINE_DEBUG
        // If the event bus is already attached, remove the listener
        if (Instrumentation::attached_event_bus != nullptr) {
            Instrumentation::attached_event_bus->remove_listener(&Instrumentation::event_stats_listener);
        }

        // Add the listener to the event bus
        event_bus.add_listener(&Instrumentation::event_stats_listener);
        Instrumentation::attached_event_bus = &event_bus;
#else
        // If the instrumentation is not initialized, do nothing
        (void)event_bus;
#endif
    }

    /**
     * @brief Method to begin a frame 
     * 
     */
    static void begin_frame() {
#ifdef ENGINE_DEBUG
        Instrumentation::profiler_instance.begin_frame();
        Instrumentation::counter_registry.reset_frame();
        Instrumentation::event_stats_listener.reset_frame_counts();
#endif
    }

    /**
     * @brief Method to refresh the memory trackers
     * 
     * @param cpu_manager The CPU resource manager
     * @param gpu_manager The GPU resource manager
     * @return void
     */
    static void refresh_memory(const CpuResourceManager& cpu_manager, const GPUResourceManager& gpu_manager)
    {
#ifdef ENGINE_DEBUG
        Instrumentation::memory_tracker.refresh(cpu_manager, gpu_manager);
#endif
    }

    /**
     * @brief Method to end a frame
     * 
     * @param delta_time The delta time in seconds between the current and previous frame
     * @return void
     */
    static void end_frame(const float delta_time) {
        // Check if the instrumentation is initialized
#ifdef ENGINE_DEBUG
        // Update the frame stats
        Instrumentation::frame_stats_instance.on_frame(delta_time);
        // Update the console reporter
        Instrumentation::console_reporter.on_frame(
            delta_time,
            Instrumentation::name_registry,
            Instrumentation::memory_tracker,
            Instrumentation::frame_stats_instance,
            Instrumentation::profiler_instance,
            Instrumentation::counter_registry,
            Instrumentation::event_stats_listener
        );
#endif
    }

    /**
     * @brief Method to shutdown the instrumentation
     * 
     * @return void
     */
    static void shutdown() {
        // Check if the instrumentation is initialized
        if (Instrumentation::attached_event_bus != nullptr) {
            Instrumentation::attached_event_bus->remove_listener(&Instrumentation::event_stats_listener);
            Instrumentation::attached_event_bus = nullptr;
        }

        // Clear the name registry
        Instrumentation::name_registry.clear();
        Instrumentation::initialized = false;
    }

    /**
     * @brief Method to get the logger instance
     * 
     * @return Logger& The logger instance
     */
    static Logger& logger() {
        return Instrumentation::logger_instance;
    }

    /**
     * @brief Method to get the name registry instance
     * 
     * @return DebugNameRegistry& The name registry instance
     */
    static DebugNameRegistry& names() {
        return Instrumentation::name_registry;
    }

    /**
     * @brief Method to get the counter registry instance
     * 
     * @return CounterRegistry& The counter registry instance
     */
    static CounterRegistry& counters() {
        return Instrumentation::counter_registry;
    }

    /**
     * @brief Method to get the profiler instance
     * 
     * @return Profiler& The profiler instance
     */
    static Profiler& profiler() {
        return Instrumentation::profiler_instance;
    }

    /**
     * @brief Method to get the frame stats instance
     * 
     * @return FrameStats& The frame stats instance
     */
    static FrameStats& frame_stats() {
        return Instrumentation::frame_stats_instance;
    }

    /**
     * @brief Method to get the memory tracker instance
     * 
     * @return MemoryTracker& The memory tracker instance
     */
    static MemoryTracker& memory() {
        return Instrumentation::memory_tracker;
    }

    /**
     * @brief Method to get the console reporter instance
     * 
     * @return ConsoleReporter& The console reporter instance
     */
    static ConsoleReporter& console() {
        return Instrumentation::console_reporter;
    }

    /**
     * @brief Method to get the event stats listener instance
     * 
     * @return EventStatsListener& The event stats listener instance
     */
    static EventStatsListener& event_stats() {
        return Instrumentation::event_stats_listener;
    }
};

#endif
