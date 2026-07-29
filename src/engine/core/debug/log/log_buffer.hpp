/**
 * @file log_buffer.hpp
 * @author FRAISSE Tom
 * @brief Buffer class for the engine debug system
 * @version 0.1
 * @date 2026-07-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef ENGINE_DEBUG_LOG_BUFFER_HPP
#define ENGINE_DEBUG_LOG_BUFFER_HPP

#include <cstddef>
#include <deque>

#include "engine/core/debug/log/log_types.hpp"

class LogBuffer {

    // The entries of the buffer
    std::deque<LogEntry> entries;
    
    // The maximum number of entries in the buffer
    std::size_t max_entries = 2000;

public:
    /**
     * @brief Set the maximum number of entries in the buffer
     * 
     * @param capacity The maximum number of entries
     * @return void
     */
    void set_max_entries(const std::size_t capacity) {
        this->max_entries = capacity > 0 ? capacity : 1;
        while (this->entries.size() > this->max_entries) {
            this->entries.pop_front();
        }
    }

    /**
     * @brief Push a log entry to the buffer
     * 
     * @param entry The log entry to push
     * @return void
     */
    void push(LogEntry entry) {
        this->entries.push_back(std::move(entry));
        while (this->entries.size() > this->max_entries) {
            this->entries.pop_front();
        }
    }

    /**
     * @brief Get the entries of the buffer
     * 
     * @return const std::deque<LogEntry>& 
     */
    [[nodiscard]] const std::deque<LogEntry>& get_entries() const noexcept {
        return this->entries;
    }

    /**
     * @brief Clear the buffer
     * 
     * @return void
     */
    void clear() {
        this->entries.clear();
    }
};

#endif
