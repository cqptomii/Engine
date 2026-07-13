#ifndef ENGINE_DEBUG_LOGGER_HPP
#define ENGINE_DEBUG_LOGGER_HPP

#include <iostream>
#include <string>
#include <string_view>

#include "engine/core/debug/log/log_buffer.hpp"
#include "engine/core/debug/log/log_types.hpp"

class Logger {
    LogLevel min_level = LogLevel::Info;
    LogBuffer buffer;

    static const char* level_to_string(const LogLevel level) {
        switch (level) {
            case LogLevel::Trace:
                return "Trace";
            case LogLevel::Info:
                return "Info";
            case LogLevel::Warning:
                return "Warning";
            case LogLevel::Error:
                return "Error";
            default:
                return "Unknown";
        }
    }

public:
    void set_min_level(const LogLevel level) {
        this->min_level = level;
    }

    void log(const LogLevel level, const std::string_view category, const std::string_view message) {
        if (level < this->min_level) {
            return;
        }

        this->buffer.push(LogEntry{
            level,
            std::string(category),
            std::string(message)
        });

        std::ostream& out = (level >= LogLevel::Warning) ? std::cerr : std::cout;
        out << "[" << level_to_string(level) << "][" << category << "] " << message << std::endl;
    }

    [[nodiscard]] const LogBuffer& get_buffer() const noexcept {
        return this->buffer;
    }

    void clear_buffer() {
        this->buffer.clear();
    }
};

#endif
