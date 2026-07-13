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
    static void init() {
        if (Instrumentation::initialized) {
            return;
        }

        Instrumentation::logger_instance.set_min_level(LogLevel::Info);
        Instrumentation::name_registry.register_name(DebugId::system(), "System");
        Instrumentation::logger_instance.log(LogLevel::Info, "Engine", "Debug instrumentation initialized.");

        debug_name_register_fn() = [](const uint32_t resource_id, const std::string& path) {
            Instrumentation::name_registry.register_name(
                DebugId{resource_id},
                resource_display_name(path));
        };

        debug_counter_add_fn() = [](const uint32_t debug_id, const char* key, const uint32_t amount) {
            Instrumentation::counter_registry.add(DebugId{debug_id}, key, amount);
        };

        debug_counter_gauge_fn() = [](const uint32_t debug_id, const char* key, const uint32_t value) {
            Instrumentation::counter_registry.set_gauge(DebugId{debug_id}, key, value);
        };

        Instrumentation::initialized = true;
    }

    static void attach_event_bus(EventBus& event_bus) {
#ifdef ENGINE_DEBUG
        if (Instrumentation::attached_event_bus != nullptr) {
            Instrumentation::attached_event_bus->remove_listener(&Instrumentation::event_stats_listener);
        }

        event_bus.add_listener(&Instrumentation::event_stats_listener);
        Instrumentation::attached_event_bus = &event_bus;
#else
        (void)event_bus;
#endif
    }

    static void begin_frame() {
#ifdef ENGINE_DEBUG
        Instrumentation::profiler_instance.begin_frame();
        Instrumentation::counter_registry.reset_frame();
        Instrumentation::event_stats_listener.reset_frame_counts();
#endif
    }

    static void refresh_memory(
        const CpuResourceManager& cpu_manager,
        const GPUResourceManager& gpu_manager)
    {
#ifdef ENGINE_DEBUG
        Instrumentation::memory_tracker.refresh(cpu_manager, gpu_manager);
#endif
    }

    static void end_frame(const float delta_time) {
#ifdef ENGINE_DEBUG
        Instrumentation::frame_stats_instance.on_frame(delta_time);
        Instrumentation::console_reporter.on_frame(
            delta_time,
            Instrumentation::name_registry,
            Instrumentation::memory_tracker,
            Instrumentation::frame_stats_instance,
            Instrumentation::profiler_instance,
            Instrumentation::counter_registry,
            Instrumentation::event_stats_listener);
#endif
    }

    static void shutdown() {
        if (Instrumentation::attached_event_bus != nullptr) {
            Instrumentation::attached_event_bus->remove_listener(&Instrumentation::event_stats_listener);
            Instrumentation::attached_event_bus = nullptr;
        }

        Instrumentation::name_registry.clear();
        Instrumentation::initialized = false;
    }

    static Logger& logger() {
        return Instrumentation::logger_instance;
    }

    static DebugNameRegistry& names() {
        return Instrumentation::name_registry;
    }

    static CounterRegistry& counters() {
        return Instrumentation::counter_registry;
    }

    static Profiler& profiler() {
        return Instrumentation::profiler_instance;
    }

    static FrameStats& frame_stats() {
        return Instrumentation::frame_stats_instance;
    }

    static MemoryTracker& memory() {
        return Instrumentation::memory_tracker;
    }

    static ConsoleReporter& console() {
        return Instrumentation::console_reporter;
    }

    static EventStatsListener& event_stats() {
        return Instrumentation::event_stats_listener;
    }
};

#endif
