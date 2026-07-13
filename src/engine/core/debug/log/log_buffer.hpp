#ifndef ENGINE_DEBUG_LOG_BUFFER_HPP
#define ENGINE_DEBUG_LOG_BUFFER_HPP

#include <cstddef>
#include <deque>

#include "engine/core/debug/log/log_types.hpp"

class LogBuffer {
    std::deque<LogEntry> entries;
    std::size_t max_entries = 2000;

public:
    void set_max_entries(const std::size_t capacity) {
        this->max_entries = capacity > 0 ? capacity : 1;
        while (this->entries.size() > this->max_entries) {
            this->entries.pop_front();
        }
    }

    void push(LogEntry entry) {
        this->entries.push_back(std::move(entry));
        while (this->entries.size() > this->max_entries) {
            this->entries.pop_front();
        }
    }

    [[nodiscard]] const std::deque<LogEntry>& get_entries() const noexcept {
        return this->entries;
    }

    void clear() {
        this->entries.clear();
    }
};

#endif
