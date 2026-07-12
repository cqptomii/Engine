#ifndef ENGINE_DEBUG_COUNTER_REGISTRY_HPP
#define ENGINE_DEBUG_COUNTER_REGISTRY_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "engine/core/debug/debug_id.hpp"

class CounterRegistry {
    struct CounterKey {
        DebugId id;
        std::string key;

        bool operator==(const CounterKey& other) const {
            return id == other.id && key == other.key;
        }
    };

    struct CounterKeyHash {
        std::size_t operator()(const CounterKey& counter_key) const noexcept {
            return std::hash<DebugId>{}(counter_key.id) ^ std::hash<std::string>{}(counter_key.key);
        }
    };

    std::unordered_map<CounterKey, uint32_t, CounterKeyHash> counters;
    std::unordered_map<CounterKey, uint32_t, CounterKeyHash> gauges;

public:
    void add(const DebugId id, const std::string& key, const uint32_t amount = 1) {
        const CounterKey counter_key{id, key};
        this->counters[counter_key] += amount;
    }

    uint32_t get(const DebugId id, const std::string& key) const {
        const CounterKey counter_key{id, key};
        const auto it = this->counters.find(counter_key);
        if (it == this->counters.end()) {
            return 0;
        }

        return it->second;
    }

    void set_gauge(const DebugId id, const std::string& key, const uint32_t value) {
        const CounterKey counter_key{id, key};
        this->gauges[counter_key] = value;
    }

    uint32_t get_gauge(const DebugId id, const std::string& key) const {
        const CounterKey counter_key{id, key};
        const auto it = this->gauges.find(counter_key);
        if (it == this->gauges.end()) {
            return 0;
        }

        return it->second;
    }

    void reset_frame() {
        this->counters.clear();
    }

    void print(const DebugId id = DebugId::system()) const {
        bool printed_header = false;
        for (const auto& [counter_key, value] : this->gauges) {
            if (counter_key.id != id) {
                continue;
            }

            if (!printed_header) {
                std::cout << "Counters:" << std::endl;
                printed_header = true;
            }

            std::cout << "  - " << counter_key.key << ": " << value << std::endl;
        }

        for (const auto& [counter_key, value] : this->counters) {
            if (counter_key.id != id) {
                continue;
            }

            if (!printed_header) {
                std::cout << "Counters:" << std::endl;
                printed_header = true;
            }

            std::cout << "  - " << counter_key.key << ": " << value << std::endl;
        }
    }
};

#endif
