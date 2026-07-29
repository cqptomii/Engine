#pragma once

#include <string>

#include "engine/core/debug/debug_id.hpp"
#include "engine/core/debug/instrumentation.hpp"

/**
 * @brief Function to register a name for a debug id macro
 * 
 * @param id The debug id
 * @param name The name to register
 */
inline void debug_register_name(const DebugId id, const std::string& name) {
#ifdef ENGINE_DEBUG
    Instrumentation::names().register_name(id, name);
#else
    (void)id;
    (void)name;
#endif
}
