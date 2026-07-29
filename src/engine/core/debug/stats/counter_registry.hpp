/**
 * @file counter_registry.hpp
 * @author FRAISSE Tom
 * @brief Counter registry class for the engine debug system
 * @details This class is responsible for the registration and management of the counters and gauges for the engine debug system
 * @version 0.1
 * @date 2026-07-29
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_COUNTER_REGISTRY_HPP
#define ENGINE_DEBUG_COUNTER_REGISTRY_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "engine/core/debug/debug_id.hpp"

class CounterRegistry {
    /**
     * @brief Structure to represent a counter key
     * 
     */
    struct CounterKey {
        DebugId id;
        std::string key;

        bool operator==(const CounterKey& other) const {
            return id == other.id && key == other.key;
        }
    };

    /**
     * @brief Structure to represent a counter key hash
     * 
     */
    struct CounterKeyHash {
        std::size_t operator()(const CounterKey& counter_key) const noexcept {
            return std::hash<DebugId>{}(counter_key.id) ^ std::hash<std::string>{}(counter_key.key);
        }
    };

    // The counters
    std::unordered_map<CounterKey, uint32_t, CounterKeyHash> counters;

    // The gauges
    std::unordered_map<CounterKey, uint32_t, CounterKeyHash> gauges;

public:

    /**
     * @brief Method to add a counter
     * 
     * @param id The debug ID
     * @param key The key of the counter
     * @param amount The amount to add to the counter
     * @return void
     */
    void add(const DebugId id, const std::string& key, const uint32_t amount = 1) {
        const CounterKey counter_key{id, key};
        this->counters[counter_key] += amount;
    }

    /**
     * @brief Method to get a counter
     * 
     * @param id The debug ID
     * @param key The key of the counter
     * @return uint32_t The value of the counter
     */
    uint32_t get(const DebugId id, const std::string& key) const {
        const CounterKey counter_key{id, key};
        const auto it = this->counters.find(counter_key);
        if (it == this->counters.end()) {
            return 0;
        }

        return it->second;
    }

    /**
     * @brief Method to set a gauge
     * 
     * @param id The debug ID
     * @param key The key of the gauge
     * @param value The value of the gauge
     * @return void
     */
    void set_gauge(const DebugId id, const std::string& key, const uint32_t value) {
        const CounterKey counter_key{id, key};
        this->gauges[counter_key] = value;
    }

    /**
     * @brief Method to get a gauge
     * 
     * @param id The debug ID
     * @param key The key of the gauge
     * @return uint32_t The value of the gauge
     */
    uint32_t get_gauge(const DebugId id, const std::string& key) const {
        const CounterKey counter_key{id, key};
        const auto it = this->gauges.find(counter_key);
        if (it == this->gauges.end()) {
            return 0;
        }

        return it->second;
    }

    /**
     * @brief Method to reset the counters and gauges of the registry
     * 
     * @return void
     */
    void reset_frame() {
        this->counters.clear();
    }

    /**
     * @brief Method to get the system gauges
     * 
     * @return std::vector<std::pair<std::string, uint32_t>> The system gauges
     */
    [[nodiscard]] std::vector<std::pair<std::string, uint32_t>> get_system_gauges() const {
        return this->collect_entries(this->gauges, DebugId::system());
    }

    /**
     * @brief Method to get the system counters
     * 
     * @return std::vector<std::pair<std::string, uint32_t>> The system counters
     */
    [[nodiscard]] std::vector<std::pair<std::string, uint32_t>> get_system_counters() const {
        return this->collect_entries(this->counters, DebugId::system());
    }

    /**
     * @brief Method to print the counters and gauges of the registry
     * 
     * @param id The debug ID
     * @return void
     */
    void print(const DebugId id = DebugId::system()) const {
        bool printed_header = false;
        
        // For each gauge in the registry print the gauge key and value
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

private:

    /**
     * @brief Method to collect the entries of the registry
     * 
     * @param source The source of the entries
     * @param id The debug ID
     * @return std::vector<std::pair<std::string, uint32_t>> The entries
     */
    [[nodiscard]] static std::vector<std::pair<std::string, uint32_t>> collect_entries(
        const std::unordered_map<CounterKey, uint32_t, CounterKeyHash>& source,
        const DebugId id)
    {
        // Create a vector to store the entries
        std::vector<std::pair<std::string, uint32_t>> entries;
        entries.reserve(source.size());

        for (const auto& [counter_key, value] : source) {
            if (counter_key.id != id) {
                continue;
            }

            entries.emplace_back(counter_key.key, value);
        }

        return entries;
    }
};

#endif
