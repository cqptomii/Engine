/**
 * @file debug_name_component.hpp
 * @author Tom Fraisse
 * @brief Component to store a debug name for an entity
 * @version 0.1
 * @date 2026-07-12
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef ENGINE_DEBUG_NAME_COMPONENT_HPP
#define ENGINE_DEBUG_NAME_COMPONENT_HPP

#include <string>
#include "engine/core/debug/debug_id.hpp"

class  DebugNameComponent {
    DebugId id{DebugId::none()};
    std::string name{"unknown"};

    public:
    /**
     * @brief Constructor of the debug name component
     * 
     * @param name The name to set for the debug name component
     */
    explicit DebugNameComponent(std::string name) : name(name) {
        id = DebugId::from_name(name);
    }

    /**
     * @brief Get the id of the debug name component
     * 
     * @return DebugId The id of the debug name component
     */
    DebugId get_id() const {
        return id;
    }

    /**
     * @brief Get the name of the debug name component
     * 
     * @return std::string The name of the debug name component
     */
    std::string get_name() const {
        return name;
    }
};

#endif // ENGINE_DEBUG_NAME_COMPONENT_HPP