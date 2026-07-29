/**
 * @file debug_name_registry.hpp
 * @author FRAISSE Tom
 * @brief Registry class for the engine debug system
 * @details This class is responsible for the registration of the names of the debug ids
 * @version 0.1
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_NAME_REGISTRY_HPP
#define ENGINE_DEBUG_NAME_REGISTRY_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/core/debug/debug_id.hpp"

class DebugNameRegistry {
    // The map of the debug ids and the names
    std::unordered_map<DebugId, std::string> name_map;

public:
    DebugNameRegistry() = default;
    ~DebugNameRegistry() = default;

    DebugNameRegistry(const DebugNameRegistry&) = delete;
    DebugNameRegistry& operator=(const DebugNameRegistry&) = delete;

    /**
     * @brief Register a name for a debug id
     * 
     * @param id The debug id
     * @param name The name to register
     * @return DebugId 
     * @details This function is used to register a name for a debug id
     * @details If the name is already registered, it will return the existing debug id
     * @details If the name is not registered, it will register the name and return the debug id
     * @details If the name is already registered with a different name, it will return the existing debug id
     * @details If the name is not registered, it will register the name and return the debug id
     */
    DebugId register_name(const DebugId id, std::string name) {
        const auto existing = this->name_map.find(id);
        if (existing == this->name_map.end()) {
            this->name_map.emplace(id, std::move(name));
            return id;
        }

        if (existing->second != name) {
#ifdef ENGINE_DEBUG
            std::cerr << "[Warning][DebugId] Hash collision between '"
                      << name << "' and '" << existing->second
                      << "' (id=" << id.value() << ")" << std::endl;
#endif
        }

        return id;
    }

    /**
     * @brief Get the name for a debug id
     * 
     * @param id The debug id
     * @return std::string 
     * @details This function is used to get the name for a debug id
     * @details If the debug id is not registered, it will return "unknown"
     */
    std::string get_name(const DebugId id) const {
        const auto it = this->name_map.find(id);
        if (it == this->name_map.end()) {
            return "unknown";
        }

        return it->second;
    }

    /**
     * @brief Check if a debug id is registered
     * 
     * @param id The debug id
     * @return true if the debug id is registered
     * @return false if the debug id is not registered
     */
    bool has_name(const DebugId id) const {
        return this->name_map.find(id) != this->name_map.end();
    }

    /**
     * @brief Clear the registry
     * 
     * @return void
     */
    void clear() {
        this->name_map.clear();
    }

    /**
     * @brief Print the names of the debug ids
     * 
     * @return void
     */
    void print_names() const {
        if (this->name_map.empty()) {
            std::cout << "  (none)" << std::endl;
            return;
        }

        for (const auto& [id, name] : this->name_map) {
            std::cout << "  - " << name << " (id=" << id.value() << ")" << std::endl;
        }
    }

    /**
     * @brief Get the number of registered debug ids
     * 
     * @return std::size_t 
     */
    std::size_t count() const {
        return this->name_map.size();
    }

    /**
     * @brief Get the entries of the registry
     * 
     * @return std::vector<std::pair<DebugId, std::string>> 
     * @details This function is used to get the entries of the registry
     * @details If the registry is empty, it will return an empty vector
     * @details If the registry is not empty, it will return a vector of the debug ids and the names
     */
    [[nodiscard]] std::vector<std::pair<DebugId, std::string>> get_entries() const {
        std::vector<std::pair<DebugId, std::string>> entries;
        entries.reserve(this->name_map.size());

        for (const auto& [id, name] : this->name_map) {
            entries.emplace_back(id, name);
        }

        return entries;
    }
};

#endif
