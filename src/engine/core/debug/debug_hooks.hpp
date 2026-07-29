#pragma once

#include <cstdint>
#include <string>

#include "engine/core/utils.hpp"

/**
 * @brief Method to get the display name of a resource
 * 
 * @param path The path of the resource
 * @return std::string The display name of the resource
 */
inline std::string resource_display_name(const std::string& path) {
    // If the path is empty, return "unknown"
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

// The debug name register function type
using DebugNameRegisterFn = void(*)(uint32_t resource_id, const std::string& path);
// The debug counter add function type
using DebugCounterAddFn = void(*)(uint32_t debug_id, const char* key, uint32_t amount);
// The debug counter gauge function type
using DebugCounterGaugeFn = void(*)(uint32_t debug_id, const char* key, uint32_t value);

/**
 * @brief Method to get the debug counter gauge function
 * 
 * @return DebugCounterGaugeFn& The debug counter gauge function
 */
inline DebugCounterGaugeFn& debug_counter_gauge_fn() {
    static DebugCounterGaugeFn fn = nullptr;
    return fn;
}

/**
 * @brief Method to set a gauge
 * 
 * @param key The key of the gauge
 * @param value The value of the gauge
 * @return void
 */
inline void debug_set_gauge(const char* key, const uint32_t value) {
    if (debug_counter_gauge_fn() != nullptr) {
        debug_counter_gauge_fn()(1, key, value);
    }
}

/**
 * @brief Method to get the debug name register function
 * 
 * @return DebugNameRegisterFn& The debug name register function
 */
inline DebugNameRegisterFn& debug_name_register_fn() {
    static DebugNameRegisterFn fn = nullptr;
    return fn;
}

/**
 * @brief Method to get the debug counter add function
 * 
 * @return DebugCounterAddFn& The debug counter add function
 */
inline DebugCounterAddFn& debug_counter_add_fn() {
    static DebugCounterAddFn fn = nullptr;
    return fn;
}

/**
 * @brief Method to notify that a resource has been loaded
 * 
 * @param resource_id The ID of the resource
 * @param path The path of the resource
 * @return void
 */
inline void debug_notify_resource_loaded(const uint32_t resource_id, const std::string& path) {
    if (debug_name_register_fn() != nullptr) {
        debug_name_register_fn()(resource_id, path);
    }
}

/**
 * @brief Method to record a draw call
 * 
 * @param index_count The number of indices
 * @return void
 */
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

/**
 * @brief Method to record a raw draw call
 * 
 * @return void
 */
inline void debug_record_raw_draw_call() {
    debug_record_draw_call(0);
}

/**
 * @brief Method to record a counter
 * 
 * @param key The key of the counter
 * @param amount The amount of the counter
 * @return void
 */
inline void debug_record_counter(const char* key, const uint32_t amount = 1) {
    if (debug_counter_add_fn() != nullptr) {
        debug_counter_add_fn()(1, key, amount);
    }
}
