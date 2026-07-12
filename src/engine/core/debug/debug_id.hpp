#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

#include "engine/core/utils.hpp"

struct DebugId {
    uint32_t id = 0;

    /**
     * @brief None debug id
     * 
     * @return constexpr DebugId 
     */
    static constexpr DebugId none() { 
        return DebugId{0}; 
    }

    /**
     * @brief System debug id
     * 
     * @return constexpr DebugId 
     */
    static constexpr DebugId system() {
        return DebugId{1};
    }

    /**
     * @brief Create a debug id from a name
     * 
     * @param name 
     * @return DebugId 
     */
    static DebugId from_name(std::string_view name) {
        return DebugId{ hash_string(std::string(name)) };
    }

    uint32_t value() const {
        return id;
    }

    /**
     * @brief Check if the debug id is equal to another debug id
     * 
     * @param o 
     * @return true if the debug id is equal to another debug id
     * @return false if the debug id is not equal to another debug id
     */
    bool operator==(const DebugId& o) const { 
        return id == o.id;
    }

    /**
     * @brief Check if the debug id is none
     * 
     * @return true if the debug id is none
     * @return false if the debug id is not none
     */
    bool valid() const {
        return id != none().id; 
    }
};

template<>
struct std::hash<DebugId> {
    std::size_t operator()(const DebugId& debug_id) const noexcept {
        return debug_id.value();
    }
};
