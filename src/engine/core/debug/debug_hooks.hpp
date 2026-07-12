#pragma once

#include <cstdint>
#include <string>

#include "engine/core/utils.hpp"

inline std::string resource_display_name(const std::string& path) {
    if (path.empty()) {
        return "unknown";
    }

    const std::string normalized = normalize_resource_path_minimal(path);
    const std::size_t slash_pos = normalized.find_last_of('/');
    if (slash_pos == std::string::npos) {
        return normalized;
    }

    return normalized.substr(slash_pos + 1);
}

using DebugNameRegisterFn = void(*)(uint32_t resource_id, const std::string& path);
using DebugCounterAddFn = void(*)(uint32_t debug_id, const char* key, uint32_t amount);
using DebugCounterGaugeFn = void(*)(uint32_t debug_id, const char* key, uint32_t value);

inline DebugCounterGaugeFn& debug_counter_gauge_fn() {
    static DebugCounterGaugeFn fn = nullptr;
    return fn;
}

inline void debug_set_gauge(const char* key, const uint32_t value) {
    if (debug_counter_gauge_fn() != nullptr) {
        debug_counter_gauge_fn()(1, key, value);
    }
}

inline DebugNameRegisterFn& debug_name_register_fn() {
    static DebugNameRegisterFn fn = nullptr;
    return fn;
}

inline DebugCounterAddFn& debug_counter_add_fn() {
    static DebugCounterAddFn fn = nullptr;
    return fn;
}

inline void debug_notify_resource_loaded(const uint32_t resource_id, const std::string& path) {
    if (debug_name_register_fn() != nullptr) {
        debug_name_register_fn()(resource_id, path);
    }
}

inline void debug_record_draw_call(const uint32_t index_count = 0) {
    if (debug_counter_add_fn() == nullptr) {
        return;
    }

    constexpr uint32_t system_id = 1;
    debug_counter_add_fn()(system_id, "draw_calls", 1);

    if (index_count > 0) {
        debug_counter_add_fn()(system_id, "triangles", index_count / 3);
    }
}

inline void debug_record_raw_draw_call() {
    debug_record_draw_call(0);
}

inline void debug_record_counter(const char* key, const uint32_t amount = 1) {
    if (debug_counter_add_fn() != nullptr) {
        debug_counter_add_fn()(1, key, amount);
    }
}
