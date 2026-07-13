#ifndef ENGINE_DEBUG_NAME_REGISTRY_HPP
#define ENGINE_DEBUG_NAME_REGISTRY_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/core/debug/debug_id.hpp"

class DebugNameRegistry {
    std::unordered_map<DebugId, std::string> name_map;

public:
    DebugNameRegistry() = default;
    ~DebugNameRegistry() = default;

    DebugNameRegistry(const DebugNameRegistry&) = delete;
    DebugNameRegistry& operator=(const DebugNameRegistry&) = delete;

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

    std::string get_name(const DebugId id) const {
        const auto it = this->name_map.find(id);
        if (it == this->name_map.end()) {
            return "unknown";
        }

        return it->second;
    }

    bool has_name(const DebugId id) const {
        return this->name_map.find(id) != this->name_map.end();
    }

    void clear() {
        this->name_map.clear();
    }

    void print_names() const {
        if (this->name_map.empty()) {
            std::cout << "  (none)" << std::endl;
            return;
        }

        for (const auto& [id, name] : this->name_map) {
            std::cout << "  - " << name << " (id=" << id.value() << ")" << std::endl;
        }
    }

    std::size_t count() const {
        return this->name_map.size();
    }

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
